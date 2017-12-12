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
		int numBloqueNodo, char * rutaArchivo, int numBloqueArchivo,
		char * nomNodoAEscribir) {

	int tamNumBloqueNodo = sizeof(int);
	int tamRutaArchivo = strlen(rutaArchivo) + 1;
	int tamNumBloqueArchivo = sizeof(int);
	int tamNomNodoAEscribir = strlen(nomNodoAEscribir) + 1;

	int tamTotal = tamNumBloqueNodo + tamRutaArchivo + tamNumBloqueArchivo
			+ tamNomNodoAEscribir;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;
	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, &numBloqueNodo,
			tamNumBloqueNodo);
	desplazamiento += tamNumBloqueNodo;

	memcpy(unPaquete->buffer->data + desplazamiento, rutaArchivo,
			tamRutaArchivo);
	desplazamiento += tamRutaArchivo;

	memcpy(unPaquete->buffer->data + desplazamiento, &numBloqueArchivo,
			tamNumBloqueArchivo);
	desplazamiento += tamNumBloqueArchivo;

	memcpy(unPaquete->buffer->data + desplazamiento, nomNodoAEscribir,
			tamNomNodoAEscribir);
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

void serializarBloqueGenerarCopia(t_paquete* unPaquete, void * data,
		char * rutaArchivo, int numBloqueArchivo, char * nomNodoAEscribir) {
	int tamRutaArchivo = strlen(rutaArchivo) + 1;
	int tamNumBloqueArchivo = sizeof(int);
	int tamNomNodoAEscribir = strlen(nomNodoAEscribir) + 1;

	int tamTotal = TAM_BLOQUE + tamRutaArchivo + tamNumBloqueArchivo
			+ tamNomNodoAEscribir;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;
	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, data, TAM_BLOQUE);
	desplazamiento += TAM_BLOQUE;

	memcpy(unPaquete->buffer->data + desplazamiento, rutaArchivo,
			tamRutaArchivo);
	desplazamiento += tamRutaArchivo;

	memcpy(unPaquete->buffer->data + desplazamiento, &numBloqueArchivo,
			tamNumBloqueArchivo);
	desplazamiento += tamNumBloqueArchivo;

	memcpy(unPaquete->buffer->data + desplazamiento, nomNodoAEscribir,
			tamNomNodoAEscribir);
	desplazamiento += tamNomNodoAEscribir;

}

void serializarBloqueArchTemp(t_paquete* unPaquete, char* bloque, int orden) {
	int tamBloque = TAM_BLOQUE;
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

void serializarSolicitudEscrituraBloque(t_paquete* unPaquete,
		int bloqueAEscribir, int size, void* data) {
	int tamBloqueAEscribir = sizeof(int);
	int tamTamBuffer = sizeof(int);

	int tamTotal = tamBloqueAEscribir + tamTamBuffer + size;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;
	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, &bloqueAEscribir,
			tamBloqueAEscribir);
	desplazamiento += tamBloqueAEscribir;

	memcpy(unPaquete->buffer->data + desplazamiento, &size, tamTamBuffer);
	desplazamiento += tamTamBuffer;

	memcpy(unPaquete->buffer->data + desplazamiento, data, size);
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
	int tamRuta = string_length(solicitud->rutaScriptTransformacion) + 1;
	int tamNumBloque = sizeof(int);
	int tamCantBytes = sizeof(int);
	int tamRutaArchivoTemporal = string_length(solicitud->rutaArchivoTemporal)
			+ 1;

	int tamTotal = tamRuta + tamNumBloque + tamCantBytes
			+ tamRutaArchivoTemporal;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;

	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento,
			solicitud->rutaScriptTransformacion, tamRuta);
	desplazamiento += tamRuta;

	memcpy(unPaquete->buffer->data + desplazamiento, &solicitud->numBloque,
			tamNumBloque);
	desplazamiento += tamNumBloque;

	memcpy(unPaquete->buffer->data + desplazamiento, &solicitud->cantBytes,
			tamCantBytes);
	desplazamiento += tamCantBytes;

	memcpy(unPaquete->buffer->data + desplazamiento,
			solicitud->rutaArchivoTemporal, tamRutaArchivoTemporal);
}

