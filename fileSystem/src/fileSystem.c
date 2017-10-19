#include "fileSystem.h"

int main(int argc, char **argv) {

	//Creo tablas administrativas
	crearTablaNodos("/home/utnso/Escritorio/metadata");
	crearTablaSockets();

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
		break;
	case ENVIAR_ERROR:
		recibirError(unPaquete);
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

/*-------------------------Tabla de nodos-------------------------*/
void crearTablaNodos(char * rutaTablaNodos) {
	tablaNodos = malloc(sizeof(t_tabla_nodo));

	tablaNodos->tamanio = 0;
	tablaNodos->libres = 0;

	tablaNodos->infoDeNodo = list_create();
	tablaNodos->nomNodos = list_create();

	crearArchivoTablaNodos(rutaTablaNodos);
}

void crearArchivoTablaNodos(char * ruta) {
	//Abro el archivo para usarlo y si no existe lo creo
	char * rutaArchivo = string_new();
	string_append(&rutaArchivo, ruta);
	string_append(&rutaArchivo, "/nodos.bin");

	FILE* file = fopen(rutaArchivo, "r");

	if (file == NULL) {
		mkdir("/home/utnso/Escritorio/metadata", 0777);
		file = fopen(rutaArchivo, "w+b");

		//Cierro el archivo
		fclose(file);

		//Creo la estructura de configuracion
		configTablaNodo = config_create(rutaArchivo);

		//Seteo las variables en cero
		config_set_value(configTablaNodo, "TAMANIO", "0");
		config_set_value(configTablaNodo, "LIBRE", "0");
		config_set_value(configTablaNodo, "NODOS", "[]");

		config_save(configTablaNodo);

	} else {

		//Creo la estructura de configuracion
		configTablaNodo = config_create(rutaArchivo);

		//Cierro el archivo
		fclose(file);

	}

	free(rutaArchivo);
}

void agregarNodoTablaNodos(t_nodo_info * info) {
	int total = tablaNodos->tamanio + info->total;
	memcpy(&tablaNodos->tamanio, &total, sizeof(int));

	int libre = tablaNodos->libres + info->libre;
	memcpy(&tablaNodos->libres, &libre, sizeof(int));

	list_add(tablaNodos->infoDeNodo, info);

	//Agrego a la tabla de nodos los nombres
	char * nombre = malloc(string_length(info->nombre) + 1);
	memcpy(nombre, info->nombre, string_length(info->nombre) + 1);
	list_add(tablaNodos->nomNodos, nombre);

	persistirTablaNodos();
}

void eliminarNodoTablaNodos(char * nomNodo) {
	//Elimino de la tabla de nodos de la lista de nombres
	bool esSocketBuscado(char * socket) {
		return string_equals_ignore_case(socket, nomNodo);
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

	persistirTablaNodos();

	//Libero memoria
	free(nombre);
	free(info->nombre);
	free(info);
	free(nomNodo);
}

void persistirTablaNodos() {
	//Persisto el tamanio de la tabla
	int tamanio = tablaNodos->tamanio;
	char * stringTamanio = string_itoa(tamanio);
	config_set_value(configTablaNodo, "TAMANIO", stringTamanio);
	free(stringTamanio);

	//Persisto los bloquees libres de la tabla
	int libres = tablaNodos->libres;
	char * stringLibres = string_itoa(libres);
	config_set_value(configTablaNodo, "LIBRE", stringLibres);
	free(stringLibres);

	//Persisto los nombres de los nodos
	char* nomNodos = string_new();

	string_append(&nomNodos, "[");

	int i;
	for (i = 0; i < tablaNodos->nomNodos->elements_count; i++) {

		char * nombre = list_get(tablaNodos->nomNodos, i);

		if (i == 0) {
			string_append(&nomNodos, nombre);
		} else {
			string_append(&nomNodos, ", ");
			string_append(&nomNodos, nombre);
		}

	}

	string_append(&nomNodos, "]");

	config_set_value(configTablaNodo, "NODOS", nomNodos);

	free(nomNodos);

	//Persisto la info de cada nodo
	for (i = 0; i < tablaNodos->infoDeNodo->elements_count; i++) {
		t_nodo_info * info = list_get(tablaNodos->infoDeNodo, i);

		char* total = string_new();
		char* libre = string_new();

		string_append(&total, info->nombre);
		string_append(&total, "Total");

		string_append(&libre, info->nombre);
		string_append(&libre, "Libre");

		char * totalNumero = string_itoa(info->total);
		char * totalLibre = string_itoa(info->libre);

		config_set_value(configTablaNodo, total, totalNumero);
		config_set_value(configTablaNodo, libre, totalLibre);

		free(total);
		free(libre);
		free(totalNumero);
		free(totalLibre);
	}

	config_save(configTablaNodo);
}

/*-------------------------Tabla de sockets-------------------------*/
void crearTablaSockets(void) {
	tablaSockets = list_create();
}

void agregarNodoTablaSockets(char * nombreNodo, int client_socket) {
	t_tabla_sockets * registroSocket = malloc(sizeof(t_tabla_nodo));
	registroSocket->nombre = malloc(string_length(nombreNodo) + 1);

	memcpy(registroSocket->nombre, nombreNodo, string_length(nombreNodo) + 1);
	memcpy(&registroSocket->socket, &client_socket, sizeof(int));

	list_add(tablaSockets, registroSocket);
}

char * eliminarNodoTablaSockets(int cliente_desconectado) {

	bool esSocketBuscado(t_tabla_sockets* nodo) {
		return (nodo->socket == cliente_desconectado);
	}

	t_tabla_sockets * registroSocket = list_remove_by_condition(tablaSockets,
			(void*) esSocketBuscado);

	char * nom = malloc(string_length(registroSocket->nombre) + 1);
	memcpy(nom, registroSocket->nombre,
			string_length(registroSocket->nombre) + 1);

	free(registroSocket->nombre);
	free(registroSocket);

	return nom;
}

/*-------------------------Tabla de Bitmap-------------------------*/
void crearArchivoTablaBitmap(t_nodo_info * info) {
	//Abro el archivo para usarlo
	char * rutaArchivo = string_new();
	string_append(&rutaArchivo, "/home/utnso/Escritorio/metadata/bitmaps/");
	string_append(&rutaArchivo, info->nombre);
	string_append(&rutaArchivo, ".dat");

	mkdir("/home/utnso/Escritorio/metadata/bitmaps", 0777);
	FILE* file = fopen(rutaArchivo, "w+b");

	//Cierro el archivo
	fclose(file);

	//Creo la estructura de configuracion
	configTablaBitmap = config_create(rutaArchivo);

	//Seteo valores de bitmap en 0
	int i;
	for (i = 0; i < info->total; ++i) {
		char * nombre = string_new();
		string_append(&nombre, "Bloque");
		char * numeroNodo = string_itoa(i);
		string_append(&nombre, numeroNodo);
		config_set_value(configTablaBitmap,nombre,"0");
		free(nombre);
		free(numeroNodo);
	}

	config_save(configTablaBitmap);

	free(rutaArchivo);
}

/*-------------------------Funciones auxiliares-------------------------*/
void iniciarServidor(char* unPuerto) {
	iniciarServer(unPuerto, (void *) procesarPaquete);
}

void mostrarTablas() {
	printf("Se conecto nuevo dataNode \n\n");

	printf("Verifico la lista de nodos y sockets \n");
	int i;
	for (i = 0; i < list_size(tablaSockets); ++i) {
		t_tabla_sockets * registroSockets = list_get(tablaSockets, i);
		printf("Nombre del nodo: %s\n", registroSockets->nombre);
		printf("Socket del nodo: %d\n", registroSockets->socket);
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

