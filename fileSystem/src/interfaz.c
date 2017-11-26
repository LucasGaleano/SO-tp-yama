#include "interfaz.h"

/*-------------------------Almacenar archivo-------------------------*/
void almacenarArchivo(char * rutaArchivo, char * rutaDestino, char * nomArchivo,
		int tipoArchivo) {
	size_t tamArch;

	FILE * archivofd;

	void * archivo = abrirArchivo(rutaArchivo, &tamArch, &archivofd);

	int desplazamiento = 0;

	int numeroBloque = 0;

	t_config * configTablaArchivo = crearArchivoTablaArchivo(rutaArchivo,
			rutaDestino, nomArchivo, tipoArchivo);

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

		agregarRegistroTablaArchivos(nodoElegido, bloqueAEscribir, numeroBloque,
				0, configTablaArchivo);

		enviarSolicitudEscrituraBloque(socketNodoElegido, buffer,
				bloqueAEscribir);

		//Genero la copia
		char * nodoElegidoCopia = buscarNodoMenosCargado();
		int socketNodoElegidoCopia = buscarSocketPorNombre(nodoElegidoCopia);
		int bloqueAEscribirCopia = buscarBloqueAEscribir(nodoElegidoCopia);

		agregarRegistroTablaArchivos(nodoElegidoCopia, bloqueAEscribirCopia,
				numeroBloque, 1, configTablaArchivo);

		enviarSolicitudEscrituraBloque(socketNodoElegidoCopia, buffer,
				bloqueAEscribirCopia);

		//Bytes guardados en un bloque
		int tamBuffer = strlen((char*) buffer);
		guardoBytesPorBloque(numeroBloque, tamBuffer, configTablaArchivo);

		//Actualizo el numero de bloques
		numeroBloque++;

		//Libero memoria
		free(buffer);
	}
	config_destroy(configTablaArchivo);
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

	if (archivoSeparado[i + 1] != NULL)
		tamProximoBloque++;

	if (tamProximoBloque > TAM_BLOQUE)
		return NULL;

	char * buffer = string_new();

	char * bufferInterno = string_new();

	string_append(&bufferInterno, archivoSeparado[i]);
	string_append(&bufferInterno, "\n");

	string_append((char**) &buffer, bufferInterno);

	i++;

	int tamBuffer = tamProximoBloque;

	if (archivoSeparado[i] != NULL) {
		tamProximoBloque = string_length(archivoSeparado[i]);
		if (archivoSeparado[i + 1] != NULL)
			tamProximoBloque++;
	}

	free(bufferInterno);

	while (TAM_BLOQUE >= (tamBuffer + tamProximoBloque)
			&& archivoSeparado[i + 1] != NULL && archivoSeparado[i] != NULL) {

		char * bufferInterno = string_new();
		string_append(&bufferInterno, archivoSeparado[i]);
		string_append(&bufferInterno, "\n");

		string_append((char**) &buffer, bufferInterno);

		i++;

		tamBuffer += tamProximoBloque;

		if (archivoSeparado[i + 1] != NULL) {
			tamProximoBloque = string_length(archivoSeparado[i + 1]);
			if (archivoSeparado[i + 2] != NULL)
				tamProximoBloque++;
		}

		free(bufferInterno);
	}

	if (TAM_BLOQUE >= (tamBuffer + tamProximoBloque)) {
		buffer = realloc(buffer, tamBuffer + tamProximoBloque);
		memcpy(buffer + tamBuffer, archivoSeparado[i], tamProximoBloque);
		tamBuffer += tamProximoBloque;

	}

	*desplazamiento += tamBuffer;

	//Libero memoria
	destruirSubstring(archivoSeparado);

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
	char * rutaConfig = string_new();
	string_append(&rutaConfig, RUTA_METADATA);
	string_append(&rutaConfig, "metadata/bitmaps/");
	string_append(&rutaConfig, nombreNodo);
	string_append(&rutaConfig, ".dat");

	t_config * configBitMap = config_create(rutaConfig);

	free(rutaConfig);

	return buscarBloqueLibre(configBitMap);
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
		indexPadre = obtenerIndexPadre("root");
	} else {
		indexPadre = obtenerIndexPadre(separado[posicion - 1]);
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

	//Busco los bloques en los nodos
	int cantidadDeBloques = (config_keys_amount(configArchivo) - 2) / 3;

	int i;

	for (i = 0; i < cantidadDeBloques; ++i) {

		char ** nodoBloqueOriginal = buscarBloque(configArchivo, i, 0);
		char ** nodoBloqueCopia = buscarBloque(configArchivo, i, 1);

		t_tarea * tarea = nodoMenosSaturado(nodoBloqueOriginal,
				nodoBloqueCopia);

		list_add(tablaTareas, tarea);

		enviarSolicitudLecturaArchTemp(buscarSocketPorNombre(tarea->nomNodo),tarea->bloque, i);

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

	char * archivoTemporal = string_new();
	for (i = 0; i < list_size(listaTemporal); i++) {
		t_respuestaLecturaArchTemp * bloque = list_get(listaTemporal, i);
		string_append(&archivoTemporal, (char*) bloque->data);
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
