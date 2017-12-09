#include "consola.h"

/*------------------------------Consola------------------------------*/
void iniciarConsola() {
	char * linea;

	bool ejecutar = true;

	while (ejecutar) {
		linea = readline(">");

		if (linea) {
			add_history(linea);
		} else {
			free(linea);
			break;
		}

		ejecutarComando(linea, &ejecutar);

		free(linea);
	}

	clear_history();
}

void ejecutarComando(char * linea, bool * ejecutar) {
	//MAN
	if (string_equals_ignore_case(linea, "man")) {
		ejecutarMan();
		return;
	}

	//EXIT
	if (string_equals_ignore_case(linea, "exit")) {
		ejecutarExit(ejecutar);
		return;
	}

	//FORMAT
	if (string_equals_ignore_case(linea, "format")) {
		formatearFilesystem();
		return;
	}

	//ELIMINAR DIRECTORIO
	if (string_starts_with(linea, "rm -d")) {
		eliminarDirectorio(linea);
		return;
	}

	//ELIMINAR BLOQUE
	if (string_starts_with(linea, "rm -b")) {
		eliminarBloque(linea);
		return;
	}

	//ELIMINAR ARCHIVO
	if (string_starts_with(linea, "rm")) {
		eliminarArchivo(linea);
		return;
	}

	//RENOMBRAR ARCHIVO O DIRECTORIO
	if (string_starts_with(linea, "rename")) {
		modificar(linea);
		return;
	}

	//MOVER ARCHIVO O DIRECTORIO
	if (string_starts_with(linea, "mv")) {
		modificar(linea);
		return;
	}

	//MOSTRAR CONTENIDO ARCHIVO
	if (string_starts_with(linea, "cat")) {
		mostrarContenidoArchivo(linea);
		return;
	}

	//CREAR DIRECTORIO
	if (string_starts_with(linea, "mkdir")) {
		crearDirectorio(linea);
		return;
	}

	//COPIO ARCHIVO LOCAL AL FILE SYSTEM SIGUIENDO LA INTERFAZ DEL FILE SYSTEM
	if (string_starts_with(linea, "cpfrom")) {
		copiarArchivoLocalAlYamafsInterfaz(linea);
		return;
	}

	//COPIO ARCHIVO LOCAL AL FILE SYSTEM
	if (string_starts_with(linea, "cpto")) {
		copiarArchivoYamafsALocal(linea);
		return;
	}

	//CREAR COPIA DE BLOQUE EN NODO
	if (string_starts_with(linea, "cpblock")) {
		crearCopiaBloqueEnNodo(linea);
		return;
	}

	//SOLICITAR HASH DEL ARCHIVO
	if (string_starts_with(linea, "md5")) {
		solicitarHash(linea);
		return;
	}

	//LISTAR ARCHIVOS DE UN DIRECTORIO
	if (string_starts_with(linea, "ls")) {
		listarArchivos(linea);
		return;
	}

	//MOSTRAR INFO DEL ARCHIVO
	if (string_starts_with(linea, "info")) {
		mostrarInfo(linea);
		return;
	}

	//PRUEBAS
	if (string_starts_with(linea, "pruebaT")) {
		almacenarArchivo("/home/utnso/Escritorio/nombres.csv", "base", "prueba",
				TEXTO);

		return;
	}

	if (string_starts_with(linea, "pruebaB")) {
		almacenarArchivo("/home/utnso/Escritorio/Feraligatr4sotw-1.png", "base",
				"prueba", BINARIO);

		return;
	}

	//NO RECONOZCO EL COMANDO
	printf("No se ha encontrado el comando %s \n", linea);
}

