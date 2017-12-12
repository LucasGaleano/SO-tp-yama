#include "paquetes.h"

/*------------------------------Paquetes------------------------------*/

void enviarPaquetes(int socketfd, t_paquete * unPaquete) {

	int desplazamiento = 0;

	int tamPaquete = sizeof(int);
	int tamCodOp = sizeof(int);
	int tamSize = sizeof(int);
	int tamData = unPaquete->buffer->size;

	int tamTotal = tamCodOp + tamSize + tamData;

	void * buffer = malloc(tamPaquete + tamTotal);

	//Tamaño del paquete
	memcpy(buffer + desplazamiento, &tamTotal, tamPaquete);
	desplazamiento += tamPaquete;

	//Codigo de operacion
	memcpy(buffer + desplazamiento, &unPaquete->codigoOperacion, tamCodOp);
	desplazamiento += tamCodOp;

	//Buffer -- size
	memcpy(buffer + desplazamiento, &unPaquete->buffer->size, tamSize);
	desplazamiento += tamSize;

	//Buffer -- data
	memcpy(buffer + desplazamiento, unPaquete->buffer->data, tamData);

	//Envio el paquete y compruebo errores
	int resultado = send(socketfd, buffer, tamPaquete + tamTotal, MSG_NOSIGNAL);

	if (resultado == -1)
		perror("send");

	//Libero memoria
	destruirPaquete(unPaquete);
	free(buffer);
}

int recibirTamPaquete(int client_socket) {
	int tamBuffer;

	//Creo el buffer
	void * buffer = malloc(sizeof(int));

	//Recibo el buffer
	int recvd = recv(client_socket, buffer, sizeof(int), MSG_WAITALL);

	//Verifico error o conexión cerrada por el cliente
	if (recvd <= 0) {
		tamBuffer = recvd;
		if (recvd == -1) {
			perror("recv");
		}
		printf("El socket %d ha producido un error "
				"y ha sido desconectado.\n", client_socket);

		//Cierro el socket
		close(client_socket);

		//Libero memoria
		free(buffer);

		return -1;
	} else {

		//Copio el buffer en una variable asi despues lo libero
		memcpy(&tamBuffer, buffer, sizeof(int));
	}

	//Libero memoria
	free(buffer);

	return tamBuffer;
}

t_paquete * recibirPaquete(int client_socket, int tamPaquete) {
	//Reservo memoria para el buffer
	void * buffer = malloc(tamPaquete);
	//memset(buffer, '\0', tamPaquete); // Lo limpio al buffer para que no tenga basura

	//Recibo el buffer
	int recvd = recv(client_socket, buffer, tamPaquete, MSG_WAITALL);

	//Verifico error o conexión cerrada por el cliente
	if (recvd <= 0) {
		if (recvd == -1) {
			perror("recv");
		}
		printf("El socket %d ha producido un error "
				"y ha sido desconectado.\n", client_socket);

		//Cierro el socket
		close(client_socket);

	}

	//Armo el paquete a partir del buffer
	t_paquete * unPaquete = crearPaquete(buffer);

	return unPaquete;
}

t_paquete * crearPaquete(void * buffer) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	int desplazamiento = 0;

	int tamCodOp = sizeof(int);
	int tamSize = sizeof(size_t);

	//Codigo de operacion
	memcpy(&unPaquete->codigoOperacion, buffer + desplazamiento, tamCodOp);
	desplazamiento += tamCodOp;

	//Buffer -- size
	unPaquete->buffer = malloc(sizeof(t_stream));
	memcpy(&unPaquete->buffer->size, buffer + desplazamiento, tamSize);
	desplazamiento += tamSize;

	//Buffer -- data
	int tamData = unPaquete->buffer->size;
	unPaquete->buffer->data = malloc(tamData);
	memcpy(unPaquete->buffer->data, buffer + desplazamiento, tamData);

	//Libero memoria
	free(buffer);

	return unPaquete;
}

