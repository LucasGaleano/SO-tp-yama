#include "tablas.h"

/*-------------------------Tabla de directorios-------------------------*/
void crearTablaDirectorios(char * rutaTablaDirectorios) {
	tablaDirectorios = list_create();

	//Creo el primer que es el root
	t_directory * registro = malloc(sizeof(t_directory));

	strncpy(registro->nombre, "root", sizeof(registro->nombre) - 1);
	registro->padre = -1;
	registro->index=0;

	list_add(tablaDirectorios, registro);

	crearArchivoTablaDirectorios(rutaTablaDirectorios);
}

void crearArchivoTablaDirectorios(char * ruta) {
	//Abro el archivo para usarlo y si no existe lo creo
	char * rutaArchivo = string_new();
	string_append(&rutaArchivo, ruta);
	string_append(&rutaArchivo, "/directorios.dat");

	FILE* file = fopen(rutaArchivo, "r");

	if (file == NULL) {
		mkdir("/home/utnso/Escritorio/metadata", 0777);
		file = fopen(rutaArchivo, "w+b");

		//Cierro el archivo
		fclose(file);

		//Creo la estructura de configuracion
		configTablaDirectorios = config_create(rutaArchivo);

	} else {

		//Creo la estructura de configuracion
		configTablaDirectorios = config_create(rutaArchivo);

		//Cierro el archivo
		fclose(file);

	}

	free(rutaArchivo);

	persistirTablaDirectorios();

}

void agregarDirectorioTabla(t_directory * registro) {
	//Verifico que haya espacio para agregar directorios
	if (tablaDirectorios->elements_count == 100) {
		printf("No se puede agregar un nuevo directorio");
		return;
	}

	//Agrego a la tabla el registro del directorio
	list_add(tablaDirectorios, registro);

	persistirTablaDirectorios();
}

void eliminarDirectorioTabla(char * nombreDirectorio, int padreDirectorio){

	bool esRegistroBuscado(t_directory * registro) {
		return string_equals_ignore_case(registro->nombre, nombreDirectorio) && registro->padre == padreDirectorio;
	}

	t_directory * registro = list_remove_by_condition(tablaDirectorios,
			(void*) esRegistroBuscado);

	free(registro->nombre);
	free(registro);

	persistirTablaDirectorios();

}

void persistirTablaDirectorios(){
	int i;
	for (i = 0; i < tablaDirectorios->elements_count; ++i) {
		t_directory * registro = list_get(tablaDirectorios, i);

		char * indexNum = string_itoa(i);

		char * nombre = string_new();
		string_append(&nombre, indexNum);
		string_append(&nombre, "_NOMBRE");

		config_set_value(configTablaDirectorios, nombre, registro->nombre);

		char * padre = string_new();
		string_append(&padre, indexNum);
		string_append(&padre, "_PADRE");

		char * padreNum;
		int registroNu = registro->padre;
		padreNum= string_itoa(registroNu);
		config_set_value(configTablaDirectorios, padre, padreNum);

		free(indexNum);
		free(padreNum);
		free(nombre);
		free(padre);
	}

	config_save(configTablaDirectorios);
}

/*-------------------------Tabla de nodos-------------------------*/
void crearTablaNodos(char * rutaTablaNodos) {
	tablaNodos = malloc(sizeof(t_tabla_nodo));

	tablaNodos->tamanio = 0;
	tablaNodos->libres = 0;

	tablaNodos->infoDeNodo = list_create();
	tablaNodos->nomNodos = list_create();

	crearArchivoTablaNodos(rutaTablaNodos);
}

void crearArchivoTablaNodos(char * ruta) {
	//Abro el archivo para usarlo y si no existe lo creo
	char * rutaArchivo = string_new();
	string_append(&rutaArchivo, ruta);
	string_append(&rutaArchivo, "/nodos.bin");

	FILE* file = fopen(rutaArchivo, "r");

	if (file == NULL) {
		mkdir("/home/utnso/Escritorio/metadata", 0777);
		file = fopen(rutaArchivo, "w+b");

		//Cierro el archivo
		fclose(file);

		//Creo la estructura de configuracion
		configTablaNodo = config_create(rutaArchivo);

		//Seteo las variables en cero
		config_set_value(configTablaNodo, "TAMANIO", "0");
		config_set_value(configTablaNodo, "LIBRE", "0");
		config_set_value(configTablaNodo, "NODOS", "[]");

		config_save(configTablaNodo);

	} else {

		//Creo la estructura de configuracion
		configTablaNodo = config_create(rutaArchivo);

		//Cierro el archivo
		fclose(file);

	}

	free(rutaArchivo);
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

void persistirTablaNodos() {
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
}

/*-------------------------Tabla de sockets-------------------------*/
void crearTablaSockets(void) {
	tablaSockets = list_create();
}

void agregarNodoTablaSockets(char * nombreNodo, int client_socket) {
	t_tabla_sockets * registroSocket = malloc(sizeof(t_tabla_nodo));
	registroSocket->nombre = malloc(string_length(nombreNodo) + 1);

	memcpy(registroSocket->nombre, nombreNodo, string_length(nombreNodo) + 1);
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
	free(registroSocket);

	return nom;
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
	configTablaBitmap = config_create(rutaArchivo);

	//Seteo valores de bitmap en 0
	int i;
	for (i = 0; i < info->total; ++i) {
		char * nombre = string_new();
		string_append(&nombre, "Bloque");
		char * numeroNodo = string_itoa(i);
		char * estado = string_itoa(0);
		string_append(&nombre, numeroNodo);
		config_set_value(configTablaBitmap, nombre, estado);
		free(nombre);
		free(numeroNodo);
		free(estado);
	}

	config_save(configTablaBitmap);

	free(rutaArchivo);
}