/*------------------------------Comandos------------------------------*/
void ejecutarMan() {
	printf("NAME \n");
	printf("	consola \n\n");

	printf("SYNOPSIS \n");
	printf("	#include <consola.h> \n\n");

	printf("	void format(void) \n");
	printf("	void rm(char * path_archivo) \n");
	printf("	void rm -d(char * path_directorio) \n");
	printf(
			"	void rm -b(char * path_archivo, int nro_bloque, int nro_copia) \n");
	printf("	void rename(char * path_original, char * nombre_final) \n");
	printf("	void mv(char * path_original, char * path_final) \n");
	printf("	void cat(char * path_archivo) \n");
	printf("	void mkdir(char * path_dir) \n");
	printf(
			"	void cpfrom(char * path_archivo_origen, char * directorio_yamafs, char * tipo_archivo) \n");
	printf(
			"	void cpto(char * path_archivo_yamafs, char * directorio_filesystem) \n");
	printf(
			"	void cpblock(char * path_archivo, int nro_bloque, char * id_nodo) \n");
	printf("	void md5(char * path_archivo_yamafs) \n");
	printf("	void ls(char * path_directorio) \n");
	printf("	void info(char * path_archivo) \n");
	printf("	void exit(void) \n\n");

	printf("DESCRIPTION \n");
	printf("	format --> Formatear el Filesystem \n");
	printf("	rm --> Eliminar un Archivo \n");
	printf(
			"	rm -d --> Eliminar un Directorio. Si un directorio a eliminar no se encuentra vacío, la operación debe fallar \n");
	printf(
			"	rm -b --> Eliminar un Bloque. Si el bloque a eliminar fuera la última copia del mismo, se abortara la operación informando lo sucedido \n");
	printf("	rename --> Renombra un Archivo o Directorio \n");
	printf("	mv --> Mueve un Archivo o Directorio \n");
	printf("	cat --> Muestra el contenido del archivo como texto plano \n");
	printf(
			"	mkdir --> Crea un directorio. Si el directorio ya existe, el comando deberá informarlo \n");
	printf(
			"	cpfrom --> Copiar un archivo local al yamafs, siguiendo los lineamientos en la operaciòn Almacenar Archivo, de la Interfaz del FileSystem \n");
	printf("	cpto --> Copiar un archivo local al yamafs \n");
	printf(
			"	cpblock --> Crea una copia de un bloque de un archivo en el nodo dado \n");
	printf("	md5 --> Solicitar el MD5 de un archivo en yamafs \n");
	printf("	ls --> Lista los archivos de un directorio \n");
	printf(
			"	info --> Muestra toda la información del archivo, incluyendo tamaño, bloques, ubicación de los bloques, etc. \n");
	printf("	exit --> Cierra la consola \n\n");
}

void ejecutarExit(bool * ejecutar) {
	printf("Se cerro la consola \n");
	*ejecutar = false;
}

void formatearFilesystem() {
	printf("Me piden formatear el file system \n");
}

void eliminarArchivo(char * linea) {
	char * path_archivo = obtenerParametro(linea, 1);

	if (path_archivo == NULL)
		return;

	//Busco el nombre del directorio
	char ** separado = string_split(path_archivo, "/");

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

	//Verifico que hayan ingresado bien el path
	if (configArchivo == NULL) {
		printf("rm: no se puede borrar «./%s»: El archivo no existe \n",
				path_archivo);

		//Libero memoria
		free(path_archivo);
		destruirSubstring(separado);
		free(rutaFS);
		free(indexPadreChar);

		return;
	}

	//Marco como libre los bloques en el bitmap de cada nodo
	int cantBloques = config_keys_amount(configArchivo);
	cantBloques -= 2;
	cantBloques = cantBloques / 3;
	cantBloques *= 2;

	int i;
	int numeroBloque = 0;
	int numeroCopia = 0;

	for (i = 0; i < cantBloques; i++) {
		char ** nodoBloqueABorar = buscarBloqueABorar(i, &numeroCopia,
				&numeroBloque, configArchivo);

		int numeroBloque = atoi(nodoBloqueABorar[1]);

		liberarBloqueTablaNodos(nodoBloqueABorar[0], numeroBloque);

		destruirSubstring(nodoBloqueABorar);
	}

	//Borro el archivo de la tabla de archivos
	remove(rutaFS);

	char * rutaDirectorioArchivo = string_new();
	string_append(&rutaDirectorioArchivo, RUTA_METADATA);
	string_append(&rutaDirectorioArchivo, "metadata/archivos/");
	string_append(&rutaDirectorioArchivo, indexPadreChar);

	int cantArchivos = cantArchivosEnDirectorio(rutaDirectorioArchivo);

	if (cantArchivos == 0) {
		rmdir(rutaDirectorioArchivo);
	}

	//Libero memoria
	free(path_archivo);
	free(rutaFS);
	free(indexPadreChar);
	destruirSubstring(separado);
	config_destroy(configArchivo);
	free(rutaDirectorioArchivo);
}

