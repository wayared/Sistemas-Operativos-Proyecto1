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


/* Extremos del pipe */
#define READ_EXT 0
#define WRITE_EXT 1

/* Resultado de los journalctl */

// Declaración de la variable mutex


#define FILE_NAME "/resultados.txt"
void journal_exec(int tiempo, char *prioridad, char **servicios);
void *wrapper_journal_exec(void *args);
void dashboard(char *services_par, int segundos, char **argv, int argc, char ***services_ptr);



//static int fd_pipe[2];
//static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//struct ThreadArgs {
//        char *prioridad;
//        char **servicios;
//    };


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




int main(int argc, char **argv){
    int opt;
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

