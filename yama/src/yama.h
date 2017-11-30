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

int socketFS;
t_queue* cola_master;

long idJob;

t_list* masterConectados;

//typedef struct {
//	char * ;
//	int disponibilidad_base;
//} t_sockets;

/*------------------------Configuracion proyecto-------------------------*/
t_configuracion *   leerArchivoDeConfiguracionYAMA     (char* path);

/*------------------------Manejo de conexiones-------------------------*/
void                iniciarServidor                    (char* unPuerto);

/*------------------------Procesamiento paquetes-------------------------*/
void 				procesarPaquete					(t_paquete *, int *);
void 				recibirInfoNodo					(t_paquete *, int);
void 				recibirError					(t_paquete *);


void 				enviarRutaArchivo				(int, char *);

long 			    generarJob                      ();

t_indicacionTransformacion* bloqueAT_indicacionTranformacion(int, t_bloque_ubicacion* ,char*);


#endif /* YAMA_H_ */