void eliminarDirectorio(char * linea) {
	char * path_directorio = obtenerParametro(linea, 2);

	if (path_directorio == NULL)
		return;

	//Busco el nombre del directorio
	char ** separado = string_split(path_directorio, "/");

	int posicion;

	for (posicion = 0; separado[posicion] != NULL; ++posicion) {
	}

	posicion -= 1;

	//Busco el directorio que voy a eliminar
	int indexPadre;

	if (posicion == 0) {
		indexPadre = obtenerIndex("root");
	} else {
		indexPadre = obtenerIndex(separado[posicion - 1]);
	}

	bool esRegistroBuscado(t_directory * registro) {
		return string_equals_ignore_case(registro->nombre, separado[posicion])
				&& registro->padre == indexPadre;
	}

	t_directory * registroDirectorio = list_find(tablaDirectorios,
			(void*) esRegistroBuscado);

	if (registroDirectorio == NULL) {
		printf("rm -d: no se puede borrar «./%s»: El directorio no existe \n",
				path_directorio);
		destruirSubstring(separado);
		free(path_directorio);
		return;

	}

	//Busco los hijos del directorio
	bool tengoHijos(t_directory * registro) {
		return registro->padre == registroDirectorio->index;
	}

	if (list_any_satisfy(tablaDirectorios, (void*) tengoHijos)) {
		printf(
				"rm -d: no se puede borrar «./%s»: El directorio no está vacío \n",
				path_directorio);
		destruirSubstring(separado);
		free(path_directorio);
		return;
	}

	eliminarDirectorioTabla(registroDirectorio->nombre,
			registroDirectorio->padre);

	//Libero memoria
	free(path_directorio);
	destruirSubstring(separado);
}

void eliminarBloque(char * linea) {
	char * path_archivo = obtenerParametro(linea, 2);

	if (path_archivo == NULL)
		return;

	char * nro_bloque = obtenerParametro(linea, 3);

	if (nro_bloque == NULL) {
		free(path_archivo);
		return;
	}

	char * nro_copia = obtenerParametro(linea, 4);
	if (nro_copia == NULL) {
		free(path_archivo);
		free(nro_bloque);
		return;
	}

	//Busco el nombre del directorio
	char ** separado = string_split(path_archivo, "/");

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

	//Verifico que hayan ingresado bien el path
	if (configArchivo == NULL) {
		printf(
				"rm -b: no se puede borrar el bloque: %s copia: %s del archivo «./%s»: El archivo no existe \n",
				nro_bloque, nro_copia, path_archivo);

		//Libero memoria
		free(path_archivo);
		free(nro_bloque);
		free(nro_copia);
		destruirSubstring(separado);
		free(rutaFS);
		free(indexPadreChar);

		return;
	}

	//Armo el bloque a buscar
	char * key = string_new();
	string_append(&key, "BLOQUE");
	string_append(&key, nro_bloque);
	string_append(&key, "COPIA");
	string_append(&key, nro_copia);

	//Verifico que el bloque exista
	if (!config_has_property(configArchivo, key)) {
		printf(
				"rm -b: no se puede borrar el bloque: %s copia: %s del archivo «./%s»: El bloque no existe \n",
				nro_bloque, nro_copia, path_archivo);

		//Libero memoria
		free(path_archivo);
		free(nro_bloque);
		free(nro_copia);
		destruirSubstring(separado);
		free(rutaFS);
		free(indexPadreChar);
		free(key);
		config_destroy(configArchivo);

		return;
	}

	//Verifico que el bloque no este borrado
	char * verificador = config_get_string_value(configArchivo, key);
	if (string_equals_ignore_case(verificador, "###")) {
		printf(
				"rm -b: no se puede borrar el bloque: %s copia: %s del archivo «./%s»: El bloque ya fue borrado \n",
				nro_bloque, nro_copia, path_archivo);

		//Libero memoria
		free(path_archivo);
		free(nro_bloque);
		free(nro_copia);
		destruirSubstring(separado);
		free(rutaFS);
		free(indexPadreChar);
		config_destroy(configArchivo);
		free(key);

		return;
	}

	//Verifico que el bloque no sea el ultimo
	bool noPuedoBorar = true;

	//Armo la copia del bloque a buscar
	char * keyCopia = string_new();
	string_append(&keyCopia, "BLOQUE");
	string_append(&keyCopia, nro_bloque);
	string_append(&keyCopia, "COPIA");
	int i = 0;
	char * numeroCopia = string_itoa(i);
	string_append(&keyCopia, numeroCopia);

	while (config_has_property(configArchivo, keyCopia)) {
		char * verificador2 = config_get_string_value(configArchivo, keyCopia);
		if (!string_equals_ignore_case(verificador2, "###")
				&& !string_equals_ignore_case(key, keyCopia)) {
			noPuedoBorar = false;
		}
		free(keyCopia);
		free(numeroCopia);
		i++;
		keyCopia = string_new();
		string_append(&keyCopia, "BLOQUE");
		string_append(&keyCopia, nro_bloque);
		string_append(&keyCopia, "COPIA");
		numeroCopia = string_itoa(i);
		string_append(&keyCopia, numeroCopia);
	}

	free(numeroCopia);

	if (noPuedoBorar) {
		printf(
				"rm -b: no se puede borrar el bloque: %s copia: %s del archivo «./%s»: El bloque es el ultimo del sistema \n",
				nro_bloque, nro_copia, path_archivo);

		//Libero memoria
		free(path_archivo);
		free(nro_bloque);
		free(nro_copia);
		destruirSubstring(separado);
		free(rutaFS);
		free(indexPadreChar);
		config_destroy(configArchivo);
		free(key);
		free(keyCopia);

		return;
	}

	//Libero el bloque de la tabla de nodos
	char ** valor = config_get_array_value(configArchivo, key);
	liberarBloqueTablaNodos(valor[0], atoi(valor[1]));

	//Libero el bloque de la tabla de archivo
	char * nuevoValor = string_new();
	string_append(&nuevoValor, "###");
	config_set_value(configArchivo, key, nuevoValor);
	config_save(configArchivo);

	//Actualizo el valor total de bloques
	int totalesAnterior = config_get_int_value(configArchivo, "CANTIDAD_BLOQUES");
	totalesAnterior --;
	char * totalesActualesChar = string_itoa(totalesAnterior);
	config_set_value(configArchivo, "CANTIDAD_BLOQUES", totalesActualesChar);


	//Libero memoria
	free(path_archivo);
	free(nro_bloque);
	free(nro_copia);
	free(rutaFS);
	free(indexPadreChar);
	destruirSubstring(separado);
	destruirSubstring(valor);
	free(nuevoValor);
	config_destroy(configArchivo);
	free(key);
	free(keyCopia);
	free(totalesActualesChar);
}

