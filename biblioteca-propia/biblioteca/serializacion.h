#ifndef BIBLIOTECA_SERIALIZACION_H_
#define BIBLIOTECA_SERIALIZACION_H_

#include "estructuras.h"

/*------------------------------Estructuras------------------------------*/
//Info de nodo
typedef struct {
	char * nombre;
	int total;
	int libre;
} t_nodo_info;

/*-------------------------Serializacion-------------------------*/
void 			serializarHandshake				(t_paquete *, int);
void 			serializarMensaje				(t_paquete *, char *);
void 			serializarArchvivo				(t_paquete *, char *);
void 			serializarInfoDataNode			(t_paquete *, char *, int, int);

/*-------------------------Deserializacion-------------------------*/
char * 			deserializarMensaje				(t_stream *);
void * 			deserializarArchivo				(t_stream *);
t_nodo_info *	deserializarInfoDataNode		(t_stream *);

/*-------------------------Funciones auxiliares-------------------------*/
void * 			abrirArchivo					(char *, size_t *, FILE **);

#endif /* BIBLIOTECA_SERIALIZACION_H_ */
