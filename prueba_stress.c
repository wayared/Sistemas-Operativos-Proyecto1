#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int num_instances = 5;  // Número de instancias de cada servicio a ejecutar

    for (int i = 0; i < num_instances; ++i) {
        pid_t pid_nginx = fork();

        if (pid_nginx == -1) {
            perror("Error al crear el proceso hijo (nginx)");
            exit(EXIT_FAILURE);
        } else if (pid_nginx == 0) {
            // Estamos en el proceso hijo para nginx
            char *nginx_args[] = {"nginx", NULL};
            
            printf("Ejecutando instancia %d de nginx...\n", i + 1);
            execvp("nginx", nginx_args);

            perror("Error al ejecutar nginx");
            exit(EXIT_FAILURE);
        }

        pid_t pid_apache2 = fork();

        if (pid_apache2 == -1) {
            perror("Error al crear el proceso hijo (apache2)");
            exit(EXIT_FAILURE);
        } else if (pid_apache2 == 0) {
            // Estamos en el proceso hijo para apache2
            char *apache2_args[] = {"apache2", NULL};
            
            printf("Ejecutando instancia %d de apache2...\n", i + 1);
            execvp("apache2", apache2_args);

            perror("Error al ejecutar apache2");
            exit(EXIT_FAILURE);
        }
    }

    // Esperar a que todos los procesos hijos terminen (tanto de nginx como de apache2)
    for (int i = 0; i < num_instances * 2; ++i) {
        wait(NULL);
    }

    return 0;
}