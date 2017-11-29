#include "interfaz.h"

/*-------------------------Almacenar archivo-------------------------*/
void almacenarArchivo(char * rutaArchivo, char * rutaDestino, char * nomArchivo,
		int tipoArchivo) {
	size_t tamArch;

	FILE * archivofd;

	void * archivo = abrirArchivo(rutaArchivo, &tamArch, &archivofd);

//	int desplazamiento = 0;
//
//	int numeroBloque = 0;

	t_config * configTablaArchivo = crearArchivoTablaArchivo(rutaArchivo,
			rutaDestino, nomArchivo, tipoArchivo);

	dividirBloqueArchivoTexto2(archivo, configTablaArchivo);
//	while (desplazamiento < tamArch) {
//		void * buffer;
//		switch (tipoArchivo) {
//		case BINARIO:
//			buffer = dividirBloqueArchivoBinario(archivo, &desplazamiento);
//			break;
//		case TEXTO:
//			buffer = dividirBloqueArchivoTexto(archivo, &desplazamiento);
//			break;
//		default:
//			printf("No puedo enviar el archivo xq no conosco su tipo de dato");
//			return;
//			break;
//		}
//
//		//Genero el bloque original
//		char * nodoElegido = buscarNodoMenosCargado();
//		int socketNodoElegido = buscarSocketPorNombre(nodoElegido);
//		int bloqueAEscribir = buscarBloqueAEscribir(nodoElegido);
//
//		agregarRegistroTablaArchivos(nodoElegido, bloqueAEscribir, numeroBloque,
//				0, configTablaArchivo);
//
//		enviarSolicitudEscrituraBloque(socketNodoElegido, buffer,
//				bloqueAEscribir);
//
//		//Genero la copia
//		char * nodoElegidoCopia = buscarNodoMenosCargado();
//		int socketNodoElegidoCopia = buscarSocketPorNombre(nodoElegidoCopia);
//		int bloqueAEscribirCopia = buscarBloqueAEscribir(nodoElegidoCopia);
//
//		agregarRegistroTablaArchivos(nodoElegidoCopia, bloqueAEscribirCopia,
//				numeroBloque, 1, configTablaArchivo);
//
//		enviarSolicitudEscrituraBloque(socketNodoElegidoCopia, buffer,
//				bloqueAEscribirCopia);
//
//		//Bytes guardados en un bloque
//		int tamBuffer = strlen(buffer);
//		guardoBytesPorBloque(numeroBloque, tamBuffer, configTablaArchivo);
//
//		//Actualizo el numero de bloques
//		numeroBloque++;
//
//		//Libero memoria
//		free(buffer);
//	}
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

char * generarBloque2(void * archivo) {
	char *text_to_iterate = string_duplicate(archivo);

	char *next = text_to_iterate;
	char *str = text_to_iterate;

	char* token = strtok_r(str, "\n", &next);

	char * bloque = string_new();
	string_append(&bloque, token);

	free(text_to_iterate);

	return bloque;
}

char * generarBloque(void * archivo) {
	int a = 0;
	int restoArchivo = strlen(archivo);

	if (string_starts_with(archivo + a, "\n"))
		a++;

	while (!string_starts_with(archivo + a, "\n") && (restoArchivo > 0)) {
		a++;
		restoArchivo--;
	}

	char * bloque = malloc((sizeof(char) * a) + 1);
	memcpy(bloque, archivo, a);
	char * nulo = "\0";
	memcpy(bloque + a, nulo, 1);
	return bloque;
}

void * dividirBloqueArchivoTexto(void * archivo, int * desplazamiento) {

	int tamArchivo = strlen(archivo);

	int tamRestante = tamArchivo - *desplazamiento;

	char * buffer = string_new();

	int tamBuffer = 0;

	//Genero el bloque
	char * bloqueAGurdar = generarBloque(archivo + *desplazamiento);
	int tamBloqueAGuardar = strlen(bloqueAGurdar);

	//Pregunto si es el ultimo bloque
	if ((tamRestante - tamBloqueAGuardar) != 0) {
		tamBloqueAGuardar++;
	}

	while (tamRestante > 0 && (tamBuffer + tamBloqueAGuardar) < 1000000) {
		//Guardo el bloque al buffer
		string_append(&buffer, bloqueAGurdar);

		//Pregunto si es el ultimo bloque
		if ((tamRestante - tamBloqueAGuardar) != 0) {
			string_append(&buffer, "\n");
		}

		printf("Este es el buffer %s", bloqueAGurdar);

		tamRestante -= tamBloqueAGuardar;

		*desplazamiento = *desplazamiento + tamBloqueAGuardar;

		tamBuffer += tamBloqueAGuardar;

		free(bloqueAGurdar);

		//Genero el bloque
		bloqueAGurdar = generarBloque(archivo + *desplazamiento);
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
	bool nodoMenosCargado(t_nodo_info * cargado, t_nodo_info * menosCargado) {
		int cargadoNum = cargado->total - cargado->libre;
		int menosCargadoNum = menosCargado->total - menosCargado->libre;
		return (cargadoNum < menosCargadoNum);
	}

	list_sort(tablaNodos->infoDeNodo, (void*) nodoMenosCargado);

	bool tieneNodos(t_nodo_info * nodo) {
		return nodo->libre > 0;
	}

	t_nodo_info * nodo = list_find(tablaNodos->infoDeNodo, (void*) tieneNodos);

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

/*------------------------------------------------------------------------------------------------------------------------*/

int leerHastaSaltoLinea(void * archivo) {
	int tamBloqueHastaSaltoLinea = 0;
	int restoArchivo = strlen(archivo);

	if (string_starts_with(archivo + tamBloqueHastaSaltoLinea, "\n"))
		tamBloqueHastaSaltoLinea++;

	while (!string_starts_with(archivo + tamBloqueHastaSaltoLinea, "\n")
			&& (restoArchivo > 0)) {
		tamBloqueHastaSaltoLinea++;
		restoArchivo--;
	}
	return tamBloqueHastaSaltoLinea;
}

int tamBloque(void * archivo) {
	int tamBloque = 0;
	int tamRestanteArchivo = strlen(archivo + tamBloque);

	printf("Llegue hasta aca 2\n");

	//Sumo hasta que el tam sea menor que el bloque
	while ((tamBloque + leerHastaSaltoLinea(archivo + tamBloque)) < TAM_BLOQUE
			&& tamRestanteArchivo > 0) {
		tamBloque += leerHastaSaltoLinea(archivo + tamBloque);
		tamRestanteArchivo = strlen(archivo + tamBloque);
		printf("TamRestante es: %d \n",tamRestanteArchivo);

	}

	return tamBloque;
}

//Hacerlo en hilo
void enviarBuffer(t_hilo_enviar * valores) {
	void * buffer = malloc(valores->tamLectura);
	memcpy(buffer, valores->archivo, valores->tamLectura);
	enviarSolicitudEscrituraBloque(buscarSocketPorNombre(valores->nomNodo),
			buffer, valores->bloqueAEscribir);
}

void dividirBloqueArchivoTexto2(void * archivo, t_config * configTablaArchivo) {

	int desplazamiento = 0;
	int tamArchivo = strlen(archivo);
	int tamRestante = tamArchivo - desplazamiento;
	int numBloque = 0;

	printf("Llegue hasta aca \n");

	while (tamRestante > 0) {
		int tamBuffer = tamBloque(archivo + desplazamiento);

		tamRestante -= tamBuffer;

		//Genero el bloque original
		char * nodoElegido = buscarNodoMenosCargado();

		int bloqueAEscribir = buscarBloqueAEscribir(nodoElegido);

		agregarRegistroTablaArchivos(nodoElegido, bloqueAEscribir, numBloque, 0,
				configTablaArchivo);

		//Creo el thread para enviar datos
		pthread_t threadEnvioArchivos1;

		t_hilo_enviar * valores1 = malloc(sizeof(t_hilo_enviar));

		valores1->archivo = archivo;
		valores1->bloqueAEscribir = bloqueAEscribir;
		valores1->nomNodo = nodoElegido;
		valores1->tamLectura = tamBuffer;

		if (pthread_create(&threadEnvioArchivos1, NULL, (void*) enviarBuffer,
				valores1)) {
			perror("Error el crear el thread para enviar archivos.");
			exit(EXIT_FAILURE);
		}

		//Genero el bloque copia
		char * nodoElegidoCopia = buscarNodoMenosCargado();

		int bloqueAEscribirCopia = buscarBloqueAEscribir(nodoElegidoCopia);

		agregarRegistroTablaArchivos(nodoElegidoCopia, bloqueAEscribirCopia,
				numBloque, 1, configTablaArchivo);

		//Creo el thread para enviar datos
		pthread_t threadEnvioArchivos2;

		t_hilo_enviar * valores2 = malloc(sizeof(t_hilo_enviar));

		valores2->archivo = archivo;
		valores2->bloqueAEscribir = bloqueAEscribirCopia;
		valores2->nomNodo = nodoElegidoCopia;
		valores2->tamLectura = tamBuffer;

		if (pthread_create(&threadEnvioArchivos2, NULL, (void*) enviarBuffer,
				valores2)) {
			perror("Error el crear el thread para enviar archivos.");
			exit(EXIT_FAILURE);
		}

		//Bytes guardados en un bloque
		guardoBytesPorBloque(numBloque, tamBuffer, configTablaArchivo);

		desplazamiento += tamBuffer;

		numBloque++;
	}
}
