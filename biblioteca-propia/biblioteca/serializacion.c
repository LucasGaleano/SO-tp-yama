#include "serializacion.h"

/*-------------------------Serializacion-------------------------*/
void serializarNumero(t_paquete* unPaquete, int numero) {
	int tamNumero = sizeof(int);

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamNumero;
	unPaquete->buffer->data = malloc(tamNumero);

	memcpy(unPaquete->buffer->data, &numero, tamNumero);
}

void serializarPalabra(t_paquete* unPaquete, char * palabra) {
	int tamPalabra = strlen(palabra) + 1;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->data = malloc(tamPalabra);
	unPaquete->buffer->size = tamPalabra;

	memcpy(unPaquete->buffer->data, palabra, tamPalabra);
}

void serializarHandshake(t_paquete * unPaquete, int emisor) {
	serializarNumero(unPaquete, emisor);
}

void serializarMensaje(t_paquete * unPaquete, char * mensaje) {
	serializarPalabra(unPaquete, mensaje);
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

void serializarInfoDataNode(t_paquete * unPaquete, char * nombreNodo,
		int bloquesTotales, int bloquesLibres) {
	int tamNombre = strlen(nombreNodo) + 1;
	int tamBloque = sizeof(int);
	int tamTotal = tamNombre + tamBloque + tamBloque;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;
	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, nombreNodo, tamNombre);
	desplazamiento += tamNombre;

	memcpy(unPaquete->buffer->data + desplazamiento, &bloquesTotales,
			tamBloque);
	desplazamiento += tamBloque;

	memcpy(unPaquete->buffer->data + desplazamiento, &bloquesLibres, tamBloque);
}

void serializarSolicitudLecturaBloque(t_paquete* unPaquete, int numBloque) {
	serializarNumero(unPaquete, numBloque);
}

void serializarSolicitudLecturaBloqueGenerarCopia(t_paquete * unPaquete,
		int numBloque, char * ruta, char * nodoBuscado, char * nodoAEscribir) {
	int tamNumBloque = sizeof(int);
	int tamRuta = strlen(ruta) + 1;
	int tamNodoBuscado = strlen(nodoBuscado) + 1;
	int tamNodoAEscribir = strlen(nodoAEscribir) + 1;
	int tamTotal = tamNumBloque + tamRuta + tamNodoBuscado + tamNodoAEscribir;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;
	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, &numBloque, tamNumBloque);
	desplazamiento += tamNumBloque;

	memcpy(unPaquete->buffer->data + desplazamiento, ruta, tamRuta);
	desplazamiento += tamRuta;

	memcpy(unPaquete->buffer->data + desplazamiento, nodoBuscado, tamNodoBuscado);
	desplazamiento +=tamNodoBuscado;

	memcpy(unPaquete->buffer->data + desplazamiento, nodoAEscribir, tamNodoAEscribir);
}

void serializarSolicitudLecturaBloqueArchTemp(t_paquete* unPaquete,
		int numBloque, int orden) {
	int tamNumBloque = sizeof(int);
	int tamOrden = sizeof(int);
	int tamTotal = tamNumBloque + tamOrden;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;
	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, &numBloque, tamNumBloque);
	desplazamiento += tamNumBloque;

	memcpy(unPaquete->buffer->data + desplazamiento, &orden, tamOrden);
}

void serializarBloque(t_paquete* unPaquete, char* bloque) {
	serializarPalabra(unPaquete, bloque);
}

void serializarBloqueGenerarCopia(t_paquete * unPaquete, int bloque, char * data,
		char * ruta, char * nombreNodo) {
	int tamBloque = sizeof(int);
	int tamData = strlen(data) + 1;
	int tamRuta = strlen(ruta) + 1;
	int tamNombreNodo = strlen(nombreNodo) + 1;

	int tamTotal = tamBloque + tamData + tamRuta + tamNombreNodo;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;
	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, &bloque, tamBloque);
	desplazamiento += tamBloque;

	memcpy(unPaquete->buffer->data + desplazamiento, data, tamData);
	desplazamiento += tamData;

	memcpy(unPaquete->buffer->data + desplazamiento, ruta, tamRuta);
	desplazamiento += tamRuta;

	memcpy(unPaquete->buffer->data + desplazamiento, nombreNodo, tamNombreNodo);
}

