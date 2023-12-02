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

/* Extremos del pipe */
#define READ_EXT 0
#define WRITE_EXT 1

/* Resultado de los journalctl */
#define FILE_NAME "/resultados.txt"

pthread_mutex_t mutex; // Declaración del mutex global

static int fd_pipe[2],fd_pipe1[2],fd_pipe2[2];
//static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct ThreadArgs {
        char *prioridad;
        char **servicios;
    };


/* monlog -s servicio1,servicio2 -t 1 */
void print_help(char *command)
{
	fprintf(stderr,"Monitoreo en tiempo real de los archivos logs de un sistema GNU/Linux.\n");
	fprintf(stderr,"uso:\n %s -s servicio1|servicio2 [OPCIONES]\n", command);
	fprintf(stderr,"uso:\n %s -p  [PRIORIDAD]\n", command);
	fprintf(stderr,"Prioridades: alert,	err, notice, info, debug\n");
	fprintf(stderr," %s -h\n", command);
	fprintf(stderr,"OPCIONES:\n");
	fprintf(stderr," -t [tiempo]\t\t\tSetea el tiempo de actualizacion (segundos), tiempo debe ser mayor de 0 y por defecto es 1.\n");
	fprintf(stderr," -h  \t\t\tmuestra mensaje de ayuda\n");
	exit(2);
	
}

/* Función para parsear servicios */
char **parsear_servicios(char *line, char *separador){
	char *token, *saveptr, *saveptr1;
	char *line_copy;
	int i, token_count = 0;
	char **service_names = NULL;
	
	line_copy = (char *) malloc(strlen(line) + 1);
	strcpy(line_copy, line);
	
	token = strtok_r(line_copy, separador, &saveptr);
	while(token != NULL){
		token = strtok_r(NULL, separador, &saveptr);
		token_count++;
	}
	free(line_copy);
	
	if(token_count > 0){
		service_names = (char **) malloc((token_count + 1) * sizeof(char **));
		token = strtok_r(line, separador, &saveptr1);
		for(i = 0; i < token_count; i++){
			service_names[i] = (char *) malloc(strlen(token) + 1);
			strcpy(service_names[i], token);
			token = strtok_r(NULL, separador, &saveptr1);
		}
		service_names[i] = NULL;
	}
	
	return service_names;
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

void dashboard(char *services_par, int segundos, char **argv, int argc, char ***services_ptr) {
    int i;
    extern int optind;
    char **services_internal = NULL; 
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

int main(int argc, char **argv){
    int opt, i, j;
    int segundos = 20;
    char *services_par = NULL;
	char *prioridad = NULL;
    char **services = NULL;

    /* Variables globales de getopt */
    extern char* optarg;

    extern int optopt;
  
    while((opt = getopt(argc, argv, "ht:s:p:")) != -1){
        switch(opt){
            case 'h':
                print_help(argv[0]);
                break;
            case 's':
                services_par = optarg;
                break;
            case 't':
                segundos = atoi(optarg);
                break;
			case 'p':
                prioridad = optarg;
                break;
            case '?':
                if(optopt == 's'){
                    fprintf(stderr, "optopt %c requiere argumentos\n", optopt);
                } else if(optopt == 't' && isprint(optopt)){
                    printf("Los segundos por defecto son 20.\n");
                    break;
                } else if(isprint(optopt)){
                    fprintf(stderr, "Opcion desconocida '-%c'.\n",optopt);
                } else {
                    fprintf(stderr, "ERR>> Caracter desconocido '\\x%x'.\n",optopt);
                }
                print_help(argv[0]);
        }       
    }
    
    dashboard(services_par, segundos, argv, argc, &services);

     while (1) { // Bucle infinito
    pthread_t hilos;

	pthread_mutex_init(&mutex, NULL); // Inicializa el mutex

    // Crear una estructura para almacenar prioridad y servicios
    struct ThreadArgs threadArgs;
    threadArgs.prioridad = prioridad; // La prioridad capturada con -p
    threadArgs.servicios = services;  // Los servicios obtenidos


    // Crear un hilo y pasar la estructura ThreadArgs como argumento
    pthread_create(&hilos, NULL, wrapper_journal_exec, (void *)&threadArgs);
    
    /* Esperando hilos */
  
    pthread_join(hilos, NULL);
 
	pthread_mutex_destroy(&mutex); // Destruye el mutex

    sleep(segundos);
    }

    
    printf(">> FIN\n");
    
    return 0;
}