void serializarSolicitudReduccionLocal(t_paquete * unPaquete,
		t_pedidoReduccionLocal * solicitud) {
	int tamArchReduc = string_length(solicitud->archivoReduccionLocal) + 1;
	int ramArchTrans = string_length(solicitud->archivoTransformacion) + 1;
	int tamRutaScript = string_length(solicitud->rutaScript) + 1;

	int tamTotal = tamArchReduc + ramArchTrans + tamRutaScript;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;

	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento,
			solicitud->archivoTransformacion, ramArchTrans);
	desplazamiento += ramArchTrans;

	memcpy(unPaquete->buffer->data + desplazamiento,
			solicitud->archivoReduccionLocal, tamArchReduc);
	desplazamiento += tamArchReduc;

	memcpy(unPaquete->buffer->data + desplazamiento,
			solicitud->archivoReduccionLocal, tamRutaScript);

}

void serializarSolicitudReduccionGlobal(t_paquete * unPaquete,
		t_pedidoReduccionGlobal * solicitud) {

	int tamDireccion = string_length(solicitud->ip) + 1;
	int tamPuerto = string_length(solicitud->puerto) + 1;
	int tamArchivoReduccionPorWorker = string_length(
			solicitud->archivoReduccionPorWorker) + 1;
	int tamWorkerEncargdo = sizeof(int);
	int tamArchivoResultadoReduccionGlobal = string_length(
			solicitud->ArchivoResultadoReduccionGlobal) + 1;
	int cantidadWorker = sizeof(int);

	int tamTotal = cantidadWorker + tamDireccion + tamPuerto
			+ tamArchivoReduccionPorWorker + tamWorkerEncargdo
			+ tamArchivoResultadoReduccionGlobal;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;

	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento,
			&solicitud->cantWorkerInvolucradros, cantidadWorker);
	desplazamiento += cantidadWorker;

	memcpy(unPaquete->buffer->data + desplazamiento, solicitud->ip,
			tamDireccion);
	desplazamiento += tamDireccion;

	memcpy(unPaquete->buffer->data + desplazamiento, solicitud->puerto,
			tamPuerto);
	desplazamiento += tamPuerto;

	memcpy(unPaquete->buffer->data + desplazamiento,
			solicitud->archivoReduccionPorWorker, tamArchivoReduccionPorWorker);
	desplazamiento += tamArchivoReduccionPorWorker;

	memcpy(unPaquete->buffer->data + desplazamiento,
			&solicitud->workerEncargado, tamWorkerEncargdo);
	desplazamiento += tamWorkerEncargdo;

	memcpy(unPaquete->buffer->data + desplazamiento,
			solicitud->ArchivoResultadoReduccionGlobal,
			tamArchivoResultadoReduccionGlobal);
}

