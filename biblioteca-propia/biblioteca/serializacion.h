#ifndef BIBLIOTECA_SERIALIZACION_H_
#define BIBLIOTECA_SERIALIZACION_H_

#include "estructuras.h"
#include <commons/string.h>
#include <commons/collections/list.h>

/*----------------------------------------Serializacion----------------------------------------*/
void 							serializarNumero								(t_paquete*, int);

void 							serializarPalabra								(t_paquete*, char *);

void 							serializarHandshake								(t_paquete *, int);

void 							serializarMensaje								(t_paquete *, char *);

void 							serializarArchvivo								(t_paquete *, char *);

void 							serializarInfoDataNode							(t_paquete *, char *, int, int);

void 							serializarSolicitudLecturaBloque				(t_paquete*, int);
void							serializarSolicitudLecturaBloqueGenerarCopia	(t_paquete *, int, char *, int, char *);
void 							serializarSolicitudLecturaBloqueArchTemp		(t_paquete*, int, int);

void 							serializarBloque								(t_paquete*, char*);
void 							serializarBloqueGenerarCopia					(t_paquete*, void *, char *, int, char *);
void 							serializarBloqueArchTemp						(t_paquete*, char*, int);

void 							serializarSolicitudEscrituraBloque				(t_paquete*, int, int, void *);
void 							serializarRespuestaEscrituraBloque				(t_paquete*, bool, int);

void 							serializarSolicitudTransformacion				(t_paquete *, t_pedidoTransformacion *);
void 							serializarSolicitudReduccionLocal				(t_paquete *, t_pedidoReduccionLocal *);
void 							serializarSolicitudReduccionGlobal				(t_paquete *, t_pedidoReduccionGlobal *);
void 							serializarSolicitudAlmacenadoFinal				(t_paquete *, t_pedidoAlmacenadoFinal *);

void 							serializarIndicacionTransformacion				(t_paquete *, t_indicacionTransformacion *);
void 							serializarIndicacionReduccionLocal				(t_paquete *, t_indicacionReduccionLocal *);
void 							serializarIndicacionReduccionGlobal				(t_paquete *, t_indicacionReduccionGlobal *);
void 							serializarIndicacionAlmacenadoFinal				(t_paquete *, t_indicacionAlmacenadoFinal *);

/*----------------------------------------Deserializacion----------------------------------------*/
int 							deserializarNumero								(t_stream *);

char* 							deserializarPalabra								(t_stream *);

int 							deserializarHandshake							(t_stream *);

char * 							deserializarMensaje								(t_stream *);

void * 							deserializarArchivo								(t_stream *);

t_nodo_info *					deserializarInfoDataNode						(t_stream *);

int								deserializarSolicitudLecturaBloque				(t_stream *);
t_lecturaGenerarCopia * 		deserializarSolicitudLecturaBloqueGenerarCopia	(t_stream *);
t_lecturaArchTemp * 			deserializarSolicitudLecturaBloqueArchTemp		(t_stream *);

void*           				deserializarBloque								(t_stream *);
t_respuestaLecturaGenerarCopia* deserializarBloqueGenerarCopia					(t_stream *);
t_respuestaLecturaArchTemp*		deserializarBloqueArchTemp						(t_stream *);

t_pedidoEscritura*				deserializarSolicitudEscrituraBloque			(t_stream *);
t_respuestaEscritura*			deserializarRespuestaEscrituraBloque			(t_stream *);

t_pedidoTransformacion* 		deserializarSolicitudTransformacion				(t_stream *);
t_pedidoReduccionLocal* 		deserializarSolicitudReduccionLocal				(t_stream *);
t_pedidoReduccionGlobal*		deserializarSolicitudReduccionGlobal			(t_stream *);
t_pedidoAlmacenadoFinal* 		deserializarSolicitudAlmacenadoFinal			(t_stream *);

t_indicacionTransformacion* 	deserializarIndicacionTransformacion			(t_stream *);
t_indicacionReduccionLocal* 	deserializarIndicacionReduccionLocal			(t_stream *);
t_indicacionReduccionGlobal* 	deserializarIndicacionReduccionGlobal			(t_stream *);
t_indicacionAlmacenadoFinal* 	deserializarIndicacionAlmacenadoFinal			(t_stream *);

/*----------------------------------------Funciones auxiliares----------------------------------------*/
void * 							abrirArchivo									(char *, size_t *, FILE **);

void serializarListaNodoBloques(t_paquete * unPaquete, t_list * listaNodoBloque);
t_list * deserializarListaNodoBloques(t_stream *);

#endif /* BIBLIOTECA_SERIALIZACION_H_ */
