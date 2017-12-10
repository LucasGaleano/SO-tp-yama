#include "tablas.h"

/*-------------------------Tabla de directorios-------------------------*/
void crearTablaDirectorios(char * rutaTablaDirectorios) {
	tablaDirectorios = list_create();

	//Lleno el bit map de directorios
	llenarBitmap();

	crearArchivoTablaDirectorios(rutaTablaDirectorios);

	//Armo el root y lo agrego a la lista
	t_directory * registro = malloc(sizeof(t_directory));

	strncpy(registro->nombre, "root", sizeof(registro->nombre) - 1);

	registro->padre = -1;

	agregarDirectorioTabla(registro, "root");
}

void crearTablaDirectorioSegunArchivo(char * ruta) {
	tablaDirectorios = list_create();

	//Abro el archivo para usarlo
	char * rutaArchivo = string_new();
	string_append(&rutaArchivo, ruta);
	string_append(&rutaArchivo, "metadata/directorios.dat");

	//Creo la estructura de configuracion
	t_config * configTablaDirectorios = config_create(rutaArchivo);

	//Lleno el bitmap
	llenarBitmap();

	int i;

	for (i = 0; i < 100; ++i) {
		char * key = string_itoa(i);
		char * valor = config_get_string_value(configTablaDirectorios, key);
		if (valor != NULL) {
			char ** directorio = config_get_array_value(configTablaDirectorios,
					key);

			t_directory * registro = malloc(sizeof(t_directory));

			strncpy(registro->nombre, directorio[0],
					sizeof(registro->nombre) - 1);

			registro->padre = atoi(directorio[1]);

			registro->index = i;

			bitMapDirectorio[i] = false;

			list_add(tablaDirectorios, registro);

			destruirSubstring(directorio);

		}
		free(key);

	}

	//Libero memoria
	free(rutaArchivo);
	config_destroy(configTablaDirectorios);
}

void crearArchivoTablaDirectorios(char * ruta) {
	//Abro el archivo para usarlo y si no existe lo creo
	char * rutaArchivo = string_new();
	string_append(&rutaArchivo, ruta);
	string_append(&rutaArchivo, "/directorios.dat");

	FILE* file = fopen(rutaArchivo, "w+b");

	//Cierro el archivo
	fclose(file);

	//Libero memoria
	free(rutaArchivo);
}

void agregarDirectorioTabla(t_directory * registroTabla, char * ruta) {

	char * registro = armarRegistroDirectorio(registroTabla->nombre,
			registroTabla->padre);

	int index = buscarIndexLibre();

	if (index > 99) {
		printf(
				"mkdir: no se puede crear el directorio «./%s»: Se supera la cantidad de directorios maximos \n",
				ruta);
		free(registro);
		return;
	}

	char * stringIndex = string_itoa(index);

	//Abro el archivo para usarlo
	char * rutaArchivo = string_new();
	string_append(&rutaArchivo, RUTA_METADATA);
	string_append(&rutaArchivo, "metadata/directorios.dat");

	//Creo la estructura de configuracion
	t_config * configTablaDirectorios = config_create(rutaArchivo);

	config_set_value(configTablaDirectorios, stringIndex, registro);

	//Agrego a la tabla el registro del directorio
	registroTabla->index = index;

	list_add(tablaDirectorios, registroTabla);

	config_save(configTablaDirectorios);

	//Libero memoria
	free(stringIndex);
	free(registro);
	free(rutaArchivo);
	config_destroy(configTablaDirectorios);

}

void eliminarDirectorioTabla(char * nombreDirectorio, int padreDirectorio) {

	bool esRegistroBuscado(t_directory * registro) {
		return string_equals_ignore_case(registro->nombre, nombreDirectorio)
				&& registro->padre == padreDirectorio;
	}

	t_directory * registro = list_remove_by_condition(tablaDirectorios,
			(void*) esRegistroBuscado);

	char * stringIndex = string_itoa(registro->index);

	//Abro el archivo para usarlo
	char * rutaArchivo = string_new();
	string_append(&rutaArchivo, RUTA_METADATA);
	string_append(&rutaArchivo, "metadata/directorios.dat");

	//Creo la estructura de configuracion
	t_config * configTablaDirectorios = config_create(rutaArchivo);

	config_set_value(configTablaDirectorios, stringIndex, "[###]");

	config_save(configTablaDirectorios);

	bitMapDirectorio[registro->index] = true;

	//Libero memoria
	free(registro);
	free(stringIndex);
	free(rutaArchivo);
	config_destroy(configTablaDirectorios);
}

