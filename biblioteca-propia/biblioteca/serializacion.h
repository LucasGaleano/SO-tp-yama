#ifndef BIBLIOTECA_SERIALIZACION_H_
#define BIBLIOTECA_SERIALIZACION_H_

#include "estructuras.h"
#include "estructurasMasterYama.h"
#include <commons/string.h>

/*------------------------------Estructuras------------------------------*/
//Info de nodo
typedef struct {
	char * nombre;
	int total;
	int libre;
} t_nodo_info;

/*-------------------------Serializacion-------------------------*/
void 						serializarHandshake						(t_paquete *, int);
void 						serializarMensaje						(t_paquete *, char *);
void 						serializarArchvivo						(t_paquete *, char *);
void 						serializarInfoDataNode					(t_paquete *, char *, int, int);
void 						serializarBloque						(t_paquete* unPaquete, char* bloque);
void 						serializarSolicitudLecturaBloque		(t_paquete*, int);
void 						serializarSolicitudEscrituraBloque		(t_paquete*, void*, int);
void 						serializarSolicitudTransformacion		(t_paquete *, t_pedidoTransformacion *);
void 						serializarSolicitudReduccionLocal		(t_paquete *, t_pedidoReduccionLocal *);
void 						serializarSolicitudReduccionGlobal		(t_paquete *, t_pedidoReduccionGlobal *);

/*-------------------------Deserializacion-------------------------*/
char * 						deserializarMensaje						(t_stream *);
void * 						deserializarArchivo						(t_stream *);
t_nodo_info *				deserializarInfoDataNode				(t_stream *);
void*           			deserializarBloque						(t_stream *);
int 						deserializarSolicitudLecturaBloque		(t_stream *);
t_pedidoEscritura*			deserializarSolicitudEscrituraBloque	(t_stream *);
t_pedidoTransformacion* 	deserializarSolicitudTransformacion		(t_stream *);
t_pedidoReduccionLocal* 	deserializarSolicitudReduccionLocal		(t_stream *);
t_pedidoReduccionGlobal*	deserializarSolicitudReduccionGlobal	(t_stream *);

/*-------------------------Funciones auxiliares-------------------------*/
void * 						abrirArchivo							(char *, size_t *, FILE **);

#endif /* BIBLIOTECA_SERIALIZACION_H_ */
