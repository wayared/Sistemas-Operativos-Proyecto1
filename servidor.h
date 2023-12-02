#ifndef SERVIDOR_H
#define SERVIDOR_H
#include <pthread.h>

/* Estructura para los argumentos del hilo */
struct ThreadArgs {
    char *prioridad;
    char **servicios;
};

extern pthread_mutex_t mutex;

void *wrapper_journal_exec(void *args);
void dashboard(char *services_par, int segundos, char **argv, int argc, char ***services_ptr);
void journal_exec(int tiempo, char *prioridad, char **servicios);
void send_message(char *message);

#endif /* SERVIDOR_H */