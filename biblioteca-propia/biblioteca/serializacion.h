#ifndef BIBLIOTECA_SERIALIZACION_H_
#define BIBLIOTECA_SERIALIZACION_H_

#include "estructuras.h"

/*-------------------------Serializacion-------------------------*/
void 			serializarMensaje				(t_paquete *, char *);

/*-------------------------Deserializacion-------------------------*/
void 			deserializarMensaje				(t_paquete *, void *);

#endif /* BIBLIOTECA_SERIALIZACION_H_ */
