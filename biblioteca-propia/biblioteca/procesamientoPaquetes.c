#include "procesamientoPaquetes.h"

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

int recibirTamPaquete(int client_socket, fd_set * set_master) {
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

		//Elimino el socket del conjunto maestro
		FD_CLR(client_socket, set_master);

	} else {

		//Copio el buffer en una variable asi despues lo libero
		memcpy(&tamBuffer, buffer, sizeof(int));
	}

	//Libero memoria
	free(buffer);

	return tamBuffer;
}

t_paquete * recibirPaquete(int client_socket, fd_set * set_master,
		int tamPaquete) {
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

		//Elimino el socket del conjunto maestro
		FD_CLR(client_socket, set_master);
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

/*-------------------------Procesamiento-------------------------*/

void procesarPaquete(t_paquete * unPaquete, int socket) {
	switch (unPaquete->codigoOperacion) {
	case ENVIAR_MENSAJE:
		recibirMensaje(unPaquete);
		break;

	case ENVIAR_ARCHIVO:
		recibirArchivo(unPaquete);
		break;

	default:
		break;
	}
	destruirPaquete(unPaquete);
}

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
