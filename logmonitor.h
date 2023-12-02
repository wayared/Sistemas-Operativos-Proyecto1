#ifndef LOGMONITOR_H
#define LOGMONITOR_H
#include <pthread.h>


extern pthread_mutex_t mutex;

/* Funciones utilizadas en logmonitor.c */
void print_help(char *command);
char **parsear_servicios(char *line, char *separador);

#endif /* LOGMONITOR_H */
