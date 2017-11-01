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
	memcpy(unPaquete->buffer->data, &numBloque, tamNumBloque);
}

void serializarSolicitudEscrituraBloque(t_paquete* unPaquete, void* bloque,
		int numBloque) {

	int tamBloque = strlen((char*) bloque);
	int tamTotal = tamBloque + sizeof(int);
	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamBloque;
	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;
	memcpy(unPaquete->buffer->data + desplazamiento, bloque, tamBloque);
	desplazamiento += tamBloque;
	memcpy(unPaquete->buffer->data + desplazamiento, &numBloque, sizeof(int));
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

void serializarSolicitudTransformacion(t_paquete * unPaquete,
		t_pedidoTransformacion * solicitud) {
	int tamIP = string_length(solicitud->ip) + 1;
	int tamPuerto = string_length(solicitud->puerto) + 1;
	int tamBloque = sizeof(int);
	int tamRutaArchivoTemporal = string_length(solicitud->rutaArchivoTemporal)
			+ 1;

	int tamTotal = tamIP + tamPuerto + tamBloque + tamRutaArchivoTemporal;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;

	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, solicitud->ip, tamIP);
	desplazamiento += tamIP;

	memcpy(unPaquete->buffer->data + desplazamiento, solicitud->puerto,
			tamPuerto);
	desplazamiento += tamPuerto;

	memcpy(unPaquete->buffer->data + desplazamiento, &solicitud->bloque,
			tamBloque);
	desplazamiento += tamBloque;

	memcpy(unPaquete->buffer->data + desplazamiento,
			solicitud->rutaArchivoTemporal, tamRutaArchivoTemporal);

}

void serializarSolicitudReduccionLocal(t_paquete * unPaquete,
		t_pedidoReduccionLocal * solicitud) {
	int tamArchReduc = string_length(solicitud->archivoReduccionLocal) + 1;
	int ramArchTrans = string_length(solicitud->archivoTransformacion) + 1;

	int tamTotal = tamArchReduc + ramArchTrans;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;

	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento,
			solicitud->archivoTransformacion, ramArchTrans);
	desplazamiento += ramArchTrans;

	memcpy(unPaquete->buffer->data + desplazamiento,
			solicitud->archivoReduccionLocal, tamArchReduc);

}

void serializarSolicitudReduccionGlobal(t_paquete * unPaquete,
		t_pedidoReduccionGlobal * solicitud) {

	int tamNodo = string_length(solicitud->nodo) + 1;
	int tamDireccion = string_length(solicitud->direccion) + 1;
	int tamPuerto = string_length(solicitud->puerto) + 1;
	int tamArchivoReduccionPorWorker = string_length(
			solicitud->archivoReduccionPorWorker) + 1;
	int tamWorkerEncargdo = string_length(solicitud->workerEncargdo) + 1;
	int tamArchivoResultadoReduccionGlobal = string_length(
			solicitud->ArchivoResultadoReduccionGlobal) + 1;

	int tamTotal = tamNodo + tamDireccion + tamPuerto
			+ tamArchivoReduccionPorWorker + tamWorkerEncargdo
			+ tamArchivoResultadoReduccionGlobal;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;

	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, solicitud->nodo, tamNodo);
	desplazamiento += tamNodo;

	memcpy(unPaquete->buffer->data + desplazamiento, solicitud->direccion,
			tamDireccion);
	desplazamiento += tamDireccion;

	memcpy(unPaquete->buffer->data + desplazamiento, solicitud->puerto,
			tamPuerto);
	desplazamiento += tamPuerto;

	memcpy(unPaquete->buffer->data + desplazamiento,
			solicitud->archivoReduccionPorWorker, tamArchivoReduccionPorWorker);
	desplazamiento += tamArchivoReduccionPorWorker;

	memcpy(unPaquete->buffer->data + desplazamiento, solicitud->workerEncargdo,
			tamWorkerEncargdo);
	desplazamiento += tamWorkerEncargdo;

	memcpy(unPaquete->buffer->data + desplazamiento,
			solicitud->ArchivoResultadoReduccionGlobal,
			tamArchivoResultadoReduccionGlobal);
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

int deserializarSolicitudLecturaBloque(t_stream* buffer) {

	return *(int*) (buffer->data);

}

t_pedidoEscritura* deserializarSolicitudEscrituraBloque(t_stream* buffer) {

	t_pedidoEscritura* pedido = malloc(sizeof(t_pedidoEscritura));
	int tamData = buffer->size - sizeof(int);
	pedido->data = malloc(tamData);
	memcpy(pedido->data, buffer->data, tamData);
	memcpy(&pedido->numBloque, buffer->data + tamData, sizeof(int));
	return pedido;

}

t_pedidoTransformacion * deserializarSolicitudTransformacion(t_stream * buffer) {
	t_pedidoTransformacion * solicitud = malloc(sizeof(t_pedidoTransformacion));

	int desplazamiento = 0;

	solicitud->ip = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->ip) + 1;

	solicitud->puerto = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->puerto) + 1;

	memcpy(&solicitud->bloque, &buffer->data + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	solicitud->rutaArchivoTemporal = strdup(buffer->data + desplazamiento);

	return solicitud;
}

t_pedidoReduccionLocal * deserializarSolicitudReduccionLocal(t_stream * buffer) {
	t_pedidoReduccionLocal * solicitud = malloc(sizeof(t_pedidoReduccionLocal));

	int desplazamiento = 0;

	solicitud->archivoTransformacion = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->archivoTransformacion) + 1;

	solicitud->archivoReduccionLocal = strdup(buffer->data + desplazamiento);

	return solicitud;
}

t_pedidoReduccionGlobal * deserializarSolicitudReduccionGlobal(
		t_stream * buffer) {
	t_pedidoReduccionGlobal * solicitud = malloc(sizeof(t_pedidoReduccionGlobal));

	int desplazamiento = 0;

	solicitud->direccion = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->direccion) + 1;

	solicitud->puerto = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->puerto) + 1;

	solicitud->archivoReduccionPorWorker = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->archivoReduccionPorWorker) + 1;

	solicitud->workerEncargdo = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->workerEncargdo) + 1;

	solicitud->ArchivoResultadoReduccionGlobal = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->ArchivoResultadoReduccionGlobal) + 1;

	return solicitud;
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