void serializarBloqueArchTemp(t_paquete* unPaquete, char* bloque, int orden) {
	int tamBloque = strlen(bloque) + 1;
	int tamOrden = sizeof(int);

	int tamTotal = tamBloque + tamOrden;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;
	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, bloque, tamBloque);
	desplazamiento += tamBloque;

	memcpy(unPaquete->buffer->data + desplazamiento, &orden, tamOrden);
}

void serializarSolicitudEscrituraBloque(t_paquete* unPaquete, void* bloque,
		int numBloque) {
	int tamBloque = strlen((char*) bloque) + 1;
	int tamNumBloque = sizeof(int);

	int tamTotal = tamBloque + tamNumBloque;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;
	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, bloque, tamBloque);
	desplazamiento += tamBloque;

	memcpy(unPaquete->buffer->data + desplazamiento, &numBloque, tamNumBloque);
}

void serializarRespuestaEscrituraBloque(t_paquete* unPaquete, bool exito,
		int numBloque) {
	int tamExito = sizeof(bool);
	int tamNumBloque = sizeof(int);

	int tamTotal = tamExito + tamNumBloque;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;
	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, &exito, tamExito);
	desplazamiento += tamExito;

	memcpy(unPaquete->buffer->data + desplazamiento, &numBloque, tamNumBloque);
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
	int tamDireccion = string_length(solicitud->ip) + 1;
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

	memcpy(unPaquete->buffer->data + desplazamiento, solicitud->ip,
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

void serializarSolicitudAlmacenadoFinal(t_paquete * unPaquete,
		t_pedidoAlmacenadoFinal * solicitud) {

	int tamDireccion = string_length(solicitud->ip) + 1;
	int tamPuerto = string_length(solicitud->puerto) + 1;
	int tamArchivoReduccionGlobal = string_length(
			solicitud->archivoReduccionGlobal) + 1;

	int tamTotal = tamDireccion + tamPuerto + tamArchivoReduccionGlobal;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;

	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, solicitud->ip,
			tamDireccion);
	desplazamiento += tamDireccion;

	memcpy(unPaquete->buffer->data + desplazamiento, solicitud->puerto,
			tamPuerto);
	desplazamiento += tamPuerto;

	memcpy(unPaquete->buffer->data + desplazamiento,
			solicitud->archivoReduccionGlobal, tamArchivoReduccionGlobal);
}

void serializarIndicacionTransformacion(t_paquete * unPaquete,
		t_indicacionTransformacion * indicacion) {
	int tamNodo = string_length(indicacion->nodo) + 1;
	int tamDireccion = string_length(indicacion->ip) + 1;
	int tamPuerto = string_length(indicacion->puerto) + 1;
	int tamBloque = sizeof(int);
	int tamBytes = sizeof(int);
	int tamRutaArchivoTemporal = string_length(indicacion->rutaArchivoTemporal)
			+ 1;

	int tamTotal = tamNodo + tamDireccion + tamPuerto + tamBloque + tamBytes
			+ tamRutaArchivoTemporal;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;

	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, indicacion->nodo, tamNodo);
	desplazamiento += tamNodo;

	memcpy(unPaquete->buffer->data + desplazamiento, indicacion->ip,
			tamDireccion);
	desplazamiento += tamDireccion;

	memcpy(unPaquete->buffer->data + desplazamiento, indicacion->puerto,
			tamPuerto);
	desplazamiento += tamPuerto;

	memcpy(unPaquete->buffer->data + desplazamiento, &indicacion->bloque,
			tamBloque);
	desplazamiento += tamBloque;

	memcpy(unPaquete->buffer->data + desplazamiento, &indicacion->bytes,
			tamBytes);
	desplazamiento += tamBytes;

	memcpy(unPaquete->buffer->data + desplazamiento,
			indicacion->rutaArchivoTemporal, tamRutaArchivoTemporal);
	desplazamiento += tamRutaArchivoTemporal;

}

