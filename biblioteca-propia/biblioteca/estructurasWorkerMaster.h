#ifndef _EWOMA_
#define _EWOMA_

#include <stdio.h>
#include <stdlib.h>



// ESTRUCTURAS DE SOLICITUD DE MASTER PARA LOS WORKERS

typedef struct  {
	char* nodo;
	char* direccion; // IP
	char* puerto;
	short unsigned int bloque;
	unsigned int bytes;
	char* rutaArchivoTemporal;
} indicacionesParaTransformacion;


 typedef struct  {
	char* nodo;
	char* direccion;
	char* puerto;
	char* archivoTemporalDeTransformacion; // Existe
	char* archivoTemporalDeReduccionLocal; // Nuevo
} indicacionesParaReduccionLocal;


typedef struct  {
	char* nodo;
	char* direccion;
	char* puerto;
	char* archivoDeReduccionLocal;
	char* archivoDeReduccionGlobal;
	char encargado;

} indicacionesParaReduccionGlobal;
#endif
