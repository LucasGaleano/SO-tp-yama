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

/*----------------------------------------------------------------------------------------*/

typedef struct {
	char* direccion;
	char* puerto;
	char* archivoReduccionGlobal;
} peticionAlmacenadoFinal;

typedef struct {
	char* nodo;
	char* direccion; // IP
	char* puerto;
	short unsigned int bloque;
	unsigned int bytes;
	char* rutaArchivoTemporal;
} indicacionesParaTransformacion;

typedef struct {
	char* nodo;
	char* direccion;
	char* puerto;
	char* archivoTemporalDeTransformacion; // Existe
	char* archivoTemporalDeReduccionLocal; // Nuevo
} indicacionesParaReduccionLocal;

typedef struct {
	char* nodo;
	char* direccion;
	char* puerto;
	char* archivoDeReduccionLocal;
	char* archivoDeReduccionGlobal;
	char encargado;
} indicacionesParaReduccionGlobal;

typedef struct {
	char* nodo;
	char* ip;
	char* puerto;
	char* rutaArchivoReduccionGlobal;
} indicacionesAlmacenamientoFinal;
#endif
