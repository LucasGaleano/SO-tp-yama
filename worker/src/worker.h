#ifndef WORKER_H_
#define WORKER_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/config.h> //config
#include <commons/log.h> //log
#include <commons/string.h>
#include <biblioteca/paquetes.h>
#include <biblioteca/estructuras.h>
#include <biblioteca/sockets.h>
#include <fcntl.h>  // O_RDONLY modo de abrir para el open()
#include <sys/stat.h> //fstat()
#include <sys/mman.h>  //mmap()
#include <unistd.h>  //PROT_READ del mmap()
#include "worker.h"

/**
* @NAME: transformacion
* @DESC: aplica la transformacion al bloque dado
*/

void recibirHandshake(t_paquete*, int*);

void procesarPaquete(t_paquete*, int*);

void transformacion (unsigned int, char*);

inline int sonTodosVerdaderos(int *, int);

void reduccionLocal(char**, int, char*);

FILE* aparear(FILE *[], int);

void iniciarEncargado();

void iniciarEsclavo(char *, char *);

#endif WORKER_H_
