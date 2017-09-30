#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <biblioteca/sockets.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "consola.h"

/*---------	---------------Estructuras-------------------------*/
#define PUERTO_FILESYSTEM "3200"

//Tabla de nodos
typedef struct {
	int tamanio;
	int libres;
	t_list * nomNodos; //Lista de nombres de los nodos
	t_list * infoDeNodo; //Lista con cada nodo en particular
} t_tabla_nodo;

//Tabla de nodos y sockets
typedef struct {
	int socket;
	char * nomNodo;
} t_nodoYsocket;

/*------------------------Variables globales-------------------------*/
t_log* logFileSystem;
t_list * listaNodoYSocket;
t_tabla_nodo * tablaNodos;

/*------------------------Procesamiento paquetes-------------------------*/
void 				procesarPaquete					(t_paquete *, int *);
void 				recibirHandshake				(t_paquete *, int *);
void 				recibirInfoNodo					(t_paquete *, int);
void 				recibirError					(t_paquete *);

/*-------------------------Modificar tabla de nodos-------------------------*/
void 				crearTablaDeNodos				(void);
char * 				buscarNodoXSocketYEliminarlo	(int);
void 				eliminarNodoDeTablaDeNodos		(char *);

/*-------------------------Funciones auxiliares-------------------------*/
void 				iniciarServidor					(char*);
void 				mostrarTablas					(void);


#endif /* FILESYSTEM_H_ */
