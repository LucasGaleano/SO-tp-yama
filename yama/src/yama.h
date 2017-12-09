#ifndef YAMA_H_
#define YAMA_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <biblioteca/sockets.h>
#include <biblioteca/estructurasWorkerMaster.h>
#include <commons/string.h>
#include <pthread.h>

#include "tablas.h"
#include <biblioteca/paquetes.h>
#include "balanceador.h"


/*----------------------------------Estructuras----------------------------------*/
typedef struct {
	char * ip;
	char * puerto;
	int retardo;
	char * algoritmo;
	char * puerto_yama;
	int disponibilidad_base;
} t_configuracion;

/*----------------------------------Variables globales----------------------------------*/
t_configuracion * configuracion;

//Socket FS
int socketFS;

//Globales Masters
t_queue* cola_master;

long idJob;

//prefijo archivos temporales
char* prefijo = "temp";

t_list* masterConectados;

int socketFS;
t_queue* cola_master;
long idJob;
t_list* masterConectados;

/*----------------------------------Planificador--------------------------------------------*/
typedef struct{
	int bloqueArchivo;
	t_list* nodosEnLosQueEsta;
} t_nodos_por_bloque;

t_list* tablaPlanificador;

/*----------------------------------Configuracion proyecto----------------------------------*/
t_configuracion * 				leerArchivoDeConfiguracionYAMA     		(char* path);

/*----------------------------------Manejo de conexiones-----------------------------------*/
void    	            		iniciarServidor                    		(char* unPuerto);

/*----------------------------------Procesamiento paquetes-----------------------------------*/
void	 						procesarPaquete							(t_paquete * unPaquete, int * client_socket);
void 							procesarRecibirHandshake				(t_paquete * unPaquete, int * client_socket);
void 							procesarRecibirMensaje					(t_paquete * unPaquete);
void 							procesarRecibirArchivo					(t_paquete * unPaquete);
void 							procesarRecibirError					(t_paquete * unPaquete);
void 							procesarEnviarSolicitudTransformacion	(t_paquete * unPaquete, int * client_socket);
void 							procesarEnviarIndicacionTransformacion	(t_paquete * unPaquete);
void 							procesarTareaCompleta					(t_paquete * unPaquete, int client_socket);

/*----------------------------------Funciones auxiliares----------------------------------*/
long 							generarJob								(void);
void 							destruirConfiguracion					(t_configuracion * configuracion);

#endif /* YAMA_H_ */
