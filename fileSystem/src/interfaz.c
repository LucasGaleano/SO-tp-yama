#include "interfaz.h"

/*-------------------------Almacenar archivo-------------------------*/
void almacenarArchivo(char * rutaArchivo, char * rutaDestino, char * nomArchivo,
		int tipoArchivo) {
	size_t tamArch;

	FILE * archivofd;

	void * archivo = abrirArchivo(rutaArchivo, &tamArch, &archivofd);

	if (archivo == NULL) {
		printf("%s: No existe el archivo o el directorio", rutaArchivo);
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
			buffer = dividirBloqueArchivoTexto(archivo, &desplazamiento);
			tamBuffer = strlen(buffer);
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

		//Actualizo el numero de bloques
		numeroBloque++;

		//Libero memoria
		free(buffer);
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

void * dividirBloqueArchivoTexto(void * archivo, int * desplazamiento) {

	int tamArchivo = strlen(archivo);

	int tamRestante = tamArchivo - *desplazamiento;

	char * buffer = string_new();

	int tamBuffer = 0;

	//Genero el bloque
	char * bloqueAGurdar = generarBloque(archivo + *desplazamiento,
			tamRestante);
	int tamBloqueAGuardar = strlen(bloqueAGurdar);

	//Pregunto si es el ultimo bloque
	if ((tamRestante - tamBloqueAGuardar) != 0) {
		tamBloqueAGuardar++;
	}

	while (tamRestante > 0 && (tamBuffer + tamBloqueAGuardar) < TAM_BLOQUE) {
		//Guardo el bloque al buffer
		string_append(&buffer, bloqueAGurdar);

		//Pregunto si es el ultimo bloque
		if ((tamRestante - tamBloqueAGuardar) != 0) {
			string_append(&buffer, "\n");
		}

		tamRestante -= tamBloqueAGuardar;

		*desplazamiento = *desplazamiento + tamBloqueAGuardar;

		tamBuffer += tamBloqueAGuardar;

		free(bloqueAGurdar);

		//Genero el bloque
		bloqueAGurdar = generarBloque(archivo + *desplazamiento, tamRestante);
		tamBloqueAGuardar = strlen(bloqueAGurdar);

		//Pregunto si es el ultimo bloque
		if ((tamRestante - tamBloqueAGuardar) != 0) {
			tamBloqueAGuardar++;
		}

	}

	free(bloqueAGurdar);

	return buffer;
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

char * generarBloque(void * archivo, int tamRestante) {

	int desplazamiento = 0;
	int tamALeer = 0;

	if (string_starts_with(archivo, "\n"))
		desplazamiento++;

	void * prueba = strchr(archivo + desplazamiento, '\n');

	if (prueba == NULL) {
		tamALeer += tamRestante;
		desplazamiento = 0;
	} else {
		tamALeer += prueba - archivo;
	}

	char * bloque = malloc(tamALeer + 1);
	char * finBloque = "\0";
	memcpy(bloque, archivo + desplazamiento, tamALeer);
	memcpy(bloque + tamALeer, finBloque, 1);

	return bloque;
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
	int cantidadDeBloques = config_get_int_value(configArchivo,"CANTIDAD_BLOQUES");

	int i;

	for (i = 0; i < cantidadDeBloques; ++i) {

		char ** nodoBloqueOriginal = buscarBloque(configArchivo, i, 0);
		char ** nodoBloqueCopia = buscarBloque(configArchivo, i, 1);

		t_tarea * tarea = nodoMenosSaturado(nodoBloqueOriginal,
				nodoBloqueCopia);

		list_add(tablaTareas, tarea);

		enviarSolicitudLecturaArchTemp(buscarSocketPorNombre(tarea->nomNodo),
				tarea->bloque, i);

		destruirSubstring(nodoBloqueOriginal);
		destruirSubstring(nodoBloqueCopia);
	}

	//Espero que lleguen todos los bloques
	while (list_size(listaTemporal) < i)
		;

	//Creo el archivo temporal en base a la lista
	bool odenarArchivo(t_respuestaLecturaArchTemp *primero,
			t_respuestaLecturaArchTemp *segundo) {
		return primero->orden < segundo->orden;
	}

	list_sort(listaTemporal, (void*) odenarArchivo);

	char * archivoTemporal = malloc(0);

	int desplazamiento = 0;

	for (i = 0; i < list_size(listaTemporal); i++) {
		t_respuestaLecturaArchTemp * bloque = list_get(listaTemporal, i);

		int tamBuffer = buscarTamBloque(configArchivo, bloque->orden);

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

t_tarea * nodoMenosSaturado(char ** nodoBloqueOriginal, char ** nodoBloqueCopia) {

	t_tarea * tarea = malloc(sizeof(t_tarea));

	if (!nodoDisponible(nodoBloqueOriginal[0])) {
		int tamNombre = strlen(nodoBloqueCopia[0]) + 1;
		tarea->nomNodo = malloc(tamNombre);
		memcpy(tarea->nomNodo, nodoBloqueCopia[0], tamNombre);
		tarea->bloque = atoi(nodoBloqueCopia[1]);
		return tarea;
	}

	if (!nodoDisponible(nodoBloqueCopia[0])) {
		int tamNombre = strlen(nodoBloqueOriginal[0]) + 1;
		tarea->nomNodo = malloc(tamNombre);
		memcpy(tarea->nomNodo, nodoBloqueOriginal[0], tamNombre);
		tarea->bloque = atoi(nodoBloqueOriginal[1]);
		return tarea;
	}

	int cumplenOriginal = cantidadTareas(nodoBloqueOriginal);

	int cumplenCopia = cantidadTareas(nodoBloqueCopia);

	if (cumplenOriginal > cumplenCopia) {
		int tamNombre = strlen(nodoBloqueCopia[0]) + 1;
		tarea->nomNodo = malloc(tamNombre);
		memcpy(tarea->nomNodo, nodoBloqueCopia[0], tamNombre);
		tarea->bloque = atoi(nodoBloqueCopia[1]);
	} else {
		int tamNombre = strlen(nodoBloqueOriginal[0]) + 1;
		tarea->nomNodo = malloc(tamNombre);
		memcpy(tarea->nomNodo, nodoBloqueOriginal[0], tamNombre);
		tarea->bloque = atoi(nodoBloqueOriginal[1]);
	}

	return tarea;
}

int cantidadTareas(char ** nodoBloqueOriginal) {
	bool cantidadDeTareas(char ** tarea) {
		return string_equals_ignore_case(tarea[0], nodoBloqueOriginal[0]);
	}

	return list_count_satisfying(tablaTareas, (void*) cantidadDeTareas);

}

bool nodoDisponible(char * nomNodo) {
	bool estaDisponible(t_nodo_info * nodo) {
		return nodo->disponible
				&& string_equals_ignore_case(nodo->nombre, nomNodo);
	}

	return list_any_satisfy(tablaNodos->infoDeNodo, (void*) estaDisponible);
}