void serializarIndicacionReduccionLocal(t_paquete * unPaquete,
		t_indicacionReduccionLocal * indicacion) {
	int tamNodo = string_length(indicacion->nodo) + 1;
	int tamDireccion = string_length(indicacion->ip) + 1;
	int tamPuerto = string_length(indicacion->puerto) + 1;
	int tamArchivoTemporalTransformacion = string_length(
			indicacion->archivoTemporalTransformacion) + 1;
	int tamArchivoTemporalReduccionLocal = string_length(
			indicacion->archivoTemporalReduccionLocal) + 1;

	int tamTotal = tamNodo + tamDireccion + tamPuerto
			+ tamArchivoTemporalTransformacion
			+ tamArchivoTemporalReduccionLocal;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;

	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, indicacion->nodo, tamNodo);
	desplazamiento += tamNodo;

	memcpy(unPaquete->buffer->data + desplazamiento, indicacion->ip,
			tamDireccion);
	desplazamiento += tamDireccion;

	memcpy(unPaquete->buffer->data + desplazamiento, indicacion->puerto,
			tamPuerto);
	desplazamiento += tamPuerto;

	memcpy(unPaquete->buffer->data + desplazamiento,
			indicacion->archivoTemporalTransformacion,
			tamArchivoTemporalTransformacion);
	desplazamiento += tamArchivoTemporalTransformacion;

	memcpy(unPaquete->buffer->data + desplazamiento,
			indicacion->archivoTemporalReduccionLocal,
			tamArchivoTemporalReduccionLocal);
	desplazamiento += tamArchivoTemporalReduccionLocal;
}

void serializarIndicacionReduccionGlobal(t_paquete * unPaquete,
		t_indicacionReduccionGlobal * indicacion) {
	int tamNodo = string_length(indicacion->nodo) + 1;
	int tamDireccion = string_length(indicacion->ip) + 1;
	int tamPuerto = string_length(indicacion->puerto) + 1;
	int tamArchivoDeReduccionLocal = string_length(
			indicacion->archivoDeReduccionLocal) + 1;
	int tamArchivoDeReduccionGlobal = string_length(
			indicacion->archivoDeReduccionGlobal) + 1;
	int tamEncargado = string_length(indicacion->encargado) + 1;

	int tamTotal = tamNodo + tamDireccion + tamPuerto
			+ tamArchivoDeReduccionLocal + tamArchivoDeReduccionGlobal
			+ tamEncargado;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;

	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, indicacion->nodo, tamNodo);
	desplazamiento += tamNodo;

	memcpy(unPaquete->buffer->data + desplazamiento, indicacion->ip,
			tamDireccion);
	desplazamiento += tamDireccion;

	memcpy(unPaquete->buffer->data + desplazamiento, indicacion->puerto,
			tamPuerto);
	desplazamiento += tamPuerto;

	memcpy(unPaquete->buffer->data + desplazamiento,
			indicacion->archivoDeReduccionLocal, tamArchivoDeReduccionLocal);
	desplazamiento += tamArchivoDeReduccionLocal;

	memcpy(unPaquete->buffer->data + desplazamiento,
			indicacion->archivoDeReduccionGlobal, tamArchivoDeReduccionGlobal);
	desplazamiento += tamArchivoDeReduccionGlobal;

	memcpy(unPaquete->buffer->data + desplazamiento, indicacion->encargado,
			tamEncargado);
	desplazamiento += tamEncargado;
}

void serializarIndicacionAlmacenadoFinal(t_paquete * unPaquete,
		t_indicacionAlmacenadoFinal * indicacion) {
	int tamNodo = string_length(indicacion->nodo) + 1;
	int tamIp = string_length(indicacion->ip) + 1;
	int tamPuerto = string_length(indicacion->puerto) + 1;
	int tamRutaArchivoReduccionGlobal = string_length(
			indicacion->rutaArchivoReduccionGlobal) + 1;

	int tamTotal = tamNodo + tamIp + tamPuerto + tamRutaArchivoReduccionGlobal;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;

	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, indicacion->nodo, tamNodo);
	desplazamiento += tamNodo;

	memcpy(unPaquete->buffer->data + desplazamiento, indicacion->ip, tamIp);
	desplazamiento += tamIp;

	memcpy(unPaquete->buffer->data + desplazamiento, indicacion->puerto,
			tamPuerto);
	desplazamiento += tamPuerto;

	memcpy(unPaquete->buffer->data + desplazamiento,
			indicacion->rutaArchivoReduccionGlobal,
			tamRutaArchivoReduccionGlobal);
	desplazamiento += tamRutaArchivoReduccionGlobal;

}

/*-------------------------Deserializacion-------------------------*/
int deserializarNumero(t_stream* buffer) {
	return *(int*) (buffer->data);
}

char * deserializarPalabra(t_stream * buffer) {
	char * palabra = malloc(buffer->size);

	palabra = strdup(buffer->data);

	return palabra;
}