void serializarSolicitudAlmacenadoFinal(t_paquete * unPaquete,
		t_pedidoAlmacenadoFinal * solicitud) {

	int tamArchivoReduccionGlobal = string_length(
			solicitud->archivoReduccionGlobal) + 1;
	int tamRutaFinal = string_length(solicitud->rutaAlmacenadoFinal) + 1;

	int tamTotal = tamRutaFinal + tamArchivoReduccionGlobal;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;

	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento,
			solicitud->archivoReduccionGlobal, tamArchivoReduccionGlobal);
	desplazamiento += tamArchivoReduccionGlobal;

	memcpy(unPaquete->buffer->data + desplazamiento,
			solicitud->archivoReduccionGlobal, tamRutaFinal);
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
	int tamEncargado = sizeof(int);

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

	memcpy(unPaquete->buffer->data + desplazamiento, &indicacion->encargado,
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

void serializarListaNodoBloques(t_paquete * unPaquete,
		t_nodos_bloques * listaNodoBloque, int masterSolicitante) {

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = 0;
	int desplazamiento = 0;

	//Cantidad de elementos de las listas
	int cantNodoBloque = listaNodoBloque->nodoBloque->elements_count;
	int cantPuertoIP = listaNodoBloque->puertoIP->elements_count;

	//Serializo nodo-bloque
	unPaquete->buffer->data = malloc(sizeof(int));

	memcpy(unPaquete->buffer->data, &cantNodoBloque, sizeof(int));

	desplazamiento += sizeof(int);
	unPaquete->buffer->size += desplazamiento;

	void serializarNodoBloque(t_nodo_bloque * nodoBloque) {

		int tamNomNodo = strlen(nodoBloque->nomNodo) + 1;
		int tamNum = sizeof(int);

		int tamNodoBloque = tamNomNodo + (tamNum * 4);

		unPaquete->buffer->size += tamNodoBloque;

		unPaquete->buffer->data = realloc(unPaquete->buffer->data,
				unPaquete->buffer->size);

		memcpy(unPaquete->buffer->data + desplazamiento, nodoBloque->nomNodo,
				tamNomNodo);
		desplazamiento += tamNomNodo;

		memcpy(unPaquete->buffer->data + desplazamiento,
				&nodoBloque->bloqueNodo, tamNum);
		desplazamiento += tamNum;

		memcpy(unPaquete->buffer->data + desplazamiento,
				&nodoBloque->bloqueArchivo, tamNum);
		desplazamiento += tamNum;

		memcpy(unPaquete->buffer->data + desplazamiento,
				&nodoBloque->originalidad, tamNum);
		desplazamiento += tamNum;

		memcpy(unPaquete->buffer->data + desplazamiento, &nodoBloque->tamanio,
				tamNum);
		desplazamiento += tamNum;
	}

	list_iterate(listaNodoBloque->nodoBloque, (void*) serializarNodoBloque);

	//Serializo puertoIp
	unPaquete->buffer->data = realloc(unPaquete->buffer->data,
			unPaquete->buffer->size + sizeof(int));

	memcpy(unPaquete->buffer->data, &cantPuertoIP, sizeof(int));

	desplazamiento += sizeof(int);
	unPaquete->buffer->size += desplazamiento;

	void serializarPuertoIP(t_puerto_ip * puertoIp) {

		int tamNomNodo = strlen(puertoIp->nomNodo) + 1;
		int tamPuerto = strlen(puertoIp->puerto) + 1;
		int tamIp = strlen(puertoIp->ip) + 1;

		int tamPuertoIp = tamNomNodo + tamPuerto + tamIp;

		unPaquete->buffer->size += tamPuertoIp;

		unPaquete->buffer->data = realloc(unPaquete->buffer->data,
				unPaquete->buffer->size);

		memcpy(unPaquete->buffer->data + desplazamiento, puertoIp->nomNodo,
				tamNomNodo);
		desplazamiento += tamNomNodo;

		memcpy(unPaquete->buffer->data + desplazamiento, puertoIp->puerto,
				tamPuerto);
		desplazamiento += tamPuerto;

		memcpy(unPaquete->buffer->data + desplazamiento, puertoIp->ip, tamIp);
		desplazamiento += tamIp;
	}
	list_iterate(listaNodoBloque->puertoIP, (void*) serializarPuertoIP);

	//Serializo el socket del master solicitante
	unPaquete->buffer->size += sizeof(int);

	unPaquete->buffer->data = realloc(unPaquete->buffer->data,
			unPaquete->buffer->size);

	memcpy(unPaquete->buffer->data + desplazamiento, &masterSolicitante,
			sizeof(int));

}

void serializarNombreIPPuerto(t_paquete * unPaquete, char * nomNodo, char * ip,
		char * puerto) {
	int tamNomNodo = strlen(nomNodo) + 1;
	int tamIp = strlen(ip) + 1;
	int tamPuerto = strlen(puerto) + 1;

	int tamTotal = tamNomNodo + tamIp + tamPuerto;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;

	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, nomNodo, tamNomNodo);
	desplazamiento += tamNomNodo;

	memcpy(unPaquete->buffer->data + desplazamiento, ip, tamIp);
	desplazamiento += tamIp;

	memcpy(unPaquete->buffer->data + desplazamiento, puerto, tamPuerto);
}

void serializarRutaArchivo(t_paquete * unPaquete, char * rutaArchivo,
		int masterSolicitante) {
	int tamRutaArchivo = strlen(rutaArchivo) + 1;
	int tamMasterSolicitante = sizeof(int);

	int tamTotal = tamRutaArchivo + tamMasterSolicitante;

	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = tamTotal;

	unPaquete->buffer->data = malloc(tamTotal);

	int desplazamiento = 0;

	memcpy(unPaquete->buffer->data + desplazamiento, rutaArchivo,
			tamRutaArchivo);
	desplazamiento += tamRutaArchivo;

	memcpy(unPaquete->buffer->data + desplazamiento, &masterSolicitante,
			tamMasterSolicitante);
}