void modificar(char * linea) {
	char * path_original = obtenerParametro(linea, 1);

	if (path_original == NULL)
		return;

	char * path_final = obtenerParametro(linea, 2);

	if (path_final == NULL) {
		free(path_original);
		return;
	}

	//Busco el nombre del directorio original
	char ** separadoOriginal = string_split(path_original, "/");

	int posicionOriginal;

	for (posicionOriginal = 0; separadoOriginal[posicionOriginal] != NULL;
			++posicionOriginal) {
	}

	posicionOriginal -= 1;

	//Busco el nombre del directorio final
	char ** separadoFinal = string_split(path_final, "/");

	int posicionFinal;

	for (posicionFinal = 0; separadoFinal[posicionFinal] != NULL;
			++posicionFinal) {
	}

	posicionFinal -= 1;

	//Busco lo que voy a modificar
	int indexPadre;

	if (posicionOriginal == 0) {
		indexPadre = obtenerIndex("root");
	} else {
		indexPadre = obtenerIndex(separadoOriginal[posicionOriginal - 1]);
	}

	bool esRegistroBuscado(t_directory * registro) {
		return string_equals_ignore_case(registro->nombre,
				separadoOriginal[posicionOriginal])
				&& registro->padre == indexPadre;
	}

	t_directory * registroDirectorio = list_find(tablaDirectorios,
			(void*) esRegistroBuscado);

	if (registroDirectorio == NULL) {
		modificarArchivo(separadoOriginal, separadoFinal, indexPadre,
				posicionOriginal, posicionFinal, path_original);
		free(path_final);
		return;
	}

	//Verifico si quiero renombrar o mover
	char * nuevoNombre = string_new();
	int nuevoIndex = registroDirectorio->padre;

	//Busco el index del padre del nombre final
	int indexPadreFinal;

	if (posicionFinal == 0) {
		indexPadreFinal = obtenerIndex("root");
	} else {
		indexPadreFinal = obtenerIndex(separadoFinal[posicionFinal - 1]);
	}

	//Busco los hijos del directorio
	bool soyDirectorio(t_directory * registro) {
		return string_equals_ignore_case(registro->nombre,
				separadoFinal[posicionFinal])
				&& (registro->padre == indexPadreFinal);
	}

	if (list_any_satisfy(tablaDirectorios, (void*) soyDirectorio)) {
		//Quiero mover un archivo
		if (posicionFinal == 0) {
			nuevoIndex = obtenerIndex("root");
		} else {
			nuevoIndex = obtenerIndex(separadoFinal[posicionFinal]);
		}
		string_append(&nuevoNombre, registroDirectorio->nombre);
	} else {
		//Quiero renombrar un archivo
		string_append(&nuevoNombre, separadoFinal[posicionFinal]);
	}

	modificarDirectorioTabla(registroDirectorio, nuevoNombre, nuevoIndex);

	//Libero memoria
	free(path_original);
	free(path_final);
	free(nuevoNombre);
	destruirSubstring(separadoOriginal);
	destruirSubstring(separadoFinal);
}

void mostrarContenidoArchivo(char * linea) {
	char * path_archivo = obtenerParametro(linea, 1);

	if (path_archivo == NULL)
		return;

	char* archivoTemporal = leerArchivo(path_archivo);

	if (archivoTemporal == NULL) {
		printf("%s: No existe el archivo o el directorio", path_archivo);
		free(path_archivo);
		return;
	}

	printf("%s \n", archivoTemporal);

	//Libero memoria
	free(path_archivo);
	free(archivoTemporal);
}

