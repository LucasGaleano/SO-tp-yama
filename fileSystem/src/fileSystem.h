#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <biblioteca/sockets.h>
#include <commons/log.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "consola.h"

/*---------	---------------Variables globales-------------------------*/
t_log* logFileSystem;

/*---------	---------------Estructuras-------------------------*/
#define PUERTO_FILESYSTEM "3200"

/*---------	---------------Procesamiento paquetes-------------------------*/
void 				procesarPaquete					(t_paquete *, int *);
void 				recibirHandshake				(t_paquete *, int *);

/*-------------------------Funciones auxiliares-------------------------*/
void 				iniciarServidor					(char*);

#endif /* FILESYSTEM_H_ */
