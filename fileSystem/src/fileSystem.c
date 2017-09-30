#include "fileSystem.h"

int main(int argc, char **argv) {

	crearTablaDeNodos();

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
	//iniciarConsola();

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
		mostrarTablas();
		break;
	case ENVIAR_ERROR:
		recibirError(unPaquete);
		mostrarTablas();
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
	t_nodoYsocket * nodoXsocket = malloc(sizeof(t_nodoYsocket));
	nodoXsocket->nomNodo = malloc(string_length(info->nombre) + 1);

	memcpy(nodoXsocket->nomNodo, info->nombre, string_length(info->nombre) + 1);
	memcpy(&nodoXsocket->socket, &client_socket, sizeof(int));

	list_add(listaNodoYSocket, nodoXsocket);

	//Agrego elemento a la tabla de nodos
	int total = tablaNodos->tamanio + info->total;
	memcpy(&tablaNodos->tamanio, &total, sizeof(int));

	int libre = tablaNodos->libres + info->libre;
	memcpy(&tablaNodos->libres, &libre, sizeof(int));

	list_add(tablaNodos->infoDeNodo, info);

	//Agrego a la tabla de nodos los nombres
	char * nombre = malloc(string_length(info->nombre)+1);
	memcpy(nombre, info->nombre, string_length(info->nombre)+1);
	list_add(tablaNodos->nomNodos, nombre);
}

void recibirError(t_paquete * unPaquete) {
	int cliente_desconectado;
	memcpy(&cliente_desconectado, unPaquete->buffer->data, sizeof(int));

	char * nomNodo = buscarNodoXSocketYEliminarlo(cliente_desconectado);

	eliminarNodoDeTablaDeNodos(nomNodo);

}

/*-------------------------Funciones auxiliares-------------------------*/
void iniciarServidor(char* unPuerto) {
	iniciarServer(unPuerto, (void *) procesarPaquete);
}

void mostrarTablas() {
	printf("Se conecto nuevo dataNode \n\n");

	printf("Verifico la lista de nodos y sockets \n");
	int i;
	for (i = 0; i < list_size(listaNodoYSocket); ++i) {
		t_nodoYsocket* nodoYsocket = list_get(listaNodoYSocket, i);
		printf("Nombre del nodo: %s\n", nodoYsocket->nomNodo);
		printf("Socket del nodo: %d\n", nodoYsocket->socket);
	}
	printf("\n");

	printf("Verifico la tabla de nodos \n");
	printf("El tamanio es: %d \n", tablaNodos->tamanio);
	printf("Los libres son: %d \n", tablaNodos->libres);
	printf("Los nombres de los nodos son: ");
	for (i = 0; i < list_size(tablaNodos->nomNodos); ++i) {
		char* nombre = list_get(tablaNodos->nomNodos, i);
		printf("%s, ", nombre);
	}
	printf("\n");

	printf("Muestro info de cada nodo conectado \n");
	for (i = 0; i < list_size(tablaNodos->infoDeNodo); ++i) {
		t_nodo_info* info = list_get(tablaNodos->infoDeNodo, i);
		printf("Nombre del nodo: %s\n", info->nombre);
		printf("Total del nodo: %d\n", info->total);
		printf("Libre del nodo: %d\n", info->libre);
	}

}

char * buscarNodoXSocketYEliminarlo(int cliente_desconectado){

	bool esSocketBuscado(t_nodoYsocket * nodo) {
		return (nodo->socket == cliente_desconectado);
	}

	t_nodoYsocket * nodoYsockets = list_remove_by_condition(listaNodoYSocket,
			(void*) esSocketBuscado);

	char * nom = malloc(string_length(nodoYsockets->nomNodo)+1);
	memcpy(nom, nodoYsockets->nomNodo, string_length(nodoYsockets->nomNodo)+1);

	free(nodoYsockets->nomNodo);
	free(nodoYsockets);

	return nom;
}

void eliminarNodoDeTablaDeNodos(char * nomNodo){
	//Elimino de la tabla de nodos de la lista de nombres
	bool esSocketBuscado(char * socket) {
		return string_equals_ignore_case(socket,nomNodo);
	}

	char * nombre = list_remove_by_condition(tablaNodos->nomNodos,
			(void*) esSocketBuscado);

	//Elimino de la tabla de nodos de la lista de info
	bool esSocketBuscadoInfo(t_nodo_info * socket) {
		return string_equals_ignore_case(socket->nombre, nomNodo);
	}

	t_nodo_info * info = list_remove_by_condition(tablaNodos->infoDeNodo,
			(void*) esSocketBuscadoInfo);

	//Recalculo el tamanio total y tamanio libre
	int nuevoTamanio = tablaNodos->tamanio - info->total;
	int nuevoLibres = tablaNodos->libres - info->libre;

	memcpy(&tablaNodos->tamanio, &nuevoTamanio, sizeof(int));
	memcpy(&tablaNodos->libres, &nuevoLibres, sizeof(int));

	//Libero memoria
	free(nombre);
	free(info->nombre);
	free(info);
	free(nomNodo);
}

void crearTablaDeNodos(){
	listaNodoYSocket = list_create();

		tablaNodos = malloc(sizeof(t_tabla_nodo));

		tablaNodos->tamanio = 0;
		tablaNodos->libres = 0;

		tablaNodos->infoDeNodo = list_create();
		tablaNodos->nomNodos = list_create();
}
