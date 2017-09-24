#include "serializacion.h"

/*-------------------------Serializacion-------------------------*/
void serializarHandshake(t_paquete * unPaquete, int emisor) {
	int tamMensaje = sizeof(int);

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->data = malloc(tamMensaje);

	unPaquete->buffer->size = tamMensaje;
	memcpy(unPaquete->buffer->data, &emisor, tamMensaje);
}

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

	FILE * archivofd;

	void * archivo = abrirArchivo(rutaArchivo, &tamArch, &archivofd);

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->data = malloc(tamArch);

	unPaquete->buffer->size = tamArch;
	memcpy(unPaquete->buffer->data, archivo, tamArch);

	munmap(archivo, tamArch);
	fclose(archivofd);
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

void * abrirArchivo(char * rutaArchivo, size_t * tamArc, FILE ** archivo) {
	//Copio informacion del archivo
	struct stat statArch;

	stat(rutaArchivo, &statArch);

	//Tamaño del archivo que voy a leer
	*tamArc = statArch.st_size;

	//Abro el archivo
	*archivo = fopen(rutaArchivo, "r");

	//Leo el total del archivo y lo asigno al buffer
	int fd = fileno(*archivo);
	void * dataArchivo = mmap(0, *tamArc, PROT_READ, MAP_SHARED, fd, 0);

	return dataArchivo;
}
