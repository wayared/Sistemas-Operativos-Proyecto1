#include "servidor.h"
#include "logmonitor.h"
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/wait.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <ctype.h>

#define READ_END 0
#define WRITE_END 1

char **parsear_servicios(char *line, char *separador);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *wrapper_journal_exec(void *args) {
    pthread_mutex_lock(&mutex); // Bloquea el mutex antes de acceder a recursos compartidos

    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;

     if (!threadArgs) {
        printf("No se proporcionaron argumentos.\n");
        pthread_mutex_unlock(&mutex); // Desbloquea el mutex antes de salir
        return NULL;
    }

    int tiempo = 20; // Definir tu tiempo aquí

    // Obtener la prioridad y los servicios de los argumentos
    char *prioridad = threadArgs->prioridad;
    char **servicios = threadArgs->servicios;

    // Verificar si se proporcionó una prioridad válida
    if (!prioridad) {
        printf("No se proporcionó una prioridad válida. Utilizando la predeterminada: info.\n");
        prioridad = "info"; // Valor predeterminado
    }

    // Llamar a journal_exec con la prioridad y los servicios
    journal_exec(tiempo, prioridad, servicios);
	
	 pthread_mutex_unlock(&mutex); // Desbloquea el mutex después de acceder a recursos compartidos

    return NULL;
}

void send_message(char *message) {
    int pipe_fd[2]; // Array para los extremos del pipe

    if (pipe(pipe_fd) == -1) { // Crear el pipe
        perror("Error al crear el pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork(); // Crear un proceso hijo

    if (pid < 0) {
        perror("Error al crear el proceso hijo");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Estamos en el proceso hijo
        close(pipe_fd[READ_END]); // Cerramos el extremo de lectura del hijo

        // Escribir en el pipe
        write(pipe_fd[WRITE_END], message, strlen(message) + 1);

        close(pipe_fd[WRITE_END]); // Cerramos el extremo de escritura del hijo
    } else {
        // Estamos en el proceso padre
        close(pipe_fd[WRITE_END]); // Cerramos el extremo de escritura del padre

        char buffer[256]; // Buffer para almacenar el mensaje

        // Leer del pipe
        read(pipe_fd[READ_END], buffer, sizeof(buffer));

        printf("Mensaje recibido en el padre: %s\n", buffer);

        close(pipe_fd[READ_END]); // Cerramos el extremo de lectura del padre

        wait(NULL); // Esperamos a que el proceso hijo termine
    }
}


void dashboard(char *services_par, int segundos, char **argv, int argc, char ***services_ptr) {
    int i;
    extern int optind;
    char **services_internal = NULL;

    send_message("Hola desde el servidor"); 
    /* Comprobar los servicios */
    if (!services_par) {
        fprintf(stderr, "Debe especificar solo 2 servicios\n");
    } else {
        printf("services_par: %s\n", services_par);
        services_internal = parsear_servicios(services_par, ".");
        if (services_internal) {
            printf("servicio_1: %s\n", services_internal[0]);
            printf("servicio_2: %s\n", services_internal[1]);
            *services_ptr = services_internal; // Asignación directa a services_ptr
        }
    }

    /* Comprobar el tiempo de actualizacion*/
    if (segundos) {
        printf("segundos: %d.\n", segundos);
    }

    for (i = optind; i < argc; i++) {
        printf("Ficheros que se enviaran %s\n", argv[i]);
    }

    printf(">> START\n");
}



void journal_exec(int tiempo, char *prioridad, char **servicios) {
    pid_t pid = fork(); // Creamos un nuevo proceso

    if (pid < 0) {
        // Error al crear el proceso hijo
        perror("Error al crear el proceso hijo");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Estamos en el proceso hijo
        char command[256]; // Asegúrate de tener suficiente espacio

        // Construir el comando
        sprintf(command, "journalctl ");

        // Agregar los servicios
        for (int i = 0; servicios[i] != NULL; ++i) {
			strcat(command, "-u ");
            strcat(command, servicios[i]);
            strcat(command, ".service ");
        }

        // Agregar la opción de prioridad y un espacio
        strcat(command, "-p ");
        strcat(command, prioridad);
        strcat(command, " ");

        // Imprime el comando para verificarlo
        printf("Comando final: %s\n", command);

        // Ejecutar el comando con exec
        char *args[] = {"bash", "-c", command, NULL}; // Argumentos para ejecutar en una subshell de Bash
        execvp(args[0], args);

        // Si execvp tiene éxito, el código a continuación no se ejecutará
        perror("Error al ejecutar execvp");
        exit(EXIT_FAILURE);
    } else {
        // Estamos en el proceso padre
        int status;
        waitpid(pid, &status, 0); // Esperamos a que el proceso hijo termine
    }
}
