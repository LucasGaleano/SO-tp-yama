#include "fileSystem.h"

int main(int argc, char **argv) {
	formateado = false;

	//Creo la tabla de sockets
	crearTablaSockets();

	//Verifico si ignoro o no el estado anterior
	manejoDeEstado(argv[1]);

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
	case ENVIAR_NOMBRE:
		procesarNombre(unPaquete, client_socket);
		break;
	case ENVIAR_INFO_DATANODE:
		procesarInfoNodo(unPaquete, *client_socket);
		break;
	case ENVIAR_ERROR:
		procesarError(unPaquete, client_socket);
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
		if (formateado) {
			*client_socket = -1;
		} else {
			enviarSolicitudNombre(*client_socket);
		}
		break;
	case YAMA:
		if (!formateado)
			*client_socket = -1;
		break;
	case WORKER:
		if (!formateado)
			*client_socket = -1;
		break;
	default:
		*client_socket = -1;
		break;
	}
}

void procesarInfoNodo(t_paquete * unPaquete, int client_socket) {
	//Recibo info
	t_nodo_info * info = recibirInfoDataNode(unPaquete);

	//Agrego elemento a la tabla de nodos
	agregarNodoTablaNodos(info);

	//Creo una tabla de Bitmap del nodo
	crearArchivoTablaBitmap(info);

}

void procesarError(t_paquete * unPaquete, int * client_socket) {
	char * nomNodo = eliminarNodoTablaSockets(*client_socket);
	free(nomNodo);
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

void procesarBloqueGenerarCopia(t_paquete * unPaquete) {
	t_respuestaLecturaGenerarCopia * bloqueGenerarCopia =
			recibirBloqueGenerarCopia(unPaquete);

	//Busco el nombre del directorio
	char ** separado = string_split(bloqueGenerarCopia->rutaArchivo, "/");

	int posicion;

	for (posicion = 0; separado[posicion] != NULL; ++posicion) {
	}

	posicion -= 1;

	//Busco index del padre
	int indexPadre;

	if (posicion == 0) {
		indexPadre = obtenerIndex("root");
	} else {
		indexPadre = obtenerIndex(separado[posicion - 1]);
	}

	//Creo el config del archivo
	char * rutaFS = string_new();
	string_append(&rutaFS, RUTA_METADATA);
	string_append(&rutaFS, "metadata/archivos/");
	char * indexPadreChar = string_itoa(indexPadre);
	string_append(&rutaFS, indexPadreChar);
	string_append(&rutaFS, "/");
	string_append(&rutaFS, separado[posicion]);

	t_config * configArchivo = config_create(rutaFS);

	//Busco el numero de copia ultima
	char * key = string_new();
	string_append(&key, "BLOQUE");
	char * bloqueChar = string_itoa(bloqueGenerarCopia->numBloqueArchivo);
	string_append(&key, bloqueChar);
	string_append(&key, "COPIA");
	int i = 0;
	char * copiaChar = string_itoa(i);
	string_append(&key, copiaChar);

	while (config_has_property(configArchivo, key)) {
		free(key);
		free(copiaChar);
		key = string_new();
		string_append(&key, "BLOQUE");
		string_append(&key, bloqueChar);
		string_append(&key, "COPIA");
		i++;
		copiaChar = string_itoa(i);
		string_append(&key, copiaChar);
	}

	int bloqueAEscribir = buscarBloqueAEscribir(
			bloqueGenerarCopia->nomNodoAEscribir);

	agregarRegistroTablaArchivos(bloqueGenerarCopia->nomNodoAEscribir,
			bloqueAEscribir, bloqueGenerarCopia->numBloqueArchivo, i,
			configArchivo);

	quitarEspacioNodo(bloqueGenerarCopia->nomNodoAEscribir);

	int socketNodo = buscarSocketPorNombre(
			bloqueGenerarCopia->nomNodoAEscribir);

	enviarSolicitudEscrituraBloque(socketNodo, bloqueAEscribir, TAM_BLOQUE,
			bloqueGenerarCopia->data);

	//Libero memoria
	destruirSubstring(separado);
	free(rutaFS);
	free(indexPadreChar);
	config_destroy(configArchivo);
	free(key);
	free(bloqueChar);
	free(copiaChar);
	free(bloqueGenerarCopia->data);
	free(bloqueGenerarCopia->nomNodoAEscribir);
	free(bloqueGenerarCopia->rutaArchivo);
	free(bloqueGenerarCopia);
}

void procesarNombre(t_paquete * unPaquete, int * client_socket) {
	char * nomNodo = recibirNombre(unPaquete);

	//Agrego elemento a la lista de nodos por sockets
	agregarNodoTablaSockets(nomNodo, *client_socket);

	if (estadoAnterior) {
		bool soyNodoBuscado(t_nodo_info * nodo){
			return string_equals_ignore_case(nodo->nombre,nomNodo);
		}

		t_nodo_info * nodo = list_find(tablaNodos->infoDeNodo,(void*)soyNodoBuscado);

		nodo->disponible = true;

		tablaNodos->tamanio += nodo->total;
		tablaNodos->libres += nodo->libre;

		persistirTablaNodos();
	}
	free(nomNodo);
}

/*-------------------------Manejos de estado-------------------------*/
void manejoDeEstado(char * comando) {
	if (comando != NULL) {
		if (string_equals_ignore_case(comando, "--clean")) {
			ignoroEstadoAnterior();
		} else {
			printf("Parametro inexistente, concidero estado anterior \n");
		}
	} else {
		consideroEstadoAnterior();
	}
}

void ignoroEstadoAnterior() {
	printf("Ignoro estado anterior \n");

	estadoAnterior = false;

	//Verifico que la carpeta metadata exista
	char * ruta = string_new();
	string_append(&ruta, RUTA_METADATA);
	string_append(&ruta, "metadata");

	if (mkdir(ruta, 0777) == -1) {
		//Borro las estructuras administrativas existentes
		char * comando = string_new();
		string_append(&comando, "sudo rm -r ");
		string_append(&comando, RUTA_METADATA);
		string_append(&comando, "metadata");

		system(comando);

		free(comando);

		mkdir(ruta, 0777);
	}

	//Libero memoria
	free(ruta);
}

void consideroEstadoAnterior() {
	printf("Considero estado anterior \n");

	estadoAnterior = true;

	//Verifico que la carpeta metadata exista
	char * ruta = string_new();
	string_append(&ruta, RUTA_METADATA);
	string_append(&ruta, "metadata");

	if (mkdir(ruta, 0777) == -1) {
		crearTablaNodosSegunArchivo(RUTA_METADATA);
		crearTablaDirectorioSegunArchivo(RUTA_METADATA);
	} else {
		//Creo las nuevas tablas administrativas
		crearTablaNodos(ruta);
		crearTablaDirectorios(ruta);
	}

	free(ruta);
}
/*-------------------------Funciones auxiliares-------------------------*/
void iniciarServidor(char* unPuerto) {
	iniciarServer(unPuerto, (void *) procesarPaquete);
}
