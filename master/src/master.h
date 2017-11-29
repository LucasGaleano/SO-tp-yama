#ifndef _MASTER.H_
#define _MASTER.H_

#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <biblioteca/sockets.h>
#include <commons/string.h>
#include <time.h>
#include <biblioteca/estructuras.h>
#include <commons/log.h>

typedef struct
{
	int conexion;
	t_indicacionTransformacion * ind;

}transformacion;

typedef struct
{
	int conexion;
	t_pedidoReduccionLocal * ind;
} reduLocal;

typedef struct
{
	int conexion;
	t_pedidoReduccionGlobal reduGlobal;
} reduGlobal;

int leerConfiguracion();
void gestionarTransformacion();
void mandarDatosTransformacion(transformacion t);
void gestionarReduccionLocal();
void mandarDatosReduccionLocal(reduLocal * reduccion);
void gestionarReduccionGlobal();
void mandarDatosReduccionGlobal();
void signal_capturer(int numeroSenial);
void procesarPaquete(t_paquete * unPaquete, int * client_socket);

// Variables globales

t_log * logMaster;
t_list * pedidosDeTransformacion ;
t_list * pedidosDeReduccionLocal;
t_list * pedidosDeReduccionGlobal;
int tareasRealizadasEnParalelo;
int tareasTotalesReduccionLocal;
int cantidadDeFallos;
char* rutaScriptTransformador ;
char* rutaScriptReductor ;
char* rutaArchivoParaArrancar ;
char* rutaParaAlmacenarArchivo;
int conexionYama;
bool * finDeSolicitudesDeTransformacion;
bool * finDeSolicitudesDeReduccionLocal;
bool * finDeSolicitudesGlobales;
int tareasTransformacion;
int tareasReduccion;
pthread_mutex_t variableTareasReduccionLocal;
pthread_mutex_t variableTareasTransformacion;
pthread_mutex_t mutexMetricas;
bool finDeSolicitudes;


// Resultado de operaciones

typedef struct
{
	float tiempoTotal;
	float promedioJobs;
	int cantMaximaTareasTransformacionParalelas;
	int cantMaximaTareasReduccionLocalParalelas;
	int cantidadTareasTotalesTransformacion;
	int cantidadTareasTotalesReduccionLocal;
	int cantidadTareasTotalesReduccionGlobal;
	int fallosDelJob; // 0 para todo mal y 1 para todo bien D:

} metricas;


metricas tablaMetricas;
float tiempoTransformacion;
float tiempoReduccionLocal;
float tiempoReduccionGlobal;


 // Para los resultados

enum resultado
{
	TODO_OK=0,
	ERROR = 1,
};

#endif
