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
			rutaDestino, nomArchivo);

	while (desplazamiento < tamArch) {
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

		//Genero el bloque original
		char * nodoElegido = buscarNodoMenosCargado();
		int socketNodoElegido = buscarSocketPorNombre(nodoElegido);
		int bloqueAEscribir = buscarBloqueAEscribir(nodoElegido);

		agregarRegistroTablaArchivos(nodoElegido, bloqueAEscribir, numeroBloque,0,
				configTablaArchivo);

		enviarSolicitudEscrituraBloque(socketNodoElegido, buffer,
				bloqueAEscribir);

		//Genero la copia
		char * nodoElegidoCopia = buscarNodoMenosCargado();
		int socketNodoElegidoCopia = buscarSocketPorNombre(nodoElegidoCopia);
		int bloqueAEscribirCopia = buscarBloqueAEscribir(nodoElegidoCopia);

		agregarRegistroTablaArchivos(nodoElegidoCopia, bloqueAEscribirCopia, numeroBloque, 1,
				configTablaArchivo);

		enviarSolicitudEscrituraBloque(socketNodoElegidoCopia, buffer,
				bloqueAEscribirCopia);

		//Bytes guardados en un bloque
		int tamBuffer = strlen((char*)buffer);
		guardoBytesPorBloque(numeroBloque, tamBuffer, configTablaArchivo);

		//Actualizo el numero de bloques
		numeroBloque++;

		//Libero memoria
		free(buffer);
	}
}

void * dividirBloqueArchivoBinario(void * archivo, int * desplazamiento) {
	int tamProximoBloque;

	int tamArch = string_length((char *) archivo);

	if ((tamArch - (*desplazamiento)) < TAM_BLOQUE) {
		tamProximoBloque = tamArch - *desplazamiento;
	} else {
		tamProximoBloque = TAM_BLOQUE;
	}

	void * buffer = malloc(tamProximoBloque);
	memcpy(buffer, archivo + (*desplazamiento), tamProximoBloque);
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

	*desplazamiento += tamBuffer;

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

	//Actualizo tabla de nodos
	tablaNodos->libres--;
	nodo->libre--;
	persistirTablaNodos();

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


void agregarRegistroTablaArchivos(char * nodoElegido, int bloqueAEscribir, int bloqueDelArchivo, int numeroCopia, t_config * configTablaArchivo) {

	char * key = string_new();
	char * valor = string_new();

	char * numeroBloqueAEscribirChar = string_itoa(bloqueAEscribir);
	char * numeroBloqueDelArchivoChar = string_itoa(bloqueDelArchivo);
	char * numeroCopiaChar = string_itoa(numeroCopia);

	string_append(&key, "BLOQUE");
	string_append(&key, numeroBloqueDelArchivoChar);
	string_append(&key, "COPIA");
	string_append(&key, numeroCopiaChar);

	string_append(&valor, "[");
	string_append(&valor, nodoElegido);
	string_append(&valor, ", ");
	string_append(&valor, numeroBloqueAEscribirChar);
	string_append(&valor, "]");

	config_set_value(configTablaArchivo, key, valor);

	config_save(configTablaArchivo);

	free(key);
	free(valor);
	free(numeroBloqueAEscribirChar);
	free(numeroBloqueDelArchivoChar);
	free(numeroCopiaChar);
}


void guardoBytesPorBloque(int numeroBloque, int tamBuffer, t_config * configTablaArchivo){
	char * bytesPorBloques = string_new();
	char * numeroBloqueChar = string_itoa(numeroBloque);
	char * totalDeBytes = string_itoa(tamBuffer);

	string_append(&bytesPorBloques, "BLOQUE");
	string_append(&bytesPorBloques, numeroBloqueChar);
	string_append(&bytesPorBloques, "BYTES");

	config_set_value(configTablaArchivo, bytesPorBloques, totalDeBytes);

	config_save(configTablaArchivo);

	free(bytesPorBloques);
	free(numeroBloqueChar);
	free(totalDeBytes);
}
