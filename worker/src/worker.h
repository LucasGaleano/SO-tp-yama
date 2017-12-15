#ifndef WORKER_H_
#define WORKER_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/config.h> //config
#include <commons/log.h> //log
#include <commons/collections/list.h>
#include <commons/string.h>
#include <biblioteca/paquetes.h>
#include <biblioteca/estructuras.h>
#include <biblioteca/sockets.h>
#include <fcntl.h>  // O_RDONLY modo de abrir para el open()
#include <sys/stat.h> //fstat()
#include <sys/mman.h>  //mmap()
#include <unistd.h>  //PROT_READ del mmap()
#include <signal.h>

#define PATHLOG "worker.log"
#define PATHCONFIG "/home/utnso/configuraciones/nodo.cfg"
#define TAM_MAX 100
#define PUERTO_REDUCCION_GLOBAL "3405"

typedef struct
{
	int socket_cliente;
	char* palabra;
}paquete_esclavo;

void recibirHandshakeLocal(t_paquete*, int*);

char* getBloque(int);

void signal_capturer(int);

void procesarPaquete(t_paquete*, int*);

void recibirPedido(t_paquete*, int*);

void recibirDatos(t_paquete *, int *);

void transformacion (unsigned int, unsigned int, char*, char*);

void reduccionLocal(char**, int, char*, char*);

FILE* aparear(FILE *[], int);

void iniciarEncargado();

void iniciarEsclavo(char *, char *);

void reduccionGlobal();

int sonTodosVerdaderos(int*, int);

#endif WORKER_H_
