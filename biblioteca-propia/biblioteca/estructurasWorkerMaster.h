#ifndef _EWOMA_
#define _EWOMA_

#include <stdio.h>
#include <stdlib.h>



// ESTRUCTURAS DE SOLICITUD DE MASTER PARA LOS WORKERS

typedef struct  {
	char* nodo;
	char* direccion; // IP : PUERTO
	short unsigned int bloque;
	unsigned int bytes;
	FILE* archivoTemporal;
} indicacionesParaTransformacion;


 typedef struct  {
	char* nodo;
	char* direccion;
	FILE* archivoTemporalDeTransformacion; // Existe
	FILE* archivoTemporalDeReduccionLocal; // Nuevo
} indicacionesParaReduccionLocal;


typedef struct  {
	char* nodo;
	char* direccion;
	FILE* archivoDeReduccionLocal;
	FILE* archivoDeReduccionGlobal;
	char encargado;

} indicacionesParaReduccionGlobal;
#endif
