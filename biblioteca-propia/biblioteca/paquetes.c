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

void enviarSolicitudLecturaBloque(int server_socket, int numBloque) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_SOLICITUD_LECTURA_BLOQUE;

	serializarSolicitudBloque(unPaquete, numBloque);

	enviarPaquetes(server_socket, unPaquete);

}

void enviarSolicitudEscrituraBloque(int server_socket, void* bloque, int numBloque) {

	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	unPaquete->codigoOperacion = ENVIAR_SOLICITUD_ESCRITURA_BLOQUE;

	serializarSolicitudBloque(unPaquete,bloque ,numBloque);

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

/*-------------------------Recibir-------------------------*/

void recibirMensaje(t_paquete * unPaquete) {
	char * mensaje = deserializarMensaje(unPaquete->buffer);

	printf("Me llego este mensaje: %s \n", mensaje);

	//Libero memoria
	free(mensaje);
}

void recibirArchivo(t_paquete * unPaquete) {
	void * archivo = deserializarArchivo(unPaquete->buffer);

	printf("Me llego un archivo \n");

	FILE* file = fopen("/home/utnso/Escritorio/pruebaFin.txt", "w+b");

	fwrite(archivo, unPaquete->buffer->size, 1, file);

	fclose(file);

	//Libero memoria
	free(archivo);
}

void* recibirbloque(t_paquete* unPaquete) {

	void* bloque = deserializarBloque(unPaquete->buffer);

	return bloque;

}

int recibirSolicitudLecturaBloque(t_paquete* unPaquete) {

	int numBloque = deserializarSolicitudBloque(unPaquete->buffer);
	return numBloque;
}


t_pedidoEscritura* recibirSolicitudEscrituraBloque(t_paquete* unPaquete) {

	return deserializarSolicitudEscrituraBloque(unPaquete->buffer);

}