int deserializarHandshake(t_stream * buffer) {
	return deserializarNumero(buffer);
}

char * deserializarMensaje(t_stream * buffer) {
	return deserializarPalabra(buffer);
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

	int tamNumero = sizeof(int);

	memcpy(&info->total, buffer->data + desplazamiento, tamNumero);
	desplazamiento += tamNumero;

	memcpy(&info->libre, buffer->data + desplazamiento, tamNumero);

	return info;
}

int deserializarSolicitudLecturaBloque(t_stream* buffer) {
	return deserializarNumero(buffer);
}

t_lecturaGenerarCopia * deserializarSolicitudLecturaBloqueGenerarCopia(
		t_stream* buffer) {
	t_lecturaGenerarCopia * solicitudLectura = malloc(
			sizeof(t_lecturaGenerarCopia));

	int desplazamiento = 0;

	int tamBloque = sizeof(int);

	memcpy(&solicitudLectura->bloque, buffer->data + desplazamiento, tamBloque);
	desplazamiento += tamBloque;

	solicitudLectura->rutaArchivo = strdup(buffer->data + desplazamiento);
	desplazamiento += (strlen(solicitudLectura->rutaArchivo) + 1);

	solicitudLectura->nodoBuscado = strdup(buffer->data + desplazamiento);
	desplazamiento += (strlen(solicitudLectura->nodoBuscado) + 1);

	solicitudLectura->nodoAEscribir = strdup(buffer->data + desplazamiento);

	return solicitudLectura;
}

t_lecturaArchTemp * deserializarSolicitudLecturaBloqueArchTemp(
		t_stream * buffer) {
	t_lecturaArchTemp * solicitudLectura = malloc(sizeof(t_lecturaArchTemp));

	int tamNumBloque = sizeof(int);
	int tamOrden = sizeof(int);

	int desplazamiento = 0;

	memcpy(&solicitudLectura->numBloque, buffer->data + desplazamiento,
			tamNumBloque);
	desplazamiento += tamNumBloque;

	memcpy(&solicitudLectura->orden, buffer->data + desplazamiento, tamOrden);

	return solicitudLectura;
}

void* deserializarBloque(t_stream* buffer) {
	return (void*) deserializarPalabra(buffer);
}

t_respuestaLecturaGenerarCopia * deserializarBloqueGenerarCopia(
		t_stream* buffer) {
	t_respuestaLecturaGenerarCopia * bloqueGenerarCopia = malloc(
			sizeof(t_respuestaLecturaGenerarCopia));

	int desplazamiento = 0;

	int tamBloque = sizeof(int);

	memcpy(&bloqueGenerarCopia->bloque, buffer->data + desplazamiento, tamBloque);
	desplazamiento +=tamBloque;

	bloqueGenerarCopia->data = strdup(buffer->data + desplazamiento);
	desplazamiento += (strlen(bloqueGenerarCopia->data) + 1);

	bloqueGenerarCopia->rutaArchivo = strdup(buffer->data + desplazamiento);
	desplazamiento += (strlen(bloqueGenerarCopia->rutaArchivo) + 1);

	bloqueGenerarCopia->nodo = strdup(buffer->data + desplazamiento);
	desplazamiento += (strlen(bloqueGenerarCopia->nodo) + 1);

	return bloqueGenerarCopia;
}

t_respuestaLecturaArchTemp * deserializarBloqueArchTemp(t_stream* buffer) {

	t_respuestaLecturaArchTemp* respuesta = malloc(
			sizeof(t_respuestaLecturaArchTemp));

	int desplazamiento = 0;

	respuesta->data = strdup(buffer->data);

	desplazamiento += (strlen(respuesta->data) + 1);

	int tamOrden = sizeof(int);

	memcpy(&respuesta->orden, buffer->data + desplazamiento, tamOrden);

	return respuesta;
}

t_pedidoEscritura* deserializarSolicitudEscrituraBloque(t_stream* buffer) {
	t_pedidoEscritura* pedido = malloc(sizeof(t_pedidoEscritura));

	int desplazamiento = 0;

	pedido->data = strdup(buffer->data);

	desplazamiento += (strlen(pedido->data) + 1);

	int tamNumBloque = sizeof(int);

	memcpy(&pedido->numBloque, buffer->data + desplazamiento, tamNumBloque);

	return pedido;
}

