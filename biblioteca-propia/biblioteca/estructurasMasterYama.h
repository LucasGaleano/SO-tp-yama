#ifndef _EMASYAM_
#define _EMASYAM_

#include <stdio.h>
#include <stdlib.h>



// ESTRUCTURAS DE PETICION DE OPERACIONES DE YAMA PARA WORKER

	typedef struct {

	char* worker;
	char* direccion;
	FILE* archivoTemporal;

} peticionDeTransformacion;

	typedef struct peticionDeReduccionLocal {

		FILE* archivoTransformacion;
		FILE* archivoReduccionLocal;

	} peticionDeReduccionLocal;

	typedef struct{

		char* nodo;
		char* direccion;
		FILE* archivoReduccionPorWorker;
		char workerEncargdo;
		FILE* ArchivoResultadoReduccionGlobal;

	} peticionDeReduccionGlobal;

	typedef struct {

		char* direccion;
		FILE* archivoReduccionGlobal;

	} peticionAlmacenadoFinal;



	typedef struct {

	char* nodo;
	char* direccion;
	FILE* archivoDeReduccionGlobal;

} indicacionesAlmacenamientoFinal;


// RESPUESTA WORKER A YAMA

	typedef struct {


} resultadoTransformacion;

#endif