void crearDirectorio(char * linea) {
	char * path_dir = obtenerParametro(linea, 1);

	if (path_dir == NULL)
		return;

	//Busco el nombre del directorio
	char ** separado = string_split(path_dir, "/");

	int posicion;

	for (posicion = 0; separado[posicion] != NULL; ++posicion) {
	}

	posicion -= 1;

	t_directory * registro = malloc(sizeof(t_directory));

	strncpy(registro->nombre, separado[posicion], sizeof(registro->nombre) - 1);

	int indexPadre;

	if (posicion == 0) {
		indexPadre = obtenerIndex("root");
	} else {
		indexPadre = obtenerIndex(separado[posicion - 1]);
	}

	if (indexPadre == -1) {
		printf(
				"mkdir: no se puede crear el directorio «./%s»: No existe el archivo o el directorio\n",
				path_dir);
		free(path_dir);
		destruirSubstring(separado);
		free(registro);
		return;
	}

	registro->padre = indexPadre;

	if (verificarDuplicados(registro)) {
		printf(
				"mkdir: no se puede crear el directorio «./%s»: El archivo ya existe \n",
				path_dir);
		free(path_dir);
		destruirSubstring(separado);
		free(registro);
		return;
	}

	agregarDirectorioTabla(registro, path_dir);

//Libero memoria
	free(path_dir);
	destruirSubstring(separado);
}

void copiarArchivoLocalAlYamafsInterfaz(char * linea) {
	char * path_archivo_origen = obtenerParametro(linea, 1);

	if (path_archivo_origen == NULL)
		return;

	char * directorio_yamafs = obtenerParametro(linea, 2);

	if (directorio_yamafs == NULL) {
		free(path_archivo_origen);
		return;
	}

	char * tipo_archivo = obtenerParametro(linea, 3);

	if (tipo_archivo == NULL) {
		free(path_archivo_origen);
		free(directorio_yamafs);
		return;
	}

	//Busco el nombre del archivo
	char ** separado = string_split(path_archivo_origen, "/");

	int posicion;

	for (posicion = 0; separado[posicion] != NULL; ++posicion) {
	}

	posicion -= 1;

	//Seteo el tipo del archivo
	int tipoArchivoNumero = TEXTO;

	if (string_equals_ignore_case(tipo_archivo, "BINARIO"))
		tipoArchivoNumero = BINARIO;

	almacenarArchivo(path_archivo_origen, directorio_yamafs, separado[posicion],
			tipoArchivoNumero);

	//Libero memoria
	free(path_archivo_origen);
	free(directorio_yamafs);
	free(tipo_archivo);
	destruirSubstring(separado);
}

void copiarArchivoYamafsALocal(char * linea) {
	char * path_archivo_origen = obtenerParametro(linea, 1);

	if (path_archivo_origen == NULL)
		return;

	char * directorio_filesystem = obtenerParametro(linea, 2);

	if (directorio_filesystem == NULL) {
		free(path_archivo_origen);
		return;
	}

	//Busco el nombre del archivo
	char ** separado = string_split(path_archivo_origen, "/");

	int posicion;

	for (posicion = 0; separado[posicion] != NULL; ++posicion) {
	}

	posicion -= 1;

	//Reconstruyo el archivo que me piden
	char* archivoTemporal = leerArchivo(path_archivo_origen);

	if (archivoTemporal == NULL) {
		printf("%s: No existe el archivo o el directorio", path_archivo_origen);
		return;
	}

	//Creo el archivo temporal
	string_append(&directorio_filesystem, "/");
	string_append(&directorio_filesystem, separado[posicion]);

	FILE* file = fopen(directorio_filesystem, "w+b");

	fwrite(archivoTemporal, strlen(archivoTemporal), 1, file);

	fclose(file);

	//Libero memoria
	free(path_archivo_origen);
	free(directorio_filesystem);
	free(archivoTemporal);
	destruirSubstring(separado);
}

