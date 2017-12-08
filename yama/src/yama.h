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
t_indicacionTransformacion* 	bloqueAT_indicacionTranformacion		(int tamanio, t_bloque_ubicacion* ubicacion, char* nombreTemp);
long 							generarJob								(void);
void 							destruirConfiguracion					(t_configuracion * configuracion);
#endif /* YAMA_H_ */
