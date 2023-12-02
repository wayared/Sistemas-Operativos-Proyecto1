#ifndef SERVIDOR_H
#define SERVIDOR_H

struct ThreadArgs {
    char *prioridad;
    char **servicios;
};

void *wrapper_journal_exec(void *args);
void dashboard(char *services_par, int segundos, char **argv, int argc, char ***services_ptr);
void journal_exec(int tiempo, char *prioridad, char **servicios);

#endif /* SERVIDOR_H */