void crearCopiaBloqueEnNodo(char * linea) {
	char * rutaArchivo = obtenerParametro(linea, 1);

	if (rutaArchivo == NULL)
		return;

	char * numeroBloqueArchivo = obtenerParametro(linea, 2);

	if (rutaArchivo == NULL) {
		free(rutaArchivo);
		return;
	}

	char * nodoAGuardar = obtenerParametro(linea, 3);

	if (nodoAGuardar == NULL) {
		free(rutaArchivo);
		free(numeroBloqueArchivo);
		return;
	}

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

	//Abro el archivo de config
	char * rutaFS = string_new();
	string_append(&rutaFS, RUTA_METADATA);
	string_append(&rutaFS, "metadata/archivos/");
	char * indexChar = string_itoa(indexPadre);
	string_append(&rutaFS, indexChar);
	string_append(&rutaFS, "/");
	string_append(&rutaFS, separado[posicion]);

	t_config * configArchivo = config_create(rutaFS);

	//Busco el bloque a copiar
	int i = 0;
	char ** bloqueBuscado = buscarBloque(configArchivo,
			atoi(numeroBloqueArchivo), i);
	while (string_equals_ignore_case(bloqueBuscado[0], "#")) {
		i++;
		destruirSubstring(bloqueBuscado);
		bloqueBuscado = buscarBloque(configArchivo, atoi(numeroBloqueArchivo),
				i);
	}

	//Pido la info del bloque buscado
	int socket = buscarSocketPorNombre(bloqueBuscado[0]);
	enviarSolicitudLecturaBloqueGenerarCopia(socket, atoi(bloqueBuscado[1]),
			rutaArchivo, atoi(numeroBloqueArchivo), nodoAGuardar);

	//Libero memoria
	free(rutaArchivo);
	free(numeroBloqueArchivo);
	free(nodoAGuardar);
	destruirSubstring(separado);
	free(rutaFS);
	free(indexChar);
	config_destroy(configArchivo);
	destruirSubstring(bloqueBuscado);
}

void solicitarHash(char * linea) {
	char * path_archivo_yamafs = obtenerParametro(linea, 1);

	if (path_archivo_yamafs == NULL)
		return;

	//Busco el nombre del directorio
	char ** separado = string_split(path_archivo_yamafs, "/");

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

	//Abro el archivo de config
	char * ruta = string_new();
	string_append(&ruta, RUTA_METADATA);
	string_append(&ruta, "metadata/archivos/");
	char * indexChar = string_itoa(indexPadre);
	string_append(&ruta, indexChar);
	string_append(&ruta, "/");
	string_append(&ruta, separado[posicion]);

	t_config * configArchivo = config_create(ruta);

	if (configArchivo == NULL) {
		printf("%s: No existe el archivo o el directorio", path_archivo_yamafs);

		free(path_archivo_yamafs);
		destruirSubstring(separado);
		free(ruta);
		free(indexChar);
		return;
	}

	int tamArchivo = config_get_int_value(configArchivo, "TAMANIO");

	//Reconstruyo el archivo que me piden
	char* archivoTemporal = leerArchivo(path_archivo_yamafs);

	if (archivoTemporal == NULL) {
		printf("%s: No existe el archivo o el directorio", path_archivo_yamafs);

		free(path_archivo_yamafs);
		destruirSubstring(separado);
		free(ruta);
		free(indexChar);
		config_destroy(configArchivo);
		return;
	}

	//Creo la carpeta temporal
	char * rutaFS = string_new();
	string_append(&rutaFS, RUTA_METADATA);
	string_append(&rutaFS, "metadata/temporales/");

	mkdir(rutaFS, 0777);

	//Creo el archivo temporal
	string_append(&rutaFS, "hash");
	FILE* file = fopen(rutaFS, "w+b");

	fwrite(archivoTemporal, tamArchivo, 1, file);

	fclose(file);

	//Pido el hash del archivo
	char * comando = string_new();
	string_append(&comando, "md5sum ");
	string_append(&comando, rutaFS);

	system(comando);

	//Borro el archivo hash
	remove(rutaFS);

	//Borro la carpeta temporales
	char * rutaTemporal = string_new();
	string_append(&rutaTemporal, RUTA_METADATA);
	string_append(&rutaTemporal, "metadata/temporales");
	remove(rutaTemporal);

	//Libero memoria
	free(path_archivo_yamafs);
	free(rutaTemporal);
	free(rutaFS);
	free(comando);
	free(archivoTemporal);
	config_destroy(configArchivo);
	destruirSubstring(separado);
	free(ruta);
	free(indexChar);
}

