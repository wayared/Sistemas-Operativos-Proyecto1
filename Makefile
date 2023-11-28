all: init logmonitor prueba_stress

init:
	touch resultados.txt

logmonitor: logmonitor.o
	gcc -o logmonitor logmonitor.o $(DFLAGS) -lpthread

logmonitor.o: logmonitor.c
	gcc -c -o logmonitor.o logmonitor.c $(DFLAGS) -lpthread

prueba_stress: prueba_stress.o
	gcc -o prueba_stress prueba_stress.o $(DFLAGS) -lpthread

prueba_stress.o: prueba_stress.c
	gcc -c -o prueba_stress.o prueba_stress.c $(DFLAGS) -lpthread

clean:
	rm logmonitor *.o resultados.txt

.PHONY: debug
debug: DFLAGS = -g
debug: clean logmonitor
