#include "fileSystem.h"

int main(int argc, char **argv) {
	formateado = false;
	estadoEstable = false;

	//Creo archivo de log
	logFileSystem = log_create("log_FileSystem.log", "fileSystem", false,
			LOG_LEVEL_TRACE);
	log_trace(logFileSystem, "Inicio el proceso fileSystem \n");

	//Creo la tabla de sockets
	crearTablaSockets();

	//Verifico si ignoro o no el estado anterior
	manejoDeEstado(argv[1]);

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

	//Libero memoria
	destruirTablaSockets();
	destruirTablaNodos();
	destruirTablaDirectorios();
	destruirTablaTareas();

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
	case ENVIAR_RUTA_ARCHIVO:
		procesarEnviarRutaArchivo(unPaquete, *client_socket);
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
		if (!estadoEstable)
			*client_socket = -1;
		break;
	case WORKER:
		if (!estadoEstable)
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
	info->disponible = true;

	//Agrego elemento a la tabla de nodos
	agregarNodoTablaNodos(info);

	//Creo una tabla de Bitmap del nodo
	crearArchivoTablaBitmap(info);

}

void procesarError(t_paquete * unPaquete, int * client_socket) {
	//Elimino de la tabla de sockets
	char * nomNodo = eliminarNodoTablaSockets(*client_socket);

	if (nomNodo == NULL)
		return;

	//Marco como no disponible en tabla de sockets
	bool esNodo(t_nodo_info * infoNodo) {
		return string_equals_ignore_case(infoNodo->nombre, nomNodo);
	}

	t_nodo_info * nodo = list_find(tablaNodos->infoDeNodo, (void*) esNodo);
	nodo->disponible = false;

	//Libero memoria
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
		log_trace(logFileSystem,
				"Se pudo guardar el bloque: %d en el nodo: %s \n",
				respuesta->numBloque, nomNodo);
	} else {
		log_warning(logFileSystem,
				"No se pudo guardar el bloque: %d en el nodo: %s \n",
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

void procesarEnviarRutaArchivo(t_paquete * unPaquete, int client_socket) {
	t_solicitudArchivo * archivoPedido = recibirRutaArchivo(unPaquete);

	//Busco el nombre del directorio
	char ** separado = string_split(archivoPedido->rutaArchivo, "/");

	int posicion;

	for (posicion = 0; separado[posicion] != NULL; ++posicion) {
	}

	posicion -= 1;

	//Busco el index del padre
	int indexPadre;

	if (posicion == 0) {
		indexPadre = obtenerIndex("root");
	} else {
		indexPadre = obtenerIndex(separado[posicion - 1]);
	}

	//Busco la configuracion del archivo
	char * rutaFS = string_new();
	string_append(&rutaFS, RUTA_METADATA);
	string_append(&rutaFS, "metadata/archivos/");
	char * indexPadreChar = string_itoa(indexPadre);
	string_append(&rutaFS, indexPadreChar);
	string_append(&rutaFS, "/");
	string_append(&rutaFS, separado[posicion]);

	t_config * configArchivo = config_create(rutaFS);

	if (configArchivo == NULL) {

		//Libero memoria
		destruirSubstring(separado);
		free(rutaFS);
		free(indexPadreChar);
		free(archivoPedido);
		return;
	}

	//Busco los bloques
	int cantidadDeBloques = config_get_int_value(configArchivo,
			"CANTIDAD_BLOQUES");

	int numeroBloque = 0;
	int numeroCopia;
	int numeroTotal;

	char ** bloque;

	t_list * listaBloques = list_create();

	for (numeroTotal = 0; numeroTotal < cantidadDeBloques; ++numeroTotal) {

		bloque = buscarBloqueCopia(configArchivo, numeroBloque, 0);

		char * key = string_new();
		string_append(&key, "BLOQUE");
		char * bloqueChar = string_itoa(numeroBloque);
		string_append(&key, bloqueChar);
		string_append(&key, "BYTES");

		int tamanioBloque = config_get_int_value(configArchivo, key);

		while (bloque != NULL) {
			t_nodo_bloque * nodoBloque = malloc(sizeof(t_nodo_bloque));

			nodoBloque->nomNodo = strdup(bloque[0]);
			nodoBloque->bloqueNodo = atoi((char*) bloque[1]);
			nodoBloque->originalidad = numeroCopia;
			nodoBloque->bloqueArchivo = numeroBloque;
			nodoBloque->tamanio = tamanioBloque;

			list_add(listaBloques, bloque);
			numeroTotal++;
			numeroCopia++;
			destruirSubstring(bloque);
			bloque = buscarBloqueCopia(configArchivo, numeroBloque,
					numeroCopia);
		}
		numeroBloque++;
	}

	//Busco nodos disponibles
	bool estoyDisponible(t_nodo_info * nodo) {
		return nodo->disponible;
	}

	t_list * nodosDisponibles = list_filter(tablaNodos->infoDeNodo,
			(void*) estoyDisponible);

	//Armo la lista final de los nodos
	t_nodos_bloques * nodosBloques = malloc(sizeof(nodosBloques));
	nodosBloques->nodoBloque = list_create();

	int i;
	for (i = 0; i < nodosDisponibles->elements_count; i++) {
		t_nodo_info * nodoDisponible = list_get(nodosDisponibles, i);

		void agregarSiEstaDisponible(t_nodo_bloque * nodo) {
			if (string_equals_ignore_case(nodoDisponible->nombre,
					nodo->nomNodo))
				list_add(nodosBloques->nodoBloque, nodo);
		}

		list_iterate(listaBloques, (void*) agregarSiEstaDisponible);
	}

	//Armo con los disponibles los puerto y ip
	nodosBloques->puertoIP = list_create();

	void agregoAListaIpPuerto(t_nodo_info * nodoDisponble) {
		t_tabla_sockets_ip_puerto * ipPuerto = buscarIpPuertoPorNombre(
				nodoDisponble->nombre);
		t_puerto_ip * puertoIpFinal = malloc(sizeof(t_puerto_ip));
		puertoIpFinal->ip = strdup(ipPuerto->ip);
		puertoIpFinal->puerto = strdup(ipPuerto->puerto);
		puertoIpFinal->nomNodo = strdup(nodoDisponble->nombre);

		list_add(nodosBloques->puertoIP, puertoIpFinal);
	}

	list_iterate(nodosDisponibles, (void*) agregoAListaIpPuerto);

	enviarListaNodoBloques(client_socket, nodosBloques,
			archivoPedido->masterSolicitante);

	free(archivoPedido);
	destruirSubstring(separado);
	free(rutaFS);
	free(indexPadreChar);
	config_destroy(configArchivo);
}

void procesarNombre(t_paquete * unPaquete, int * client_socket) {
	t_nodo_nombre * nodo = recibirNombre(unPaquete);

	//Agrego elemento a la lista de nodos por sockets
	agregarNodoTablaSockets(nodo->nombre, *client_socket, nodo->ip,
			nodo->puerto);

	if (estadoAnterior) {
		bool soyNodoBuscado(t_nodo_info * nodoTabla) {
			return string_equals_ignore_case(nodoTabla->nombre, nodo->nombre);
		}

		t_nodo_info * nodo = list_find(tablaNodos->infoDeNodo,
				(void*) soyNodoBuscado);

		if (nodo == NULL) {
			eliminarNodoTablaSockets(*client_socket);
			*client_socket = -1;
			return;
		}

		nodo->disponible = true;

		tablaNodos->tamanio += nodo->total;
		tablaNodos->libres += nodo->libre;

		persistirTablaNodos();

		if (verificarEstadoEstable()) {
			estadoEstable = true;
			log_trace(logFileSystem,
					"El proceso FileSystem está en un estado estable \n");
		} else {
			log_trace(logFileSystem,
					"El proceso FileSystem está en un estado no-estable \n");
		}
	}

	free(nodo->ip);
	free(nodo->nombre);
	free(nodo->puerto);
	free(nodo);
}

/*-------------------------Manejos de estado-------------------------*/
void manejoDeEstado(char * comando) {
	if (comando != NULL) {
		if (string_equals_ignore_case(comando, "--clean")) {
			ignoroEstadoAnterior();
		} else {
			log_trace(logFileSystem,
					"Parametro inexistente, concidero estado anterior \n");
		}
	} else {
		consideroEstadoAnterior();
	}
}

void ignoroEstadoAnterior() {
	log_trace(logFileSystem, "Ignoro estado anterior \n");

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
	log_trace(logFileSystem, "Considero estado anterior \n");

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

/*-------------------------Estado estable/no estable-------------------------*/
bool verificarEstadoEstable() {
	t_list * listaArchivos = buscarTodosArchivos();

	bool estoyEnEstadoEstable(char * rutaArchivo) {
		return soyEstable(rutaArchivo);
	}

	bool todosEstanEstables = list_all_satisfy(listaArchivos,
			(void*) estoyEnEstadoEstable);

	list_destroy_and_destroy_elements(listaArchivos, free);

	return todosEstanEstables;
}

t_list * buscarTodosArchivos() {
	t_list * listaCarpetas = list_create();

	char * ruta = string_new();
	string_append(&ruta, RUTA_METADATA);
	string_append(&ruta, "metadata/archivos");

	listarCarpetasDeArchivos(ruta, listaCarpetas);

	t_list *listaArchivos = list_create();

	void listarArchivos(char * ruta) {
		listarArchivosDeMismaCarpeta(ruta, listaArchivos);
	}

	list_iterate(listaCarpetas, (void*) listarArchivos);

	list_destroy_and_destroy_elements(listaCarpetas, free);

	free(ruta);

	return listaArchivos;
}

/*-------------------------Funciones auxiliares-------------------------*/
void iniciarServidor(char* unPuerto) {
	iniciarServer(unPuerto, (void *) procesarPaquete);
}

void listarArchivosDeMismaCarpeta(char * ruta, t_list * listaArchivos) {
	DIR *dir;
	struct dirent *ent;

	dir = opendir(ruta);

	while ((ent = readdir(dir)) != NULL) {
		if (ent->d_name[0] != '.') {
			char * archivo = string_new();
			string_append(&archivo, ruta);
			string_append(&archivo, "/");
			string_append(&archivo, (char*) ent->d_name);
			list_add(listaArchivos, archivo);
		}
	}

	closedir(dir);
}

void listarCarpetasDeArchivos(char * ruta, t_list * lista) {
	DIR *dir;
	struct dirent *ent;

	dir = opendir(ruta);

	while ((ent = readdir(dir)) != NULL) {
		if (ent->d_name[0] != '.') {
			char * carpeta = string_new();
			string_append(&carpeta, ruta);
			string_append(&carpeta, "/");
			string_append(&carpeta, (char*) ent->d_name);
			list_add(lista, carpeta);
		}
	}

	closedir(dir);

}