void listarArchivos(char * linea) {
	char * path_directorio = obtenerParametro(linea, 1);

	if (path_directorio == NULL)
		return;

	//Busco el nombre del directorio
	char ** separado = string_split(path_directorio, "/");

	int posicion;

	for (posicion = 0; separado[posicion] != NULL; ++posicion) {
	}

	posicion -= 1;

	//Busco el index del padre del nombre final
	int indexPadre;

	if (posicion == 0) {
		indexPadre = obtenerIndex("root");
	} else {
		indexPadre = obtenerIndex(separado[posicion - 1]);
	}

	//Busco los hijos del directorio
	bool esRegistroBuscado(t_directory * registro) {
		return string_equals_ignore_case(registro->nombre, separado[posicion])
				&& registro->padre == indexPadre;
	}

	t_directory * registroDirectorio = list_find(tablaDirectorios,
			(void*) esRegistroBuscado);

	//Busco los directorios hijos del direcorio
	int i;
	int cantidadMaximaDirectorios = config_keys_amount(configTablaDirectorios);
	for (i = 0; i < cantidadMaximaDirectorios; ++i) {
		char * iChar = string_itoa(i);
		char * directorioPrueba = config_get_string_value(
				configTablaDirectorios, iChar);
		if (!string_equals_ignore_case(directorioPrueba, "[###]")) {
			char ** directorioAMostrar = config_get_array_value(
					configTablaDirectorios, iChar);
			if (registroDirectorio->index == atoi(directorioAMostrar[1]))
				printf("%s/\n", directorioAMostrar[0]);
			free(iChar);
			destruirSubstring(directorioAMostrar);
		}
	}

	//Busco los archivos que contenga el directorio
	char * rutaFS = string_new();
	string_append(&rutaFS, RUTA_METADATA);
	string_append(&rutaFS, "metadata/archivos/");
	char * indexChar = string_itoa(registroDirectorio->index);
	string_append(&rutaFS, indexChar);
	listarArchivosDirectorios(rutaFS);

	//Libero memoria
	free(path_directorio);
	destruirSubstring(separado);
	free(rutaFS);
	free(indexChar);
}

void mostrarInfo(char * linea) {
	char * path_archivo = obtenerParametro(linea, 1);

	if (path_archivo == NULL)
		return;

	//Busco el nombre del directorio
	char ** separado = string_split(path_archivo, "/");

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

	//Abro el archivo de config
	char * rutaFS = string_new();
	string_append(&rutaFS, RUTA_METADATA);
	string_append(&rutaFS, "metadata/archivos/");
	char * indexChar = string_itoa(indexPadre);
	string_append(&rutaFS, indexChar);
	string_append(&rutaFS, "/");
	string_append(&rutaFS, separado[posicion]);

	t_config * configArchivo = config_create(rutaFS);

	if (configArchivo == NULL) {
		printf("%s: No existe el archivo o el directorio", path_archivo);

		free(path_archivo);
		destruirSubstring(separado);
		free(rutaFS);
		free(indexChar);
		return;
	}

	//Imprimo por pantalla el archivo
	char * tamanio = config_get_string_value(configArchivo, "TAMANIO");
	printf("TAMANIO = %s\n", tamanio);
	char * tipo = config_get_string_value(configArchivo, "TIPO");
	printf("TIPO = %s\n", tipo);

	int i;
	int bloque = 0;
	int copia = 0;
	int cantidadBloque = ((config_keys_amount(configArchivo) - 2) / 3) * 2;
	for (i = 0; i < cantidadBloque; ++i) {
		imprimirBloque(configArchivo, bloque, copia);
		if (copia == 0) {
			copia++;
		} else {
			char * bloqueBytes = string_new();
			string_append(&bloqueBytes, "BLOQUE");
			char * numeroBloqueChar = string_itoa(bloque);
			string_append(&bloqueBytes, numeroBloqueChar);
			string_append(&bloqueBytes, "BYTES");

			char * valor = config_get_string_value(configArchivo, bloqueBytes);

			printf("%s = %s \n", bloqueBytes, valor);

			free(bloqueBytes);
			free(numeroBloqueChar);
			copia = 0;
			bloque++;
		}
	}

	//Libero memoria
	free(path_archivo);
	destruirSubstring(separado);
	free(rutaFS);
	free(indexChar);
	config_destroy(configArchivo);
}

/*------------------------------Auxiliares------------------------------*/
char * obtenerParametro(char * linea, int parametro) {
	char** substrings = string_split(linea, " ");

	if (substrings[parametro] == NULL) {
		destruirSubstring(substrings);
		printf("falta un operando\n");
		return NULL;
	}

	int tamPath = string_length(substrings[parametro]);

	char * path = malloc(tamPath + 1);

	memcpy(path, substrings[parametro], tamPath);

	destruirSubstring(substrings);

	path[tamPath] = '\0';

	return path;
}

bool verificarDuplicados(t_directory * duplicado) {
	bool esDulpicadoBuscado(t_directory * registro) {
		return string_equals_ignore_case(registro->nombre, duplicado->nombre)
				&& (registro->padre == duplicado->padre);
	}

	return list_any_satisfy(tablaDirectorios, (void*) esDulpicadoBuscado);
}

