#include "serializacion.h"

/*-------------------------Serializacion-------------------------*/

void serializarMensaje(t_paquete * unPaquete, char * mensaje) {
	int tamMensaje = strlen(mensaje);

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->data = malloc(tamMensaje);

	unPaquete->buffer->size = tamMensaje;
	memcpy(unPaquete->buffer->data, mensaje, tamMensaje);
}

/*-------------------------Deserializacion-------------------------*/

void deserializarMensaje(t_paquete * unPaquete, void * buffer) {

	int desplazamiento = 0;
	int tamSize = sizeof(t_stream);

	unPaquete->buffer = malloc(sizeof(t_stream));

	memcpy(&unPaquete->buffer->size, buffer, tamSize);
	desplazamiento += tamSize;

	int tamData = unPaquete->buffer->size;

	unPaquete->buffer->data = malloc(tamData);
	memcpy(unPaquete->buffer->data, buffer, tamData);

	free(buffer);
}
