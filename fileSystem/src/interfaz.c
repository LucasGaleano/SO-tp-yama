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
	string_append(&rutaConfig, "/home/utnso/Escritorio/metadata/bitmaps/");
	string_append(&rutaConfig, nombreNodo);
	string_append(&rutaConfig, ".dat");

	t_config * configBitMap = config_create(rutaConfig);

	free(rutaConfig);

	return buscarBloqueLibre(configBitMap);
}

