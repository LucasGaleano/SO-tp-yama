#ifndef BIBLIOTECA_SERIALIZACION_H_
#define BIBLIOTECA_SERIALIZACION_H_

#include "estructuras.h"

/*-------------------------Serializacion-------------------------*/
void 			serializarMensaje				(t_paquete *, char *);
void 			serializarArchvivo				(t_paquete *, char *);

/*-------------------------Deserializacion-------------------------*/
char * 			deserializarMensaje				(t_stream *);
void * 			deserializarArchivo				(t_stream *);

/*-------------------------Funciones auxiliares-------------------------*/
void * 			abrirArchivo					(char *, size_t *, FILE **);

#endif /* BIBLIOTECA_SERIALIZACION_H_ */