/*-------------------------Deserializacion-------------------------*/
int deserializarNumero(t_stream* buffer) {
	return *(int*) (buffer->data);
}

char * deserializarPalabra(t_stream * buffer) {
	char * palabra = strdup(buffer->data);

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
		t_stream * buffer) {

	t_lecturaGenerarCopia * solicitudLectura = malloc(
			sizeof(t_lecturaGenerarCopia));

	int desplazamiento = 0;

	memcpy(&solicitudLectura->numBloqueNodo, buffer->data + desplazamiento,
			sizeof(int));
	desplazamiento += sizeof(int);

	solicitudLectura->rutaArchivo = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(solicitudLectura->rutaArchivo) + 1;

	memcpy(&solicitudLectura->numBloqueArchivo, buffer->data + desplazamiento,
			sizeof(int));
	desplazamiento += sizeof(int);

	solicitudLectura->nomNodoAEscribir = strdup(buffer->data + desplazamiento);

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

	bloqueGenerarCopia->data = malloc(TAM_BLOQUE);

	memcpy(bloqueGenerarCopia->data, buffer->data + desplazamiento,
	TAM_BLOQUE);
	desplazamiento += TAM_BLOQUE;

	bloqueGenerarCopia->rutaArchivo = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(bloqueGenerarCopia->rutaArchivo) + 1;

	memcpy(&bloqueGenerarCopia->numBloqueArchivo, buffer->data + desplazamiento,
			sizeof(int));
	desplazamiento += sizeof(int);

	bloqueGenerarCopia->nomNodoAEscribir = strdup(
			buffer->data + desplazamiento);

	return bloqueGenerarCopia;
}

t_respuestaLecturaArchTemp * deserializarBloqueArchTemp(t_stream* buffer) {

	t_respuestaLecturaArchTemp* respuesta = malloc(
			sizeof(t_respuestaLecturaArchTemp));

	int desplazamiento = 0;

	respuesta->data = malloc(TAM_BLOQUE);

	memcpy(respuesta->data, buffer->data + desplazamiento, TAM_BLOQUE);
	desplazamiento += TAM_BLOQUE;

	memcpy(&respuesta->orden, buffer->data + desplazamiento, sizeof(int));

	return respuesta;
}

