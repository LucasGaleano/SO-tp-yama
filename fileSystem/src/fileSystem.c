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

/*-------------------------Almacenar archivo-------------------------*/
void almacenarArchivo(char * rutaArchivo, char * rutaDestino, char * nomArchivo,
		int tipoArchivo) {
	size_t tamArch;

	FILE * archivofd;

	void * archivo = abrirArchivo(rutaArchivo, &tamArch, &archivofd);

	int desplazamiento = 0;

	int numeroBloque = 0;

	t_config * configTablaArchivo = crearArchivoTablaArchivo(rutaArchivo,
			rutaDestino);

	while (desplazamiento < tamArch) {
		char * nodoElegido = buscarNodoMenosCargado();

		int socketNodoElegido = buscarSocketPorNombre(nodoElegido);
		int bloqueAEscribir = buscarBloqueAEscribir(nodoElegido);

		void * buffer;
		switch (tipoArchivo) {
		case BINARIO:
			buffer = dividirBloqueArchivoBinario(archivo, &desplazamiento);
			break;
		case TEXTO:
			buffer = dividirBloqueArchivoTexto(archivo, &desplazamiento);
			break;
		default:
			printf("No puedo enviar el archivo xq no conosco su tipo de dato");
			return;
			break;
		}
		enviarSolicitudEscrituraBloque(socketNodoElegido, buffer,
				bloqueAEscribir);

		char * keyOriginal = string_new();
		char * valorOriginal = string_new();

		armarRegistroTablaArchivos(&keyOriginal, &valorOriginal, nodoElegido,
				numeroBloque, 0, bloqueAEscribir);

		config_set_value(configTablaArchivo, keyOriginal, valorOriginal);

		//Genero la copia
		char * nodoElegidoCopia = buscarNodoMenosCargado();

		int socketNodoElegidoCopia = buscarSocketPorNombre(nodoElegidoCopia);
		int bloqueAEscribirCopia = buscarBloqueAEscribir(nodoElegidoCopia);

		enviarSolicitudEscrituraBloque(socketNodoElegidoCopia, buffer,
				bloqueAEscribirCopia);

		char * keyCopia = string_new();
		char * valorCopia = string_new();

		armarRegistroTablaArchivos(&keyCopia, &valorCopia, nodoElegidoCopia,
				numeroBloque, 1, bloqueAEscribirCopia);

		config_set_value(configTablaArchivo, keyCopia, valorCopia);

		char * bytesPorBloques = string_new();
		string_append(&bytesPorBloques, "BLOQUE");
		char * numeroBloqueChar = string_itoa(numeroBloque);
		string_append(&bytesPorBloques, numeroBloqueChar);
		string_append(&bytesPorBloques, "BYTES");

		char * totalDeBytes = string_itoa(string_length((char *) buffer));

		config_set_value(configTablaArchivo, bytesPorBloques, totalDeBytes);

		free(buffer);
		numeroBloque++;
	}
}

void * dividirBloqueArchivoBinario(void * archivo, int * desplazamiento) {
	int tamProximoBloque;

	int tamArch = string_length((char *) archivo);

	if (tamArch - *desplazamiento < TAM_BLOQUE) {
		tamProximoBloque = tamArch - *desplazamiento;
	} else {
		tamProximoBloque = TAM_BLOQUE;
	}

	void * buffer = malloc(tamProximoBloque);
	memcpy(buffer, archivo + *desplazamiento, tamProximoBloque);
	*desplazamiento += tamProximoBloque;
	return buffer;
}

void * dividirBloqueArchivoTexto(void * archivo, int * desplazamiento) {
	char ** archivoSeparado = string_split((char *) archivo + *desplazamiento,
			"\n");

	int i = 0;
	int tamProximoBloque = string_length(archivoSeparado[i]);
	void * buffer;
	int tamBuffer = 0;

	while (TAM_BLOQUE > tamBuffer + tamProximoBloque
			&& archivoSeparado[i] != NULL) {
		buffer = realloc(buffer, tamProximoBloque);
		memcpy(buffer, archivoSeparado[i], tamProximoBloque);
		i++;
		tamProximoBloque = string_length(archivoSeparado[i]);
		tamBuffer = string_length((char *) buffer);
	}

	*desplazamiento *= tamBuffer;

	return buffer;
}

char * buscarNodoMenosCargado() {

	bool nodoMenosCargado(t_nodo_info * cargado, t_nodo_info * menosCargado) {
		int cargadoNum = cargado->total - cargado->libre;
		int menosCargadoNum = menosCargado->total - menosCargado->libre;
		return cargadoNum < menosCargadoNum;
	}

	list_sort(tablaNodos->infoDeNodo, (void*) nodoMenosCargado);

	t_nodo_info * nodo = list_get(tablaNodos->infoDeNodo, 0);

	return nodo->nombre;
}

int buscarBloqueAEscribir(char * nombreNodo) {

	bool esNodoBuscado(t_tabla_bitMaps * registroNodo) {
		return string_equals_ignore_case(registroNodo->nombre, nombreNodo);
	}

	t_tabla_bitMaps * registroNodo = list_find(tablaBitmapPorNodo,
			(void*) esNodoBuscado);

	return buscarBloqueLibre(registroNodo->configTablaBitmap);
}

/*-------------------------Funciones auxiliares-------------------------*/
void iniciarServidor(char* unPuerto) {
	iniciarServer(unPuerto, (void *) procesarPaquete);
}

void armarRegistroTablaArchivos(char ** key, char ** valor, char * nodoElegido,
		int numeroBloque, int numeroCopia, int bloqueAEscribir) {
	char * numeroBloqueChar = string_itoa(numeroBloque);
	char * numeroCopiaChar = string_itoa(numeroCopia);
	char * bloqueAEscribirChar = string_itoa(bloqueAEscribir);

	string_append(key, "BLOQUE");
	string_append(key, numeroBloqueChar);
	string_append(key, "COPIA");
	string_append(key, numeroCopiaChar);

	string_append(valor, "[");
	string_append(valor, nodoElegido);
	string_append(valor, ", ");
	string_append(valor, bloqueAEscribirChar);
	string_append(valor, "]");
}