t_paquete * crearPaqueteError(int client_socket) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));
	unPaquete->codigoOperacion = ENVIAR_ERROR;
	unPaquete->buffer = malloc(sizeof(t_stream));
	unPaquete->buffer->size = sizeof(int);
	unPaquete->buffer->data = malloc(unPaquete->buffer->size);
	memcpy(unPaquete->buffer->data, &client_socket, unPaquete->buffer->size);
	return unPaquete;
}

void destruirPaquete(t_paquete * unPaquete) {
	free(unPaquete->buffer->data);
	free(unPaquete->buffer);
	free(unPaquete);
}

void mostrarPaquete(t_paquete * unPaquete) {
	printf("Muestro el paquete: \n");
	printf("Codigo de operacion: %d \n", unPaquete->codigoOperacion);
	printf("Tamanio del buffer: %d \n", unPaquete->buffer->size);
	printf("Buffer: %s \n", (char*) unPaquete->buffer->data);
}

/*-------------------------Enviar-------------------------*/

void enviarHandshake(int server_socket, int emisor) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = HANDSHAKE;

	serializarHandshake(unPaquete, emisor);

	enviarPaquetes(server_socket, unPaquete);

}

void enviarSolicitudNombre(int server_socket) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_SOLICITUD_NOMBRE;

	serializarNumero(unPaquete, 0);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarNombre(int server_socket, char * nombre, char* ip, char * puerto){
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_NOMBRE;

	serializarNombreIPPuerto(unPaquete, nombre, ip, puerto);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarRutaArchivo(int server_socket, char * rutaArchivo, int masterSolicitante) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_RUTA_ARCHIVO;

	serializarRutaArchivo(unPaquete, rutaArchivo, masterSolicitante);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarMensaje(int server_socket, char * mensaje) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_MENSAJE;

	serializarMensaje(unPaquete, mensaje);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarArchivo(int server_socket, char * rutaArchivo) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_ARCHIVO;

	serializarArchvivo(unPaquete, rutaArchivo);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarBloque(int server_socket, char* bloque) {

	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_BLOQUE;

	serializarBloque(unPaquete, bloque);

	enviarPaquetes(server_socket, unPaquete);

}

void enviarBloqueGenerarCopia(int server_socket, void * data,
		char * rutaArchivo, int numBloqueArchivo, char * nomNodoAEscribir) {

	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_BLOQUE_GENERAR_COPIA;

	serializarBloqueGenerarCopia(unPaquete, data, rutaArchivo, numBloqueArchivo,
			nomNodoAEscribir);

	enviarPaquetes(server_socket, unPaquete);

}

void enviarBloqueArchTemp(int server_socket, char* bloque, int orden) {

	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_BLOQUE_ARCHIVO_TEMPORAL;

	serializarBloqueArchTemp(unPaquete, bloque, orden);

	enviarPaquetes(server_socket, unPaquete);

}

void enviarSolicitudLecturaBloque(int server_socket, int numBloque) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_SOLICITUD_LECTURA_BLOQUE;

	serializarSolicitudLecturaBloque(unPaquete, numBloque);

	enviarPaquetes(server_socket, unPaquete);

}

void enviarSolicitudLecturaBloqueGenerarCopia(int server_socket,
		int numBloqueNodo, char * rutaArchivo, int numBloqueArchivo,
		char * nomNodoAEscribir) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_SOLICITUD_LECTURA_BLOQUE_GENERAR_COPIA;

	serializarSolicitudLecturaBloqueGenerarCopia(unPaquete, numBloqueNodo,
			rutaArchivo, numBloqueArchivo, nomNodoAEscribir);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarSolicitudLecturaArchTemp(int server_socket, int numBloque, int orden) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_SOLICITUD_LECTURA_ARCHIVO_TEMPORAL;

	serializarSolicitudLecturaBloqueArchTemp(unPaquete, numBloque, orden);

	enviarPaquetes(server_socket, unPaquete);

}

void enviarSolicitudEscrituraBloque(int server_socket, int bloqueAEscribir,
		int size, void* data) {

	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_SOLICITUD_ESCRITURA_BLOQUE;

	serializarSolicitudEscrituraBloque(unPaquete, bloqueAEscribir, size, data);

	enviarPaquetes(server_socket, unPaquete);

}

