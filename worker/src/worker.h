#ifndef WORKER_H_
#define WORKER_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/config.h> //config
#include <commons/log.h> //log
#include <commons/collections/list.h>.h> //txt
#include <commons/string.h>
#include <biblioteca/paquetes.h>
#include <biblioteca/estructuras.h>
#include <fcntl.h>  // O_RDONLY modo de abrir para el open()
#include <sys/stat.h> //fstat()
#include <sys/mman.h>  //mmap()
#include <unistd.h>  //PROT_READ del mmap()
#include "worker.h"

/**
* @NAME: transformacion
* @DESC: aplica la transformacion al bloque dado
*/

void transformacion (unsigned int, char*);

char** ordenar (char**, int);

inline int sonTodosVerdaderos(int *, int);

void reduccionLocal(char*[], int);

FILE* aparear(FILE *[], int);

void iniciarEncargado();

void iniciarEsclavo();

#endif WORKER_H_