t_pedidoEscritura* deserializarSolicitudEscrituraBloque(t_stream* buffer) {
	t_pedidoEscritura* pedido = malloc(sizeof(t_pedidoEscritura));

	int desplazamiento = 0;

	pedido->buffer = malloc(sizeof(t_stream));

	memcpy(&pedido->bloqueAEscribir, buffer->data + desplazamiento,
			sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(&pedido->buffer->size, buffer->data + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	pedido->buffer->data = malloc(pedido->buffer->size + 1);

	memcpy(pedido->buffer->data, buffer->data + desplazamiento,
			pedido->buffer->size);

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

	solicitud->rutaScriptTransformacion = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->rutaScriptTransformacion) + 1;

	memcpy(&solicitud->numBloque, &buffer->data + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(&solicitud->cantBytes, &buffer->data + desplazamiento, sizeof(int));
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
	desplazamiento += strlen(solicitud->archivoReduccionLocal) + 1;

	solicitud->rutaScript = strdup(buffer->data + desplazamiento);

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

	memcpy(&solicitud->workerEncargado, &buffer->data, sizeof(int));
	desplazamiento += sizeof(int);

	solicitud->ArchivoResultadoReduccionGlobal = strdup(
			buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->ArchivoResultadoReduccionGlobal) + 1;

	memcpy(&solicitud->cantWorkerInvolucradros, &buffer->data, sizeof(int));
	desplazamiento += sizeof(int);

	return solicitud;
}

t_pedidoAlmacenadoFinal * deserializarSolicitudAlmacenadoFinal(
		t_stream * buffer) {
	t_pedidoAlmacenadoFinal * solicitud = malloc(
			sizeof(t_pedidoAlmacenadoFinal));

	int desplazamiento = 0;

	solicitud->archivoReduccionGlobal = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->archivoReduccionGlobal) + 1;

	solicitud->rutaAlmacenadoFinal = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->rutaAlmacenadoFinal) + 1;

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

	memcpy(&indicacion->encargado, buffer->data + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

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

t_nodos_bloques * deserializarListaNodoBloques(t_stream * buffer) {
	t_nodos_bloques * nodosBloques = malloc(sizeof(t_nodos_bloques));
	int desplazamiento = 0;

	//Deserializo nodo-bloque
	int cantNodosBloques = 0;

	memcpy(&cantNodosBloques, buffer->data + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	nodosBloques->nodoBloque = list_create();

	int i;
	for (i = 0; i < cantNodosBloques; ++i) {
		t_nodo_bloque * nodoBloque = malloc(sizeof(t_nodo_bloque));

		nodoBloque->nomNodo = strdup(buffer->data + desplazamiento);
		desplazamiento += strlen(nodoBloque->nomNodo) + 1;

		memcpy(&nodoBloque->bloqueNodo, buffer->data + desplazamiento,
				sizeof(int));
		desplazamiento += sizeof(int);

		memcpy(&nodoBloque->bloqueArchivo, buffer->data + desplazamiento,
				sizeof(int));
		desplazamiento += sizeof(int);

		memcpy(&nodoBloque->originalidad, buffer->data + desplazamiento,
				sizeof(int));
		desplazamiento += sizeof(int);

		memcpy(&nodoBloque->tamanio, buffer->data + desplazamiento,
				sizeof(int));
		desplazamiento += sizeof(int);

		list_add(nodosBloques->nodoBloque, nodoBloque);
	}

	//Deserializo puertoIp
	int cantPuertoIP = 0;

	memcpy(&cantPuertoIP, buffer->data + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	nodosBloques->puertoIP = list_create();

	for (i = 0; i < cantPuertoIP; ++i) {
		t_puerto_ip * puertoIp = malloc(sizeof(t_puerto_ip));

		puertoIp->nomNodo = strdup(buffer->data + desplazamiento);
		desplazamiento += strlen(puertoIp->nomNodo) + 1;

		puertoIp->puerto = strdup(buffer->data + desplazamiento);
		desplazamiento += strlen(puertoIp->puerto) + 1;

		puertoIp->ip = strdup(buffer->data + desplazamiento);
		desplazamiento += strlen(puertoIp->ip) + 1;

		list_add(nodosBloques->puertoIP, puertoIp);
	}

	//Deserializo socket master solicitante
	memcpy(&nodosBloques->masterSolicitante, buffer->data + desplazamiento, sizeof(int));

	return nodosBloques;
}

t_nodo_nombre * deserializarNombreIPPuerto(t_stream * buffer) {
	t_nodo_nombre * nodoNombre = malloc(sizeof(t_nodo_nombre));

	int desplazamiento = 0;

	nodoNombre->nombre = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(nodoNombre->nombre) + 1;

	nodoNombre->ip = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(nodoNombre->ip) + 1;

	nodoNombre->puerto = strdup(buffer->data + desplazamiento);

	return nodoNombre;
}

t_solicitudArchivo * deserializarRutaArchivo(t_stream * buffer) {
	t_solicitudArchivo * solicitud = malloc(sizeof(t_solicitudArchivo));

	int desplazamiento = 0;

	solicitud->rutaArchivo = strdup(buffer->data + desplazamiento);
	desplazamiento += strlen(solicitud->rutaArchivo) + 1;

	memcpy(&solicitud->masterSolicitante, buffer->data + desplazamiento,
			sizeof(int));

	return solicitud;
}

//t_resultado_transformacion* deserializarResultadoTransformacion(t_stream * buffer){
//	//TODO HACER SEREALIZACION DE RECEPCION DE RESULTADO DE TRANSFORMACION
//}

/*-------------------------Funciones auxiliares-------------------------*/
void * abrirArchivo(char * rutaArchivo, size_t * tamArc, FILE ** archivo) {
	//Abro el archivo
	*archivo = fopen(rutaArchivo, "r");

	if (*archivo == NULL) {
		printf("%s: No existe el archivo o el directorio", rutaArchivo);
		return NULL;
	}

	//Copio informacion del archivo
	struct stat statArch;

	stat(rutaArchivo, &statArch);

	//Tamaño del archivo que voy a leer
	*tamArc = statArch.st_size;

	//Leo el total del archivo y lo asigno al buffer
	int fd = fileno(*archivo);
	void * dataArchivo = mmap(0, *tamArc, PROT_READ, MAP_SHARED, fd, 0);

	return dataArchivo;
}