void modificarDirectorioTabla(t_directory * registroTabla, char * nombreFinal,
		int indexPadre) {

	memcpy(registroTabla->nombre, nombreFinal, string_length(nombreFinal) + 1);
	registroTabla->padre = indexPadre;

	char * registro = armarRegistroDirectorio(nombreFinal, indexPadre);

	char * stringIndex = string_itoa(registroTabla->index);

	//Abro el archivo para usarlo
	char * rutaArchivo = string_new();
	string_append(&rutaArchivo, RUTA_METADATA);
	string_append(&rutaArchivo, "metadata/directorios.dat");

	//Creo la estructura de configuracion
	t_config * configTablaDirectorios = config_create(rutaArchivo);

	config_set_value(configTablaDirectorios, stringIndex, registro);

	config_save(configTablaDirectorios);

	free(stringIndex);
	free(registro);
	free(rutaArchivo);
	config_destroy(configTablaDirectorios);
}

/*-------------------------Tabla de archivos-------------------------*/
t_config * crearArchivoTablaArchivo(char * origen, char *destino, char * nombre,
		int tipoArchivo) {
	//Copio informacion del archivo
	struct stat statArch;

	stat(origen, &statArch);

	//Busco el nombre del archivo
	char * nombreArchivo = nombre;

	//Busco el nombre del archivo
	char ** listaDestino = string_split(destino, "/");

	int posicion;

	for (posicion = 0; listaDestino[posicion] != NULL; ++posicion) {
	}

	int indexPadre;

	if (posicion == 0) {
		indexPadre = obtenerIndex("root");
	} else {
		indexPadre = obtenerIndex(listaDestino[posicion - 1]);
	}

	char * indexPadreString = string_itoa(indexPadre);

	//Creo la carpeta donde va a estar el archivo
	char * rutaArchivo = string_new();
	string_append(&rutaArchivo, "/home/utnso/Escritorio/metadata/archivos");

	mkdir(rutaArchivo, 0777);

	string_append(&rutaArchivo, "/");
	string_append(&rutaArchivo, indexPadreString);

	mkdir(rutaArchivo, 0777);

	//Abro el archivo para usarlo
	string_append(&rutaArchivo, "/");
	string_append(&rutaArchivo, nombreArchivo);

	FILE* file = fopen(rutaArchivo, "w+b");

	//Cierro el archivo
	fclose(file);

	//Creo la estructura de configuracion
	t_config * configTablaArchivo = config_create(rutaArchivo);

	//Cargo tamanio en el archivo
	int tamArc = statArch.st_size;
	char * tamArcString = string_itoa(tamArc);
	config_set_value(configTablaArchivo, "TAMANIO", tamArcString);

	//Cargo tipo en el archivo
	if (tipoArchivo == TEXTO) {
		config_set_value(configTablaArchivo, "TIPO", "TEXTO");
	} else {
		config_set_value(configTablaArchivo, "TIPO", "BINARIO");
	}

	//Cargo la cantidad de bloques totales
	char * cantBloques = string_itoa(0);
	config_set_value(configTablaArchivo, "CANTIDAD_BLOQUES", cantBloques);

	//Guardo en el archivo
	config_save(configTablaArchivo);

	//Libero memoria
	free(indexPadreString);
	free(tamArcString);
	destruirSubstring(listaDestino);
	free(rutaArchivo);
	free(cantBloques);

	return configTablaArchivo;
}

void agregarRegistroTablaArchivos(char * nodoElegido, int bloqueAEscribir,
		int bloqueDelArchivo, int numeroCopia, t_config * configTablaArchivo) {

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

	//Lo agrego a la cantidad de bloques totales
	int totalesAnterior = config_get_int_value(configTablaArchivo,
			"CANTIDAD_BLOQUES");
	totalesAnterior++;
	char * totalesActualesChar = string_itoa(totalesAnterior);
	config_set_value(configTablaArchivo, "CANTIDAD_BLOQUES",
			totalesActualesChar);

	config_save(configTablaArchivo);

	free(key);
	free(valor);
	free(numeroBloqueAEscribirChar);
	free(numeroBloqueDelArchivoChar);
	free(numeroCopiaChar);
	free(totalesActualesChar);
}

