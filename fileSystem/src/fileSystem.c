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
		recibirHandshake(unPaquete, client_socket);
		break;
	case ENVIAR_MENSAJE:
		recibirMensaje(unPaquete);
		break;
	case ENVIAR_ARCHIVO:
		recibirArchivo(unPaquete);
		break;
	case ENVIAR_INFO_DATANODE:
		recibirInfoNodo(unPaquete, *client_socket);
		break;
	case ENVIAR_ERROR:
		recibirError(unPaquete);
		break;
	case ENVIAR_BLOQUE_ARCHIVO_TEMPORAL:
		;
		t_respuestaLecturaArchTemp * bloque = recibirBloqueArchTemp(unPaquete);
		list_add(listaTemporal,bloque);
		break;
	case ENVIAR_RESPUESTA_ESCRITURA_BLOQUE:
		;
		t_respuestaEscritura * respuesta = recibirRespuestaEscrituraBloque(unPaquete);

		char * nomNodo = buscarNombrePorSocket(*client_socket);

		if(respuesta->exito){
			printf("Se pudo guardar el bloque: %d en el nodo: %s \n",respuesta->numBloque,nomNodo);
		}else{
			printf("No se pudo guardar el bloque: %d en el nodo: %s \n",respuesta->numBloque,nomNodo);
		}

		free(respuesta);
		break;
	default:
		break;
	}
	destruirPaquete(unPaquete);
}

void recibirHandshake(t_paquete * unPaquete, int * client_socket) {
	int tipoCliente;
	memcpy(&tipoCliente, unPaquete->buffer->data, sizeof(int));

	switch (tipoCliente) {
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

void recibirInfoNodo(t_paquete * unPaquete, int client_socket) {
	//Deserializo
	t_nodo_info * info = deserializarInfoDataNode(unPaquete->buffer);

	//Agrego elemento a la lista de nodos por sockets
	agregarNodoTablaSockets(info->nombre, client_socket);

	//Agrego elemento a la tabla de nodos
	agregarNodoTablaNodos(info);

	//Creo una tabla de Bitmap del nodo
	crearArchivoTablaBitmap(info);

}

void recibirError(t_paquete * unPaquete) {
	int cliente_desconectado;
	memcpy(&cliente_desconectado, unPaquete->buffer->data, sizeof(int));

	char * nomNodo = eliminarNodoTablaSockets(cliente_desconectado);

	eliminarNodoTablaNodos(nomNodo);

}

/*-------------------------Funciones auxiliares-------------------------*/
void iniciarServidor(char* unPuerto) {
	iniciarServer(unPuerto, (void *) procesarPaquete);
}
