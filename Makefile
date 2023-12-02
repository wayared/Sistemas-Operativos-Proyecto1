CC = gcc
CFLAGS = -Wall -Wextra -pthread

all: logmonitor prueba_stress

logmonitor: logmonitor.o servidor.o
	$(CC) -o logmonitor logmonitor.o servidor.o $(CFLAGS)

logmonitor.o: logmonitor.c logmonitor.h servidor.h
	$(CC) -c logmonitor.c -o logmonitor.o $(CFLAGS)

prueba_stress: prueba_stress.o
	$(CC) -o prueba_stress prueba_stress.o $(CFLAGS)

prueba_stress.o: prueba_stress.c
	$(CC) -c prueba_stress.c -o prueba_stress.o $(CFLAGS)

servidor.o: servidor.c servidor.h logmonitor.h
	$(CC) -c servidor.c -o servidor.o $(CFLAGS)

clean:
	rm -f logmonitor prueba_stress *.o resultados.txt

.PHONY: debug
debug: CFLAGS += -g
debug: clean logmonitor prueba_stress
