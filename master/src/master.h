#ifndef _MASTER_H_
#define _MASTER_H_
#define ERROR_SOCKET  -1

//---------------------INCLUDES----------------------------------------------//

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



//------------------------------ ESTRUCTURAS PROPIAS ---------------------------------------//


typedef struct
{
	int conexion;
	t_pedidoReduccionGlobal * reduGlobal;
} reduGlobal;


// --------------------------------- ENCABEZADOS ---------------------------------------------- //

int leerConfiguracionYConectarYama();
void liberarListas();
void inicializarVariablesGlobales();
void imprimirMetricas();
void gestionarTransformacion();
void mandarDatosTransformacion(t_indicacionTransformacion * indicacion);
void mandarDatosReduccionLocal(t_indicacionReduccionLocal * indicacion);
void gestionarReduccionGlobal();
void mandarDatosReduccionGlobal();
void signal_capturer(int numeroSenial);
void procesarPaquete(t_paquete * unPaquete, int * client_socket);
void gestionarAlmacenadoFinal(t_indicacionAlmacenadoFinal * pedido);
void liberarMemoria();
void calcularTiempoTotalTransformacion();
void calcularTiempoTotalReduccionLocal();
void liberarReduGlobal(t_indicacionReduccionGlobal * ind);
void procesarError();


// ------------------------------------- VARIABLES GLOBALES --------------------------------- //

t_log * logMaster;
t_list * pedidosDeTransformacion ;
t_list * indicacionesDeReduccionGlobal;
t_list * tiemposTransformacion;
t_list * tiemposReduccionLocal;
char* rutaScriptTransformador ;
char* rutaScriptReductor ;
char* rutaArchivoParaArrancar ;
char* rutaParaAlmacenarArchivo;
int tareasRealizadasEnParalelo;
int tareasTotalesReduccionLocal;
int tareasTotalesTransformacion;
int cantidadDeFallos;
int conexionYama;
int tareasTransformacionEnParalelo;
int tareasReduccionLocalEnParalelo;
float tiempoTransformacion;
float tiempoReduccionLocal;
float tiempoReduccionGlobal;
bool finDeSolicitudes;
bool finDeSolicitudesDeTransformacion;
bool finDeSolicitudesDeReduccionLocal;
bool finDeSolicitudesGlobales;
bool errorTransformacion;
bool errorReduLocal;
bool errorReduGlobal;
bool errorAlmacenamiento;
bool errorWorker;
bool dejarDeRecibirSolicitudes;

// ------------------------------------- SEMAFOROS E HILOS ----------------------------------------------- //


pthread_mutex_t mutexReduLocal;
pthread_mutex_t mutexTareasParalelasReduccionLocal;
pthread_mutex_t mutexTareasTransformacionEnParalelo;
pthread_mutex_t mutexTransformaciones;
pthread_mutex_t mutexReduccionGlobal;
pthread_mutex_t mutexErrorTransformacion;
pthread_mutex_t mutexErrorReduccionLocal;
pthread_t * hilosTransformacion;
pthread_t * hilosReduccionLocal;


// -------------------------------------- METRICAS ----------------------------------------------- //

typedef struct
{
	float tiempoTotal;
	float promedioJobs;
	float promedioTransformacion;
	float promedioReduccionLocal;
	int cantMaximaTareasTransformacionParalelas;
	int cantMaximaTareasReduccionLocalParalelas;
	int cantidadTareasTotalesTransformacion;
	int cantidadTareasTotalesReduccionLocal;
	int cantidadTareasTotalesReduccionGlobal;
	int cantidadFallosTransformacion;
	int cantidadFallosReduccionLocal;
	int cantidadFallosReduccionGlobal;
	int cantidadFallosAlmacenamiento;

} metricas;


metricas tablaMetricas;

#endif
