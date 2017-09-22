#include "serializacion.h"

/*-------------------------Serializacion-------------------------*/

void serializarMensaje(t_paquete * unPaquete, char * mensaje) {
	int tamMensaje = strlen(mensaje) + 1;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->data = malloc(tamMensaje);

	unPaquete->buffer->size = tamMensaje;
	memcpy(unPaquete->buffer->data, mensaje, tamMensaje);

	free(mensaje);
}

void serializarArchvivo(t_paquete * unPaquete, char * rutaArchivo) {
	size_t tamArch;

	void * archivo = abrirArchivo(rutaArchivo, &tamArch);

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->data = malloc(tamArch);

	unPaquete->buffer->size = tamArch;
	memcpy(unPaquete->buffer->data, archivo, tamArch);

	free(archivo);
}

/*-------------------------Deserializacion-------------------------*/

char * deserializarMensaje(t_stream * buffer) {

	char * mensaje = malloc(buffer->size);

	memcpy(mensaje, buffer->data, buffer->size);

	return mensaje;
}

void * deserializarArchivo(t_stream * buffer) {

	void * archivo = malloc(buffer->size);

	memcpy(archivo, buffer->data, buffer->size);

	return archivo;
}

/*-------------------------Funciones auxiliares-------------------------*/

void * abrirArchivo(char * rutaArchivo, size_t * tamArc) {
	//Copio informacion del archivo
	struct stat statArch;

	stat(rutaArchivo, &statArch);

	//Tamaño del archivo que voy a leer
	*tamArc = statArch.st_size;

	//Abro el archivo
	FILE * archivo = fopen(rutaArchivo, "r");

	//Reservo lugar para copiar el archivo
	void * dataArchivo = malloc(*tamArc);

	//Leo el total del archivo y lo asigno al buffer
	fread(dataArchivo, *tamArc, 1, archivo);

	//Cierro el archivo
	fclose(archivo);

	return dataArchivo;
}
