#include "interfaz.h"

/*-------------------------Almacenar archivo-------------------------*/
void almacenarArchivo(char * rutaArchivo, char * rutaDestino, char * nomArchivo,
		int tipoArchivo) {
	size_t tamArch;

	FILE * archivofd;

	void * archivo = abrirArchivo(rutaArchivo, &tamArch, &archivofd);

	if (archivo == NULL) {
		log_warning(logFileSystem, "%s: No existe el archivo o el directorio",
				rutaArchivo);
		return;
	}

	int desplazamiento = 0;

	int numeroBloque = 0;

	t_config * configTablaArchivo = crearArchivoTablaArchivo(rutaArchivo,
			rutaDestino, nomArchivo, tipoArchivo);

	int tamBuffer;

	while (desplazamiento < tamArch) {
		void * buffer;
		switch (tipoArchivo) {
		case BINARIO:
			buffer = dividirBloqueArchivoBinario(archivo, tamArch,
					&desplazamiento, &tamBuffer);
			break;
		case TEXTO:
			;
			t_stream * bufferEncontrado = dividirBloqueArchivoTexto(archivo,
					&desplazamiento, tamArch);
			buffer = malloc(bufferEncontrado->size);
			memcpy(buffer, bufferEncontrado->data, bufferEncontrado->size);
			memcpy(&tamBuffer, &bufferEncontrado->size, sizeof(int));

			free(bufferEncontrado->data);
			free(bufferEncontrado);

			//Verifico que el buffer no sea superior a el tamanio del bloque
			if (tamBuffer > TAM_BLOQUE) {
				log_warning(logFileSystem,
						"El espacio del bloque no es suficiente para guardar el buffer \n");
				log_warning(logFileSystem, "Bloque %d corrupto \n",
						numeroBloque);
			}
			break;
		default:
			log_warning(logFileSystem,
					"No puedo enviar el archivo xq no conosco su tipo de dato");
			return;
			break;
		}

		//Genero el bloque original
		char * nodoElegido = buscarNodoMenosCargado();
		int socketNodoElegido = buscarSocketPorNombre(nodoElegido);
		int bloqueAEscribir = buscarBloqueAEscribir(nodoElegido);

		agregarRegistroTablaArchivos(nodoElegido, bloqueAEscribir, numeroBloque,
				0, configTablaArchivo);

		enviarSolicitudEscrituraBloque(socketNodoElegido, bloqueAEscribir,
				tamBuffer, buffer);

		//Genero la copia
		char * nodoElegidoCopia = buscarNodoMenosCargado();
		int socketNodoElegidoCopia = buscarSocketPorNombre(nodoElegidoCopia);
		int bloqueAEscribirCopia = buscarBloqueAEscribir(nodoElegidoCopia);

		agregarRegistroTablaArchivos(nodoElegidoCopia, bloqueAEscribirCopia,
				numeroBloque, 1, configTablaArchivo);

		enviarSolicitudEscrituraBloque(socketNodoElegidoCopia,
				bloqueAEscribirCopia, tamBuffer, buffer);

		//Bytes guardados en un bloque
		guardoBytesPorBloque(numeroBloque, tamBuffer, configTablaArchivo);

		//Lo agrego a la cantidad de bloques totales
		int totalesAnterior = config_get_int_value(configTablaArchivo,
				"CANTIDAD_BLOQUES");
		totalesAnterior++;
		char * totalesActualesChar = string_itoa(totalesAnterior);
		config_set_value(configTablaArchivo, "CANTIDAD_BLOQUES",
				totalesActualesChar);

		//Actualizo el numero de bloques
		numeroBloque++;

		//Libero memoria
		free(buffer);
		free(nodoElegido);
		free(nodoElegidoCopia);
		free(totalesActualesChar);
	}

	config_destroy(configTablaArchivo);
	munmap(archivo, tamArch);
	fclose(archivofd);
}

