#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <biblioteca/sockets.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "consola.h"
#include "tablas.h"
#include "interfaz.h"

/*---------	---------------Estructuras-------------------------*/
#define PUERTO_FILESYSTEM "3200"

/*------------------------Variables globales-------------------------*/
t_log* logFileSystem;
bool estadoAnterior;

/*------------------------Procesamiento paquetes-------------------------*/
void 				procesarPaquete						(t_paquete *, int *);
void 				procesarHandshake					(t_paquete *, int *);
void 				procesarInfoNodo					(t_paquete *, int);
void 				procesarError						(t_paquete *, int *);
void 				procesarBloqueArchivoTemporal		(t_paquete *);
void				procesarRespuestaEscrituraBloque	(t_paquete *, int);
void 				procesarBloqueGenerarCopia			(t_paquete *);
void 				procesarNombre						(t_paquete *, int *);

/*-------------------------Manejos de estado-------------------------*/
void 				manejoDeEstado						(char *);
void 				ignoroEstadoAnterior				(void);
void 				consideroEstadoAnterior				(void);

/*-------------------------Funciones auxiliares-------------------------*/
void 				iniciarServidor						(char*);

#endif /* FILESYSTEM_H_ */