char ** buscarBloqueABorar(int posicion, int * numeroCopia, int * numeroBloque,
		t_config * configArchivo) {
	char * key = string_new();
	string_append(&key, "BLOQUE");
	char * numeoBloqueChar = string_itoa(*numeroBloque);
	string_append(&key, numeoBloqueChar);
	string_append(&key, "COPIA");
	char * numeroCopiaChar = string_itoa(*numeroCopia);
	string_append(&key, numeroCopiaChar);

	char ** nodoBloqueABorar = config_get_array_value(configArchivo, key);

	if (*numeroCopia == 0) {
		*numeroCopia = *numeroCopia + 1;
	} else {
		*numeroCopia = 0;
		*numeroBloque = *numeroBloque + 1;
	}

	//Libero memoria
	free(key);
	free(numeoBloqueChar);
	free(numeroCopiaChar);

	return nodoBloqueABorar;
}

int cantArchivosEnDirectorio(char * ruta) {
	DIR *dir;
	struct dirent *ent;

	dir = opendir(ruta);

	int i = 0;

	while ((ent = readdir(dir)) != NULL) {
		if (ent->d_name[0] != '.') {
			i++;
		}
	}

	closedir(dir);

	return i;
}

void listarArchivosDirectorios(char * ruta) {
	DIR *dir;
	struct dirent *ent;

	dir = opendir(ruta);

	if (dir == NULL)
		return;

	while ((ent = readdir(dir)) != NULL) {
		if (ent->d_name[0] != '.') {
			printf("%s \n", ent->d_name);
		}
	}

	closedir(dir);
}

void modificarArchivo(char ** separadoOriginal, char ** separadoFinal,
		int indexPadre, int posicionOriginal, int posicionFinal,
		char * path_original) {
	//Verifico que el archivo que voy a modificar exista
	char * rutaFS = string_new();
	string_append(&rutaFS, RUTA_METADATA);
	string_append(&rutaFS, "metadata/archivos/");
	char * indexPadreChar = string_itoa(indexPadre);
	string_append(&rutaFS, indexPadreChar);
	string_append(&rutaFS, "/");
	string_append(&rutaFS, separadoOriginal[posicionOriginal]);

	t_config * configArchivo = config_create(rutaFS);

	if (configArchivo == NULL) {
		printf("El archivo/directorio «./%s» no existe \n", path_original);

		//Libero memoria
		destruirSubstring(separadoOriginal);
		destruirSubstring(separadoFinal);
		free(path_original);
		free(rutaFS);
		free(indexPadreChar);

		return;
	}

	//Verifico si quiero renombrar o mover

	//Busco el index del padre del nombre final
	int indexPadreFinal;

	if (posicionFinal == 0) {
		indexPadreFinal = obtenerIndex("root");
	} else {
		indexPadreFinal = obtenerIndex(separadoFinal[posicionFinal - 1]);
	}

	//Busco los hijos del directorio
	bool esRegistroBuscado(t_directory * registro) {
		return string_equals_ignore_case(registro->nombre,
				separadoFinal[posicionFinal])
				&& registro->padre == indexPadreFinal;
	}

	t_directory * registroDirectorio = list_find(tablaDirectorios,
			(void*) esRegistroBuscado);

	char * nuevaRutaFS = string_new();
	string_append(&rutaFS, RUTA_METADATA);
	string_append(&nuevaRutaFS, "metadata/archivos/");

	if (registroDirectorio != NULL) {
		//Quiero mover un archivo
		char * indexPadreNuevoChar = string_itoa(registroDirectorio->index);
		string_append(&nuevaRutaFS, indexPadreNuevoChar);

		mkdir(nuevaRutaFS, 0777);

		string_append(&nuevaRutaFS, "/");
		string_append(&nuevaRutaFS, separadoOriginal[posicionOriginal]);

		free(indexPadreNuevoChar);
	} else {
		//Quiero renombrar un archivo
		string_append(&nuevaRutaFS, indexPadreChar);
		string_append(&nuevaRutaFS, "/");
		string_append(&nuevaRutaFS, separadoFinal[posicionFinal]);
	}

	config_save_in_file(configArchivo, nuevaRutaFS);

	remove(rutaFS);

	//Libero memoria
	destruirSubstring(separadoOriginal);
	destruirSubstring(separadoFinal);
	free(path_original);
	free(rutaFS);
	free(indexPadreChar);
	config_destroy(configArchivo);
	free(nuevaRutaFS);
}

void imprimirBloque(t_config * configArchivo, int numeroBloque, int numeroCopia) {
	char * key = string_new();
	string_append(&key, "BLOQUE");
	char * numeoBloqueChar = string_itoa(numeroBloque);
	string_append(&key, numeoBloqueChar);
	string_append(&key, "COPIA");
	char * numeroCopiaChar = string_itoa(numeroCopia);
	string_append(&key, numeroCopiaChar);

	char * nodoBloqueABorar = config_get_string_value(configArchivo, key);

	printf("%s = %s\n", key, nodoBloqueABorar);

	//Libero memoria
	free(key);
	free(numeoBloqueChar);
	free(numeroCopiaChar);
}
