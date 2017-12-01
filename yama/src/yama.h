#ifndef YAMA_H_
#define YAMA_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <biblioteca/sockets.h>
//#include <biblioteca/estructurasMasterYama.h>
#include <biblioteca/estructurasWorkerMaster.h>
#include <commons/string.h>
#include <pthread.h>
#include "tablas.h"

typedef struct {
	char * ip;
	char * puerto;
	int retardo;
	char * algoritmo;
	char * puerto_yama;
	int disponibilidad_base;
} t_configuracion;

typedef struct{
	int id;
	int weight;
}t_worker;


typedef struct {
	t_worker* worker;
	t_list* listaBloques;

} t_registro_planificador;

typedef struct {

	t_list* registros;

} t_tablaPlanificador;



int socketFS;
t_queue* cola_master;

long idJob;

t_list* masterConectados;


/*------------------------Configuracion proyecto-------------------------*/
void   leerArchivoDeConfiguracionYAMA     ();

/*------------------------Manejo de conexiones-------------------------*/
void                iniciarServidor                    (char* unPuerto);

/*------------------------Procesamiento paquetes-------------------------*/
void 				procesarPaquete					(t_paquete *, int *);
void 				recibirInfoNodo					(t_paquete *, int);
void 				recibirError					(t_paquete *);


void 				enviarRutaArchivo				(int, char *);

long 			    generarJob                      ();

t_indicacionTransformacion* bloqueAT_indicacionTranformacion(int, t_bloque_ubicacion* ,char*);

t_tabla_planificador* tablaPlanificador_create();
t_tabla_planificador* tablaPlanificador_destroy();

t_worker* worker_create();
t_worker* worker_destroy(); //todo hacer funcion
t_configuracion* config;


#endif /* YAMA_H_ */
