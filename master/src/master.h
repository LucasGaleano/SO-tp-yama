#ifndef _MASTER_H_
#define _MASTER_H_

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

#define FIN "0"
#define ERROR "-1"
#define SIGUE "1"
#define SALIOBIEN "2"

typedef struct
{
	int conexion;
	t_pedidoTransformacion * ind;

}transformacion;

typedef struct
{
	int conexion;
	t_pedidoReduccionLocal * ind;
} reduLocal;

typedef struct
{
	int conexion;
	t_pedidoReduccionGlobal * reduGlobal;
} reduGlobal;

int leerConfiguracion();
void liberarListas();
void gestionarTransformacion();
void mandarDatosTransformacion(transformacion * t);
void gestionarReduccionLocal();
void mandarDatosReduccionLocal(reduLocal * reduccion);
void gestionarReduccionGlobal();
void mandarDatosReduccionGlobal();
void signal_capturer(int numeroSenial);
void procesarPaquete(t_paquete * unPaquete, int * client_socket);
void gestionarAlmacenadoFinal(t_indicacionAlmacenadoFinal * pedido);
t_log* log_create(char* file, char *program_name, bool is_active_console, t_log_level level);


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
bool finDeSolicitudesDeTransformacion;
bool finDeSolicitudesDeReduccionLocal;
bool finDeSolicitudesGlobales;
bool terminoConError;
int tareasTransformacion;
int tareasReduccion;
pthread_mutex_t variableTareasReduccionLocal;
pthread_mutex_t variableTareasTransformacion;
pthread_mutex_t mutexMetricas;
pthread_mutex_t mutexError;
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


#endif