void enviarRespuestaEscrituraBloque(int server_socket, bool exito,
		int numBloque) {

	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_RESPUESTA_ESCRITURA_BLOQUE;

	serializarRespuestaEscrituraBloque(unPaquete, exito, numBloque);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarSolicitudInfoDataNode(int server_socket){
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_SOLICITUD_INFO_DATANODE;

	serializarNumero(unPaquete,0);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarInfoDataNode(int server_socket, char * nombreNodo,
		int bloquesTotales, int bloquesLibres) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_INFO_DATANODE;

	serializarInfoDataNode(unPaquete, nombreNodo, bloquesTotales,
			bloquesLibres);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarSolicitudTransformacion(int server_socket,
		t_pedidoTransformacion * solicitud) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_SOLICITUD_TRANSFORMACION;

	serializarSolicitudTransformacion(unPaquete, solicitud);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarSolicitudReduccionLocal(int server_socket,
		t_pedidoReduccionLocal * solicitud) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_SOLICITUD_REDUCCION_LOCAL;

	serializarSolicitudReduccionLocal(unPaquete, solicitud);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarSolicitudReduccionGlobal(int server_socket,
		t_pedidoReduccionGlobal * solicitud) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_SOLICITUD_REDUCCION_GLOBAL;

	serializarSolicitudReduccionGlobal(unPaquete, solicitud);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarSolicitudAlmacenadoFinal(int server_socket,
		t_pedidoAlmacenadoFinal * solicitud) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_SOLICITUD_ALMACENADO_FINAL;

	serializarSolicitudAlmacenadoFinal(unPaquete, solicitud);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarIndicacionTransformacion(int server_socket,
		t_indicacionTransformacion * indicacion) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_INDICACION_TRANSFORMACION;

	serializarIndicacionTransformacion(unPaquete, indicacion);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarIndicacionReduccionLocal(int server_socket,
		t_indicacionReduccionLocal * indicacion) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_INDICACION_REDUCCION_LOCAL;

	serializarIndicacionReduccionLocal(unPaquete, indicacion);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarIndicacionReduccionGlobal(int server_socket,
		t_indicacionReduccionGlobal * indicacion) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_INDICACION_REDUCCION_GLOBAL;

	serializarIndicacionReduccionGlobal(unPaquete, indicacion);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarIndicacionAlmacenadoFinal(int server_socket,
		t_indicacionAlmacenadoFinal * indicacion) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_INDICACION_ALMACENADO_FINAL;

	serializarIndicacionAlmacenadoFinal(unPaquete, indicacion);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarRutaParaArrancarTransformacion(int server_socket, char * ruta) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_RUTA_PARA_ARRANCAR_TRANSFORMACION;

	serializarMensaje(unPaquete, ruta);
}

void enviarError(int server_socket, int cod_error) {
	t_paquete* unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_ERROR;

	serializarNumero(unPaquete, cod_error);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarTareaCompletada(int server_socket, int cod_tarea) {
	t_paquete* unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = CONTINUA_MENSAJES;

	serializarNumero(unPaquete, cod_tarea);

	enviarPaquetes(server_socket, unPaquete);
}

void enviarListaNodoBloques(int server_socket, t_nodos_bloques * listaNodoBloque) {
	t_paquete* unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_LISTA_NODO_BLOQUES;

	serializarListaNodoBloques(unPaquete, listaNodoBloque);

	enviarPaquetes(server_socket, unPaquete);
}

/*-------------------------Recibir-------------------------*/

int recibirHandshake(t_paquete * unPaquete) {
	return deserializarHandshake(unPaquete->buffer);
}

t_solicitudArchivo * recibirRutaArchivo(t_paquete * unPaquete) {
	return deserializarRutaArchivo(unPaquete->buffer);
}

t_nodo_nombre * recibirNombre(t_paquete * unPaquete){
	return deserializarNombreIPPuerto(unPaquete->buffer);
}

char * recibirMensaje(t_paquete * unPaquete) {
	return deserializarMensaje(unPaquete->buffer);
}

void* recibirArchivo(t_paquete * unPaquete) {
	return deserializarArchivo(unPaquete->buffer);
}

t_nodo_info* recibirInfoDataNode(t_paquete * unPaquete) {
	return deserializarInfoDataNode(unPaquete->buffer);
}

void * recibirBloque(t_paquete * unPaquete) {
	return deserializarBloque(unPaquete->buffer);
}

t_respuestaLecturaGenerarCopia* recibirBloqueGenerarCopia(t_paquete * unPaquete) {
	return deserializarBloqueGenerarCopia(unPaquete->buffer);
}

t_respuestaLecturaArchTemp * recibirBloqueArchTemp(t_paquete * unPaquete) {
	return deserializarBloqueArchTemp(unPaquete->buffer);
}

int recibirSolicitudLecturaBloque(t_paquete* unPaquete) {
	return deserializarSolicitudLecturaBloque(unPaquete->buffer);
}

t_lecturaGenerarCopia * recibirSolicitudLecturaBloqueGenerarCopia(
		t_paquete * unPaquete) {
	return deserializarSolicitudLecturaBloqueGenerarCopia(unPaquete->buffer);
}

t_lecturaArchTemp * recibirSolicitudLecturaBloqueArchTemp(t_paquete* unPaquete) {
	return deserializarSolicitudLecturaBloqueArchTemp(unPaquete->buffer);
}

t_pedidoEscritura* recibirSolicitudEscrituraBloque(t_paquete* unPaquete) {
	return deserializarSolicitudEscrituraBloque(unPaquete->buffer);
}

t_respuestaEscritura * recibirRespuestaEscrituraBloque(t_paquete* unPaquete) {
	return deserializarRespuestaEscrituraBloque(unPaquete->buffer);
}

t_pedidoTransformacion * recibirSolicitudTransformacion(t_paquete * unPaquete) {
	return deserializarSolicitudTransformacion(unPaquete->buffer);
}

t_pedidoReduccionLocal * recibirSolicitudReduccionLocal(t_paquete * unPaquete) {
	return deserializarSolicitudReduccionLocal(unPaquete->buffer);
}

t_pedidoReduccionGlobal * recibirSolicitudReduccionGlobal(t_paquete * unPaquete) {
	return deserializarSolicitudReduccionGlobal(unPaquete->buffer);
}

t_pedidoAlmacenadoFinal * recibirSolicitudAlmacenadoFinal(t_paquete * unPaquete) {
	return deserializarSolicitudAlmacenadoFinal(unPaquete->buffer);
}

t_indicacionTransformacion * recibirIndicacionTransformacion(
		t_paquete * unPaquete) {
	return deserializarIndicacionTransformacion(unPaquete->buffer);
}

t_indicacionReduccionLocal * recibirIndicacionReduccionLocal(
		t_paquete * unPaquete) {
	return deserializarIndicacionReduccionLocal(unPaquete->buffer);
}

t_indicacionReduccionGlobal * recibirIndicacionReduccionGlobal(
		t_paquete * unPaquete) {
	return deserializarIndicacionReduccionGlobal(unPaquete->buffer);
}

t_indicacionAlmacenadoFinal * recibirIndicacionAlmacenadoFinal(
		t_paquete * unPaquete) {
	return deserializarIndicacionAlmacenadoFinal(unPaquete->buffer);
}

char * recibirRutaParaArrancarTransformacion(t_paquete * unPaquete) {
	return deserializarMensaje(unPaquete->buffer);
}

char * recibirRegistro(t_paquete * unPaquete) {

	return deserializarMensaje(unPaquete->buffer);
}

int recibirError(t_paquete * unPaquete) {
	return deserializarNumero(unPaquete->buffer);
}

int recibirTareaCompletada(t_paquete * unPaquete) {
	return deserializarNumero(unPaquete->buffer);
}
t_nodos_bloques * recibirListaNodoBloques(t_paquete * unPaquete){
	 return deserializarListaNodoBloques(unPaquete->buffer);
 }