t_respuestaEscritura * deserializarRespuestaEscrituraBloque(t_stream* buffer) {
	t_respuestaEscritura* respuesta = malloc(sizeof(t_pedidoEscritura));
	memcpy(&respuesta->exito, buffer->data, sizeof(bool));
	memcpy(&respuesta->numBloque, buffer->data + sizeof(bool), sizeof(int));
	return respuesta;
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
	t_pedidoReduccionGlobal * solicitud = malloc(
			sizeof(t_pedidoReduccionGlobal));

	int desplazamiento = 0;

	solicitud->ip = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->ip) + 1;

	solicitud->puerto = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->puerto) + 1;

	solicitud->archivoReduccionPorWorker = strdup(
			buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->archivoReduccionPorWorker) + 1;

	solicitud->workerEncargdo = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->workerEncargdo) + 1;

	solicitud->ArchivoResultadoReduccionGlobal = strdup(
			buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->ArchivoResultadoReduccionGlobal) + 1;

	return solicitud;
}

t_pedidoAlmacenadoFinal * deserializarSolicitudAlmacenadoFinal(
		t_stream * buffer) {
	t_pedidoAlmacenadoFinal * solicitud = malloc(
			sizeof(t_pedidoAlmacenadoFinal));

	int desplazamiento = 0;

	solicitud->ip = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->ip) + 1;

	solicitud->puerto = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->puerto) + 1;

	solicitud->archivoReduccionGlobal = strdup(buffer->data + desplazamiento);

	return solicitud;
}

t_indicacionTransformacion * deserializarIndicacionTransformacion(
		t_stream * buffer) {
	t_indicacionTransformacion * indicacion = malloc(
			sizeof(t_indicacionTransformacion));

	int desplazamiento = 0;

	indicacion->nodo = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(indicacion->nodo) + 1;

	indicacion->ip = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(indicacion->ip) + 1;

	indicacion->puerto = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(indicacion->puerto) + 1;

	memcpy(&indicacion->bloque, buffer->data + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(&indicacion->bytes, buffer->data + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	indicacion->rutaArchivoTemporal = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(indicacion->rutaArchivoTemporal) + 1;

	return indicacion;
}

t_indicacionReduccionLocal * deserializarIndicacionReduccionLocal(
		t_stream * buffer) {
	t_indicacionReduccionLocal * indicacion = malloc(
			sizeof(t_indicacionReduccionLocal));

	int desplazamiento = 0;

	indicacion->nodo = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(indicacion->nodo) + 1;

	indicacion->ip = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(indicacion->ip) + 1;

	indicacion->puerto = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(indicacion->puerto) + 1;

	indicacion->archivoTemporalTransformacion = strdup(
			buffer->data + desplazamiento);
	desplazamiento += strlen(indicacion->archivoTemporalTransformacion) + 1;

	indicacion->archivoTemporalReduccionLocal = strdup(
			buffer->data + desplazamiento);

	return indicacion;
}

t_indicacionReduccionGlobal * deserializarIndicacionReduccionGlobal(
		t_stream * buffer) {
	t_indicacionReduccionGlobal * indicacion = malloc(
			sizeof(t_indicacionReduccionGlobal));

	int desplazamiento = 0;

	indicacion->nodo = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(indicacion->nodo) + 1;

	indicacion->ip = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(indicacion->ip) + 1;

	indicacion->puerto = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(indicacion->puerto) + 1;

	indicacion->archivoDeReduccionLocal = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(indicacion->archivoDeReduccionLocal) + 1;

	indicacion->archivoDeReduccionGlobal = strdup(
			buffer->data + desplazamiento);
	desplazamiento += strlen(indicacion->archivoDeReduccionGlobal) + 1;

	indicacion->encargado = strdup(buffer->data + desplazamiento);

	return indicacion;
}

t_indicacionAlmacenadoFinal * deserializarIndicacionAlmacenadoFinal(
		t_stream * buffer) {
	t_indicacionAlmacenadoFinal * indicacion = malloc(
			sizeof(t_indicacionAlmacenadoFinal));

	int desplazamiento = 0;

	indicacion->nodo = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(indicacion->nodo) + 1;

	indicacion->ip = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(indicacion->ip) + 1;

	indicacion->puerto = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(indicacion->puerto) + 1;

	indicacion->rutaArchivoReduccionGlobal = strdup(
			buffer->data + desplazamiento);

	return indicacion;
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

