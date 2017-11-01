#ifndef _EMASYAM_
#define _EMASYAM_

#include <stdio.h>
#include <stdlib.h>

// ESTRUCTURAS DE PETICION DE OPERACIONES DE YAMA PARA WORKER

typedef struct {
	char* ip;
	char* puerto;
	int bloque;
	char* rutaArchivoTemporal;
} t_pedidoTransformacion;

typedef struct {
	char* archivoTransformacion;
	char* archivoReduccionLocal;
} t_pedidoReduccionLocal;

typedef struct {
	char* nodo;
	char* direccion;
	char* puerto;
	char* archivoReduccionPorWorker;
	char* workerEncargdo;
	char* ArchivoResultadoReduccionGlobal;
} t_pedidoReduccionGlobal;

typedef struct {
	char* direccion;
	char* puerto;
	char* archivoReduccionGlobal;
} t_pedidoAlmacenadoFinal;

/*----------------------------------------------------------------------------------------*/

typedef struct {
	char* nodo;
	char* direccion;
	char* puerto;
	int bloque;
	int bytes;
	char* rutaArchivoTemporal;
} t_indicacionTransformacion;

typedef struct {
	char* nodo;
	char* direccion;
	char* puerto;
	char* archivoTemporalDeTransformacion; // Existe
	char* archivoTemporalDeReduccionLocal; // Nuevo
} t_indicacionReduccionLocal;

typedef struct {
	char* nodo;
	char* direccion;
	char* puerto;
	char* archivoDeReduccionLocal;
	char* archivoDeReduccionGlobal;
	char* encargado;
} t_indicacionReduccionGlobal;

typedef struct {
	char* nodo;
	char* ip;
	char* puerto;
	char* rutaArchivoReduccionGlobal;
} indicacionesAlmacenamientoFinal;

#endif
