#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int num_instances = 5;  // Número de instancias de cada servicio a ejecutar

    for (int i = 0; i < num_instances; ++i) {
        pid_t pid_ssh = fork();

        if (pid_ssh == -1) {
            perror("Error al crear el proceso hijo (nginx)");
            exit(EXIT_FAILURE);
        } else if (pid_ssh == 0) {
            // Estamos en el proceso hijo para nginx
            char *ssh_args[] = {"ssh", "usuario@localhost", NULL};
            
            printf("Ejecutando instancia %d de ssh...\n", i + 1);
            execvp("ssh", ssh_args);

            perror("Error al ejecutar ssh");
            exit(EXIT_FAILURE);
        }

        pid_t pid_cups = fork();

        if (pid_cups == -1) {
            perror("Error al crear el proceso hijo (sups)");
            exit(EXIT_FAILURE);
        } else if (pid_cups == 0) {
            // Estamos en el proceso hijo para apache2
            char *apache2_args[] = {"cups", NULL};
            
            printf("Ejecutando instancia %d de cups...\n", i + 1);
            execvp("cups", apache2_args);

            perror("Error al ejecutar cups");
            exit(EXIT_FAILURE);
        }
    }

    // Esperar a que todos los procesos hijos terminen (tanto de nginx como de apache2)
    for (int i = 0; i < num_instances * 2; ++i) {
        wait(NULL);
    }

    return 0;
}
