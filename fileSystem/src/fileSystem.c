#include "fileSystem.h"

int main(int argc, char **argv) {

	//Creo tablas administrativas
	crearTablaNodos("/home/utnso/Escritorio/metadata");
	crearTablaSockets();
	crearTablaDirectorios("/home/utnso/Escritorio/metadata");

	//Creo archivo de log
	logFileSystem = log_create("log_FileSystem.log", "fileSystem", false,
			LOG_LEVEL_TRACE);
	log_info(logFileSystem, "Inicio el proceso fileSystem \n");

	//Creo el thread para escuchar conexiones
	pthread_t threadServer;

	if (pthread_create(&threadServer, NULL, (void*) iniciarServidor,
	PUERTO_FILESYSTEM)) {
		perror("Error el crear el thread servidor.");
		exit(EXIT_FAILURE);
	}

	//Levanto consola
	iniciarConsola();

	pthread_join(threadServer, NULL);

	//Termina fileSystem
	log_info(logFileSystem, "Termino el proceso fileSystem");

	//Destruo archivo de log
	log_destroy(logFileSystem);

	return EXIT_SUCCESS;
}

/*-------------------------Procesamiento paquetes-------------------------*/
void procesarPaquete(t_paquete * unPaquete, int * client_socket) {
	switch (unPaquete->codigoOperacion) {
	case HANDSHAKE:
		procesarHandshake(unPaquete, client_socket);
		break;
	case ENVIAR_INFO_DATANODE:
		procesarInfoNodo(unPaquete, *client_socket);
		break;
	case ENVIAR_ERROR:
		procesarError(unPaquete);
		break;
	case ENVIAR_BLOQUE_ARCHIVO_TEMPORAL:
		procesarBloqueArchivoTemporal(unPaquete);
		break;
	case ENVIAR_RESPUESTA_ESCRITURA_BLOQUE:
		procesarRespuestaEscrituraBloque(unPaquete, *client_socket);
		break;
	case ENVIAR_BLOQUE_GENERAR_COPIA:
		procesarBloqueGenerarCopia(unPaquete);
		break;
	default:
		break;
	}
	destruirPaquete(unPaquete);
}

void procesarHandshake(t_paquete * unPaquete, int * client_socket) {
	switch (recibirHandshake(unPaquete)) {
	case DATANODE:
		break;
	case YAMA:
		break;
	case WORKER:
		break;
	default:
		*client_socket = -1;
		break;
	}
}

void procesarInfoNodo(t_paquete * unPaquete, int client_socket) {
	//Recibo info
	t_nodo_info * info = recibirInfoDataNode(unPaquete);

	//Agrego elemento a la lista de nodos por sockets
	agregarNodoTablaSockets(info->nombre, client_socket);

	//Agrego elemento a la tabla de nodos
	agregarNodoTablaNodos(info);

	//Creo una tabla de Bitmap del nodo
	crearArchivoTablaBitmap(info);

}

void procesarError(t_paquete * unPaquete) {
	int cliente_desconectado;
	memcpy(&cliente_desconectado, unPaquete->buffer->data, sizeof(int));

	char * nomNodo = eliminarNodoTablaSockets(cliente_desconectado);

	eliminarNodoTablaNodos(nomNodo);

}

void procesarBloqueArchivoTemporal(t_paquete * unPaquete) {
	t_respuestaLecturaArchTemp * bloqueArchTem = recibirBloqueArchTemp(
			unPaquete);
	list_add(listaTemporal, bloqueArchTem);
}

void procesarRespuestaEscrituraBloque(t_paquete * unPaquete, int client_socket) {
	t_respuestaEscritura * respuesta = recibirRespuestaEscrituraBloque(
			unPaquete);
	char * nomNodo = buscarNombrePorSocket(client_socket);

	if (respuesta->exito) {
		printf("Se pudo guardar el bloque: %d en el nodo: %s \n",
				respuesta->numBloque, nomNodo);
	} else {
		printf("No se pudo guardar el bloque: %d en el nodo: %s \n",
				respuesta->numBloque, nomNodo);
	}

	free(respuesta);

}

void procesarBloqueGenerarCopia(t_paquete * unPaquete){
	void * bloque = recibirBloque(unPaquete);
}
/*-------------------------Funciones auxiliares-------------------------*/
void iniciarServidor(char* unPuerto) {
	iniciarServer(unPuerto, (void *) procesarPaquete);
}