void * dividirBloqueArchivoBinario(void * archivo, size_t tamArch,
		int * desplazamiento, int * tamBuffer) {
	int tamProximoBloque;

	int tamRestante = tamArch - *desplazamiento;

	if (tamRestante < TAM_BLOQUE) {
		tamProximoBloque = tamRestante;
	} else {
		tamProximoBloque = TAM_BLOQUE;
	}

	void * buffer = malloc(tamProximoBloque);
	memcpy(buffer, archivo + (*desplazamiento), tamProximoBloque);
	*desplazamiento = *desplazamiento + tamProximoBloque;
	*tamBuffer = tamProximoBloque;
	return buffer;
}

t_stream * dividirBloqueArchivoTexto(void * archivo, int * desplazamiento,
		int tamArchivo) {

	bool noTermino = true;

	int tamRestante = tamArchivo - *desplazamiento;

	t_stream * bufferFinal = malloc(sizeof(t_stream));

	bufferFinal->size = 0;
	bufferFinal->data = malloc(1);

	//Genero el bloque
	t_stream * bloqueAGurdar = generarBloque(archivo + *desplazamiento,
			tamRestante);

	while (noTermino && tamRestante > 0
			&& (bufferFinal->size + bloqueAGurdar->size) < TAM_BLOQUE) {

		//Guardo el bloque al buffer
		bufferFinal->data = realloc(bufferFinal->data,
				bufferFinal->size + bloqueAGurdar->size);
		memcpy(bufferFinal->data + bufferFinal->size, bloqueAGurdar->data,
				bloqueAGurdar->size);
		bufferFinal->size += bloqueAGurdar->size;

		tamRestante -= bloqueAGurdar->size;

		*desplazamiento = *desplazamiento + bloqueAGurdar->size;

		free(bloqueAGurdar->data);
		free(bloqueAGurdar);

		//Genero el bloque
		bloqueAGurdar = generarBloque(archivo + *desplazamiento, tamRestante);

		if (bloqueAGurdar->size == 0) {
			noTermino = false;
		}
	}

	free(bloqueAGurdar->data);
	free(bloqueAGurdar);

	return bufferFinal;

}

char * buscarNodoMenosCargado() {

	bool estaDisponible(t_nodo_info * nodo) {
		return nodo->disponible;
	}

	t_list * disponibles = list_filter(tablaNodos->infoDeNodo,
			(void*) estaDisponible);

	bool nodoMenosCargado(t_nodo_info * cargado, t_nodo_info * menosCargado) {
		int cargadoNum = cargado->total - cargado->libre;
		int menosCargadoNum = menosCargado->total - menosCargado->libre;
		return cargadoNum < menosCargadoNum;
	}

	list_sort(disponibles, (void*) nodoMenosCargado);

	t_nodo_info * nodo = list_get(disponibles, 0);

	//Actualizo tabla de nodos
	tablaNodos->libres--;
	nodo->libre--;
	persistirTablaNodos();

	char * nomNodo = strdup(nodo->nombre);

	list_destroy(disponibles);

	return nomNodo;
}

int buscarBloqueAEscribir(char * nombreNodo) {
	char * rutaConfig = string_new();
	string_append(&rutaConfig, RUTA_METADATA);
	string_append(&rutaConfig, "metadata/bitmaps/");
	string_append(&rutaConfig, nombreNodo);
	string_append(&rutaConfig, ".dat");

	t_config * configBitMap = config_create(rutaConfig);

	free(rutaConfig);

	return buscarBloqueLibre(configBitMap);
}

t_stream * generarBloque(void * archivo, int tamRestante) {

	int desplazamiento = 0;
	int tamALeer = 0;

	if (string_starts_with(archivo, "\n"))
		desplazamiento++;

	void * prueba = strchr(archivo + desplazamiento, '\n');

	if (prueba == NULL) {
		tamALeer += tamRestante;
	} else {
		tamALeer += prueba - archivo + 1;
	}

	t_stream * buffer = malloc(sizeof(t_stream));

	buffer->data = malloc(tamALeer);
	buffer->size = tamALeer;

	memcpy(buffer->data, archivo + desplazamiento, tamALeer);

	return buffer;
}

