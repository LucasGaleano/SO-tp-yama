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

void serializarBloque(t_paquete* unPaquete, char* bloque) {
	int tamBloque = strlen(bloque);

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamBloque;

	unPaquete->buffer->data = malloc(tamBloque);

	memcpy(unPaquete->buffer->data, bloque, tamBloque);
}

void serializarSolicitudLecturaBloque(t_paquete* unPaquete, int numBloque) {

	int tamNumBloque = sizeof(int);
	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamNumBloque;
	unPaquete->buffer->data = malloc(tamNumBloque);
	memcpy(unPaquete->buffer->data,&numBloque,tamNumBloque);
}


void serializarSolicitudEscrituraBloque(t_paquete* unPaquete,void* bloque ,int numBloque) {

	int tamBloque = strlen((char*)bloque);
	int tamTotal = tamBloque + sizeof(int);
	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamBloque;
	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;
	memcpy(unPaquete->buffer->data + desplazamiento,bloque,tamBloque);
	desplazamiento += tamBloque;
	memcpy(unPaquete->buffer->data + desplazamiento,&numBloque,sizeof(int));
}


void serializarInfoDataNode(t_paquete * unPaquete, char * nombreNodo,
		int bloquesTotales, int bloquesLibres) {
	int lengthNombre = strlen(nombreNodo) + 1;
	int tamBloque = sizeof(int);
	int tamTotal = lengthNombre + tamBloque + tamBloque;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;

	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, nombreNodo, lengthNombre);
	desplazamiento += lengthNombre;

	memcpy(unPaquete->buffer->data + desplazamiento, &bloquesTotales,
			tamBloque);
	desplazamiento += tamBloque;

	memcpy(unPaquete->buffer->data + desplazamiento, &bloquesLibres, tamBloque);
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

t_nodo_info * deserializarInfoDataNode(t_stream * buffer) {
	t_nodo_info * info = malloc(sizeof(t_nodo_info));

	int desplazamiento = 0;

	info->nombre = strdup(buffer->data);

	desplazamiento += (strlen(info->nombre) + 1);

	int tamBloque = sizeof(int);

	memcpy(&info->total, buffer->data + desplazamiento, tamBloque);
	desplazamiento += tamBloque;

	memcpy(&info->libre, buffer->data + desplazamiento, tamBloque);

	return info;
}

void* deserializarBloque(t_stream* buffer) {

	void* bloque = malloc(buffer->size);
	memcpy(bloque, buffer->data, buffer->size);
	return bloque;

}

int deserializarSolicitudLecturaBloque(t_stream* buffer){

	return *(int*)(buffer->data);

}

t_pedidoEscritura* deserializarSolicitudEscrituraBloque(t_stream* buffer){

	t_pedidoEscritura* pedido = malloc(sizeof(t_pedidoEscritura));
	int tamData = buffer->size - sizeof(int);
	pedido->data = malloc(tamData);
	memcpy(pedido->data, buffer->data,tamData);
	memcpy(&pedido->numBloque, buffer->data + tamData, sizeof(int));
	return pedido;

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
