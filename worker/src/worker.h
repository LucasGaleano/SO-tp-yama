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

#define PATHLOG "worker.log"
#define PATHCONFIG "configuraciones/nodo.cfg"
#define TAM_MAX 100
#define TAM_BLOQUE 1048576 //1024 * 1024, un mega
#define PUERTO_REDUCCION_GLOBAL "9999"

typedef struct
{
	int socket_cliente;
	char* palabra;
}paquete_esclavo;

void recibirHandshakeLocal(t_paquete*, int*);

void procesarPaquete(t_paquete*, int*);

void recibirSolicitud(t_paquete*, int*);

void transformacion (unsigned int, unsigned int, char*, char*);

inline int sonTodosVerdaderos(int *, int);

void reduccionLocal(char**, int, char*, char*);

FILE* aparear(FILE *[], int);

void iniciarEncargado();

void iniciarEsclavo(char *, char *);

void reduccionGlobal();

#endif WORKER_H_
