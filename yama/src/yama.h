#ifndef YAMA_H_
#define YAMA_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <biblioteca/sockets.h>
#include <biblioteca/estructurasMasterYama.h>
#include <biblioteca/estructurasWorkerMaster.h>
#include <commons/string.h>
#include <pthread.h>

typedef struct {
	int job;
	int master;
	int nodo;
	int bloque;
	int etapa;
	char * archivoTemporal;
	int estado;
} t_elemento_tabla_estado;

typedef struct {
	char * ip;
	char * puerto;
	int retardo;
	char * algoritmo;
	char * puerto_yama;
} t_configuracion;

/*------------------------Configuracion proyecto-------------------------*/
t_configuracion *   leerArchivoDeConfiguracionYAMA     (char* path);

/*------------------------Manejo de tabla-------------------------*/
int                 actualizarTablaDeEstados     (t_list* tabla_de_estados, t_elemento_tabla_estado fila_nueva);
int                 numeroRandom();

/*------------------------Manejo de conexiones-------------------------*/
void                iniciarServidor                    (char* unPuerto);

/*------------------------Procesamiento paquetes-------------------------*/
void 				procesarPaquete					(t_paquete *, int *);
void 				recibirHandshake				(t_paquete *, int *);
void 				recibirInfoNodo					(t_paquete *, int);
void 				recibirError					(t_paquete *);



#endif /* YAMA_H_ */