void guardoBytesPorBloque(int numeroBloque, int tamBuffer,
		t_config * configTablaArchivo) {
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

t_list * buscarBloque(t_config * configArchivo, int bloque) {
	t_list * lista = list_create();

	char * keyOriginal = string_new();
	string_append(&keyOriginal, "BLOQUE");
	char * bloqueChar = string_itoa(bloque);
	string_append(&keyOriginal, bloqueChar);
	string_append(&keyOriginal, "COPIA");

	int copia = 0;

	char * key = string_new();
	string_append(&key, keyOriginal);
	char * copiaChar = string_itoa(copia);
	string_append(&key, copiaChar);

	char * bloqueBuscado = config_get_string_value(configArchivo, key);

	while (bloqueBuscado != NULL) {
		char ** bloqueEncontrado = config_get_array_value(configArchivo, key);
		if (!bloqueNodoVacio(bloqueEncontrado)
				&& nodoDisponible(bloqueEncontrado[0])) {
			t_nodoBloque * nodoBloque = malloc(sizeof(t_nodoBloque));
			nodoBloque->bloque = atoi(bloqueEncontrado[1]);
			nodoBloque->nomNodo = strdup(bloqueEncontrado[0]);
			list_add(lista, nodoBloque);
		}

		destruirSubstring(bloqueEncontrado);
		free(key);
		free(copiaChar);

		copia++;
		key = string_new();
		string_append(&key, keyOriginal);
		copiaChar = string_itoa(copia);
		string_append(&key, copiaChar);
		bloqueBuscado = config_get_string_value(configArchivo, key);
	}

	free(key);
	free(keyOriginal);
	free(bloqueChar);
	free(copiaChar);

	return lista;
}

char ** buscarBloqueCopia(t_config * configArchivo, int bloque, int copia) {
	char * key = string_new();
	string_append(&key, "BLOQUE");
	char * bloqueChar = string_itoa(bloque);
	string_append(&key, bloqueChar);
	string_append(&key, "COPIA");
	char * copiaChar = string_itoa(copia);
	string_append(&key, copiaChar);

	char ** bloqueEncontrado = config_get_array_value(configArchivo, key);

	free(key);
	free(bloqueChar);
	free(copiaChar);

	return bloqueEncontrado;

}

int buscarTamBloque(t_config * configArchivo, int numeroBloqueArchivo) {
	char * key = string_new();
	string_append(&key, "BLOQUE");
	char * ordenChar = string_itoa(numeroBloqueArchivo);
	string_append(&key, ordenChar);
	string_append(&key, "BYTES");

	int tamBuffer = config_get_int_value(configArchivo, key);

	free(key);
	free(ordenChar);

	return tamBuffer;
}

/*-------------------------Tabla de nodos-------------------------*/
void crearTablaNodos(char * rutaTablaNodos) {
	tablaNodos = malloc(sizeof(t_tabla_nodo));

	tablaNodos->tamanio = 0;
	tablaNodos->libres = 0;

	tablaNodos->infoDeNodo = list_create();
	tablaNodos->nomNodos = list_create();

	crearArchivoTablaNodos(rutaTablaNodos);

	tablaTareas = list_create();

}

void crearArchivoTablaNodos(char * ruta) {
	//Abro el archivo para usarlo y si no existe lo creo
	char * rutaArchivo = string_new();
	string_append(&rutaArchivo, ruta);
	string_append(&rutaArchivo, "/nodos.bin");

	FILE* file = fopen(rutaArchivo, "w+b");

	//Cierro el archivo
	fclose(file);

	//Creo la estructura de configuracion
	t_config * configTablaNodo = config_create(rutaArchivo);

	//Seteo las variables en cero
	config_set_value(configTablaNodo, "TAMANIO", "0");
	config_set_value(configTablaNodo, "LIBRE", "0");
	config_set_value(configTablaNodo, "NODOS", "[]");

	config_save(configTablaNodo);

	config_destroy(configTablaNodo);

	free(rutaArchivo);
}

void crearTablaNodosSegunArchivo(char * ruta) {
	//Abro el archivo para usarlo
	char * rutaArchivo = string_new();
	string_append(&rutaArchivo, ruta);
	string_append(&rutaArchivo, "metadata/nodos.bin");

	tablaTareas = list_create();

	//Creo la estructura de configuracion
	char * rutaTabla = string_new();
	string_append(&rutaTabla, RUTA_METADATA);
	string_append(&rutaTabla, "metadata/nodos.bin");

	t_config * configTablaNodo = config_create(rutaTabla);

	//Creo la lista
	tablaNodos = malloc(sizeof(t_tabla_nodo));

	//Tamanio
	tablaNodos->tamanio = 0;

	//Libre
	tablaNodos->libres = 0;

	//Nombre de nodos
	tablaNodos->nomNodos = list_create();

	char ** nomNodos = config_get_array_value(configTablaNodo, "NODOS");
	int i;
	for (i = 0; nomNodos[i] != NULL; i++) {
		char * nomNodo = strdup(nomNodos[i]);
		list_add(tablaNodos->nomNodos, nomNodo);
	}

	//Info de cada nodo
	tablaNodos->infoDeNodo = list_create();

	char * nombreNodo = string_new();
	string_append(&nombreNodo, "Nodo");
	int numeroNodo = 1;
	char * numeroNodoChar = string_itoa(numeroNodo);
	string_append(&nombreNodo, numeroNodoChar);

	char * nombreNodoTotal = string_new();
	string_append(&nombreNodoTotal, nombreNodo);
	string_append(&nombreNodoTotal, "Total");

	char * nombreNodoLibre = string_new();
	string_append(&nombreNodoLibre, nombreNodo);
	string_append(&nombreNodoLibre, "Libre");

	while (config_has_property(configTablaNodo, nombreNodoTotal)) {
		t_nodo_info * nodo = malloc(sizeof(t_nodo_info));
		nodo->libre = config_get_int_value(configTablaNodo, nombreNodoLibre);
		nodo->total = config_get_int_value(configTablaNodo, nombreNodoTotal);
		nodo->nombre = strdup(nombreNodo);
		nodo->disponible = false;

		list_add(tablaNodos->infoDeNodo, nodo);

		numeroNodo++;

		free(nombreNodoLibre);
		free(nombreNodoTotal);
		free(nombreNodo);
		free(numeroNodoChar);

		nombreNodo = string_new();
		string_append(&nombreNodo, "Nodo");
		numeroNodoChar = string_itoa(numeroNodo);
		string_append(&nombreNodo, numeroNodoChar);

		nombreNodoTotal = string_new();
		string_append(&nombreNodoTotal, nombreNodo);
		string_append(&nombreNodoTotal, "Total");

		nombreNodoLibre = string_new();
		string_append(&nombreNodoLibre, nombreNodo);
		string_append(&nombreNodoLibre, "Libre");

	}
	free(nombreNodoLibre);
	free(nombreNodoTotal);
	free(nombreNodo);
	free(numeroNodoChar);
	free(rutaArchivo);
	destruirSubstring(nomNodos);
	free(rutaTabla);
	config_destroy(configTablaNodo);
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

void liberarBloqueTablaNodos(char * nomNodo, int bloque) {
	int libre = tablaNodos->libres + 1;
	memcpy(&tablaNodos->libres, &libre, sizeof(int));

//Modifico de la tabla de nodos de la lista de info
	bool esSocketBuscadoInfo(t_nodo_info * nodo) {
		return string_equals_ignore_case(nodo->nombre, nomNodo);
	}

	t_nodo_info * info = list_find(tablaNodos->infoDeNodo,
			(void*) esSocketBuscadoInfo);

	int libreNodo = info->libre + 1;
	memcpy(&info->libre, &libreNodo, sizeof(int));

//Modifico su bitMap
	liberarBloquebitMap(nomNodo, bloque);

	persistirTablaNodos();
}

void persistirTablaNodos() {

	//Creo la estructura de configuracion
	char * rutaTabla = string_new();
	string_append(&rutaTabla, RUTA_METADATA);
	string_append(&rutaTabla, "metadata/nodos.bin");

	t_config * configTablaNodo = config_create(rutaTabla);

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

	free(rutaTabla);
	config_destroy(configTablaNodo);

}

void quitarEspacioNodo(char * nomNodo) {
	bool esNodoBuscado(t_nodo_info * nodo) {
		return string_equals_ignore_case(nodo->nombre, nomNodo);
	}

	t_nodo_info * nodo = list_find(tablaNodos->infoDeNodo,
			(void*) esNodoBuscado);

	//Actualizo tabla de nodos
	tablaNodos->libres--;
	nodo->libre--;
	persistirTablaNodos();
}

/*-------------------------Tabla de sockets-------------------------*/
void crearTablaSockets(void) {
	tablaSockets = list_create();
}

void agregarNodoTablaSockets(char * nombreNodo, int client_socket, char * ip,
		char *puerto) {
	t_tabla_sockets * registroSocket = malloc(sizeof(t_tabla_nodo));

	registroSocket->nombre = strdup(nombreNodo);
	registroSocket->ip = strdup(ip);
	registroSocket->puerto = strdup(puerto);

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
	free(registroSocket->ip);
	free(registroSocket->puerto);
	free(registroSocket);

	return nom;
}

int buscarSocketPorNombre(char * nombreNodo) {
	bool esSocketBuscado(t_tabla_sockets* nodo) {
		return string_equals_ignore_case(nodo->nombre, nombreNodo);
	}

	t_tabla_sockets * registroSocket = list_find(tablaSockets,
			(void*) esSocketBuscado);

	return registroSocket->socket;
}

char* buscarNombrePorSocket(int socket) {
	bool esNombreBuscado(t_tabla_sockets* nodo) {
		return (nodo->socket == socket);
	}

	t_tabla_sockets * registroSocket = list_find(tablaSockets,
			(void*) esNombreBuscado);

	return registroSocket->nombre;
}

void modificarNodoTablaSockets(char * nombreNodo, int client_socket) {
	bool esNodoBuscado(char * nodo) {
		return string_equals_ignore_case(nodo, nombreNodo);
	}

	t_tabla_sockets * registro = list_find(tablaNodos->nomNodos,
			(void*) esNodoBuscado);

	registro->socket = client_socket;
}

t_tabla_sockets_ip_puerto * buscarIpPuertoPorNombre(char * nombreNodo) {
	bool esNombreBuscado(t_tabla_sockets * nodo) {
		return string_ends_with(nombreNodo, nodo->nombre);
	}

	t_tabla_sockets * registroSocket = list_find(tablaSockets,
			(void*) esNombreBuscado);

	t_tabla_sockets_ip_puerto * ipPuerto = malloc(
			sizeof(t_tabla_sockets_ip_puerto));
	ipPuerto->ip = strdup(registroSocket->ip);
	ipPuerto->puerto = strdup(registroSocket->puerto);

	return ipPuerto;

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
	t_config * configTablaBitmap = config_create(rutaArchivo);

//Seteo valores de bitmap en 0
	int i;
	for (i = 0; i < info->total; ++i) {
		char * nombre = string_new();
		string_append(&nombre, "Bloque");
		char * numeroNodo = string_itoa(i);
		char * estado = string_itoa(true);
		string_append(&nombre, numeroNodo);
		config_set_value(configTablaBitmap, nombre, estado);
		free(nombre);
		free(numeroNodo);
		free(estado);
	}

	config_save(configTablaBitmap);

	config_destroy(configTablaBitmap);
	free(rutaArchivo);
}

int buscarBloqueLibre(t_config * tablaBitMaps) {
	int cantBloquesBitMaps = config_keys_amount(tablaBitMaps);
	int i;
	bool encontreBloqueLibre = false;

	for (i = 0; i < cantBloquesBitMaps && !encontreBloqueLibre; ++i) {
		char * nombreBloque = string_new();
		string_append(&nombreBloque, "Bloque");
		char * numeroNodo = string_itoa(i);
		string_append(&nombreBloque, numeroNodo);

		int estado = config_get_int_value(tablaBitMaps, nombreBloque);

		if (estado == 1) {
			encontreBloqueLibre = true;
			char * nuevoEstado = string_itoa(false);
			config_set_value(tablaBitMaps, nombreBloque, nuevoEstado);
			config_save(tablaBitMaps);
			free(nuevoEstado);
			i--;
		}

		free(nombreBloque);
		free(numeroNodo);
	}

	config_destroy(tablaBitMaps);

	return i;
}

void liberarBloquebitMap(char * nomNodo, int bloque) {
	char * rutaBitMap = string_new();
	string_append(&rutaBitMap, "/home/utnso/Escritorio/metadata/bitmaps/");
	string_append(&rutaBitMap, nomNodo);
	string_append(&rutaBitMap, ".dat");

	t_config * configBitMap = config_create(rutaBitMap);

	char * key = string_new();
	string_append(&key, "Bloque");
	char * bloqueChar = string_itoa(bloque);
	string_append(&key, bloqueChar);
	char * valor = string_itoa(true);

	config_set_value(configBitMap, key, valor);

	config_save(configBitMap);

//Libero memoria
	free(rutaBitMap);
	config_destroy(configBitMap);
	free(key);
	free(bloqueChar);
	free(valor);
}

/*-------------------------Eliminar listas-------------------------*/
void destruirTablaSockets() {
	//Verifico que no este vacio
	if (tablaSockets == NULL)
		return;

	void eliminarRegistroTablaSockets(t_tabla_sockets * registro) {
		free(registro->ip);
		free(registro->nombre);
		free(registro->puerto);
		free(registro);
	}
	list_destroy_and_destroy_elements(tablaSockets,
			(void*) eliminarRegistroTablaSockets);
}

void destruirTablaNodos() {
	//Verifico que no este vacio
	if (tablaNodos == NULL)
		return;

	//Elimino la lista de info de Nodos
	void eliminarRegistroInfoNodo(t_nodo_info * registro) {
		free(registro->nombre);
		free(registro);
	}
	list_destroy_and_destroy_elements(tablaNodos->infoDeNodo,
			(void*) eliminarRegistroInfoNodo);

	//Elimino la lista de nombre de Nodos
	void eliminarRegistroNombreNodo(char * registro) {
		free(registro);
	}
	list_destroy_and_destroy_elements(tablaNodos->nomNodos,
			(void*) eliminarRegistroNombreNodo);

	//Elimino el puntero a la estructura
	free(tablaNodos);
}

void destruirTablaDirectorios() {
	//Verifico que no este vacio
	if (tablaDirectorios == NULL)
		return;

	void eliminarRegistroDirectorio(t_directory * registro) {
		free(registro);
	}

	list_destroy_and_destroy_elements(tablaDirectorios,
			(void*) eliminarRegistroDirectorio);
}

void destruirTablaTareas() {
	//Verifico que no este vacio
	if (tablaTareas == NULL)
		return;

	void eliminarRegistroTarea(t_nodoBloque * registro) {
		free(registro->nomNodo);
		free(registro);
	}
	list_destroy_and_destroy_elements(tablaTareas,
			(void*) eliminarRegistroTarea);
}

/*-------------------------Funciones auxiliares-------------------------*/
char * armarRegistroDirectorio(char * nombreDirectorio, int indexPadre) {
	char * registro = string_new();
	string_append(&registro, "[ ");
	string_append(&registro, nombreDirectorio);
	string_append(&registro, ", ");
	char * stringIndexPadre = string_itoa(indexPadre);
	string_append(&registro, stringIndexPadre);
	string_append(&registro, "]");

	free(stringIndexPadre);

	return registro;
}

void llenarBitmap() {
	int i;
	for (i = 0; i < 100; ++i) {
		bitMapDirectorio[i] = true;
	}
}

int buscarIndexLibre() {
	int index = 0;

	while (!bitMapDirectorio[index]) {
		index++;
	}

	if (index < 99)
		bitMapDirectorio[index] = false;

	return index;
}

int obtenerIndex(char * nombre) {
	bool esPadreBuscado(t_directory * registro) {
		return string_equals_ignore_case(registro->nombre, nombre);
	}

	t_directory *registro = list_find(tablaDirectorios, (void*) esPadreBuscado);

	if (registro == NULL) {
		return -1;
	}

	return registro->index;
}

void destruirSubstring(char ** sub) {
	int i;
	for (i = 0; sub[i] != NULL; ++i) {
		free(sub[i]);
	}
	free(sub[i]);
	free(sub);
}

bool bloqueNodoVacio(char ** bloque) {
	return string_equals_ignore_case(bloque[1], "#");
}

bool nodoDisponible(char * nomNodo) {
	bool estoyLibre(t_nodo_info * nodo) {
		return string_equals_ignore_case(nomNodo, nodo->nombre) && nodo->disponible;
	}

	return list_any_satisfy(tablaNodos->infoDeNodo, (void*) estoyLibre);
}