/*-------------------------Leer archivo-------------------------*/
char * leerArchivo(char * rutaArchivo) {

	listaTemporal = list_create();

	//Busco el nombre del directorio
	char ** separado = string_split(rutaArchivo, "/");

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

		return NULL;
	}

	//Busco los bloques en los nodos
	int bloque = 0;

	if (!estadoEstable) {
		return NULL;
	}

	t_list * listaNodoBloque = buscarBloque(configArchivo, bloque);

	while (!list_is_empty(listaNodoBloque)) {
		t_nodoBloque * nodoBloque = nodoMenosSaturado(listaNodoBloque);

		t_nodoBloque * tarea = malloc(sizeof(t_nodoBloque));
		tarea->nomNodo = strdup(nodoBloque->nomNodo);
		tarea->bloque = nodoBloque->bloque;

		if (!estadoEstable) {
			return NULL;
		}

		list_add(tablaTareas, tarea);

		if (!estadoEstable) {
			return NULL;
		}

		enviarSolicitudLecturaArchTemp(
				buscarSocketPorNombre(nodoBloque->nomNodo), nodoBloque->bloque,
				bloque);

		void eliminarNodo(t_nodoBloque * nodo) {
			free(nodo->nomNodo);
			free(nodo);
		}

		list_destroy_and_destroy_elements(listaNodoBloque,
				(void*) eliminarNodo);

		bloque++;

		if (!estadoEstable) {
			return NULL;
		}
		listaNodoBloque = buscarBloque(configArchivo, bloque);
	}

	list_destroy(listaNodoBloque);

	//Espero que lleguen todos los bloques
	while (list_size(listaTemporal) < bloque) {
		if (!estadoEstable) {
			return NULL;
		}
	}

	//Creo el archivo temporal en base a la lista
	bool odenarArchivo(t_respuestaLecturaArchTemp *primero,
			t_respuestaLecturaArchTemp *segundo) {
		return primero->orden < segundo->orden;
	}

	list_sort(listaTemporal, (void*) odenarArchivo);

	char * archivoTemporal = malloc(0);

	int desplazamiento = 0;

	int i, tamBuffer;
	for (i = 0; i < list_size(listaTemporal); i++) {
		t_respuestaLecturaArchTemp * bloque = list_get(listaTemporal, i);

		tamBuffer = buscarTamBloque(configArchivo, bloque->orden);

		archivoTemporal = realloc(archivoTemporal, desplazamiento + tamBuffer);
		memcpy(archivoTemporal + desplazamiento, bloque->data, tamBuffer);

		desplazamiento += tamBuffer;
	}

//Libero memoria
	destruirSubstring(separado);
	free(rutaFS);
	free(indexPadreChar);
	config_destroy(configArchivo);

	void destruirLista(t_respuestaLecturaArchTemp * regitro) {
		free(regitro->data);
		free(regitro);
	}
	list_destroy_and_destroy_elements(listaTemporal, (void*) destruirLista);

	return archivoTemporal;
}

t_nodoBloque * nodoMenosSaturado(t_list * listaNodoBloque) {

	bool nodoMenosCargado(t_nodoBloque * nodo, t_nodoBloque * nodoMenosCargado) {
		int cantTareasNodo = cantidadTareas(nodo->nomNodo);
		int cantTareasNodoMenosCargado = cantidadTareas(
				nodoMenosCargado->nomNodo);
		return cantTareasNodo < cantTareasNodoMenosCargado;
	}

	list_sort(listaNodoBloque, (void*) nodoMenosCargado);

	return list_get(listaNodoBloque, 0);
}

int cantidadTareas(char * nodoBloqueOriginal) {
	bool cantidadDeTareas(t_nodoBloque * tarea) {
		return string_equals_ignore_case(tarea->nomNodo, nodoBloqueOriginal);
	}
	return list_count_satisfying(tablaTareas, (void*) cantidadDeTareas);
}
