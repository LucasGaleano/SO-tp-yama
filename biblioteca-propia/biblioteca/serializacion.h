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
void 				serializarHandshake						(t_paquete *, int);
void 				serializarMensaje						(t_paquete *, char *);
void 				serializarArchvivo						(t_paquete *, char *);
void 				serializarInfoDataNode					(t_paquete *, char *, int, int);
void 				serializarBloque						(t_paquete* unPaquete, char* bloque);
void 				serializarSolicitudLecturaBloque		(t_paquete*, int);
void 				serializarSolicitudEscrituraBloque		(t_paquete*, void*, int);


/*-------------------------Deserializacion-------------------------*/
char * 				deserializarMensaje						(t_stream *);
void * 				deserializarArchivo						(t_stream *);
t_nodo_info *		deserializarInfoDataNode				(t_stream *);
void*           	deserializarBloque						(t_stream* buffer);
int 				deserializarSolicitudLecturaBloque		(t_stream* buffer);
t_pedidoEscritura*	deserializarSolicitudEscrituraBloque	(t_stream* buffer);


/*-------------------------Funciones auxiliares-------------------------*/
void * 			abrirArchivo							(char *, size_t *, FILE **);

#endif /* BIBLIOTECA_SERIALIZACION_H_ */
