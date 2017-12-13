#include "dataNode.h"

int main(int argc, char **argv) {

	//Creo archivo de log
	t_log_level logLevel = LOG_LEVEL_INFO; //elijo enum de log
	logger = log_create("dataNode.log", "dataNode", true, logLevel); //creo archivo log

	//Leer archivo de configuracion
	t_config * conf = config_create(PATHCONFIG); // path relativo al archivo nodo.cfg

	char * ipFS = config_get_string_value(conf, "IP_FILESYSTEM"); // traigo los datos del archivo nodo.cfg
	char * puertoFS = config_get_string_value(conf, "PUERTO_FILESYSTEM");
	nombreNodo = config_get_string_value(conf, "NOMBRE_NODO");
	rutaDatabin = config_get_string_value(conf, "RUTA_DATABIN");
	ipNodo = config_get_string_value(conf, "IP_NODO"); // traigo los datos del archivo nodo.cfg
	puertoNodo = config_get_string_value(conf, "PUERTO_DATANODE");

	//Me fijo cuantos bloques tengo
	struct stat statArch;

	stat(rutaDatabin, &statArch);

	cantidadBloques = statArch.st_size / TAM_BLOQUE;

	//Conectar a FS
	socketFileSystem = conectarCliente(ipFS, puertoFS, DATANODE);

	recibirSolicitudes = true;

	//Quedo a la espera de solicitudes
	while (recibirSolicitudes) {
		gestionarSolicitudes(socketFileSystem, (void*) recibirSolicitud);
	}

	//Libero memoria
	log_destroy(logger);
	config_destroy(conf);
	free(ipFS);
	free(puertoFS);
	free(nombreNodo);
	free(rutaDatabin);
	free(ipNodo);
	free(puertoNodo);

	return EXIT_SUCCESS;
}

char* getBloque(int numBloque) {
	struct stat sb;
	char *map;
	char *bloque = malloc(TAM_BLOQUE);

	int fd = open(rutaDatabin, O_RDONLY); //abrir archivo data.bin
	fstat(fd, &sb);
	map = mmap(NULL, //donde comienza a guardar el mapeo, NULL significa "donde quiera el S.O"
			sb.st_size, //el tamaño del file
			PROT_READ, //proteccion del file (PROT_READ = solo lectura)
			MAP_SHARED, //que comparta el mapeo con otros procesos creo, no se bien que hace
			fd, //el file descriptor
			0); //desde donde leer
	if (map == MAP_FAILED) {
		close(fd);
		perror("[-] Error mapeando el archivo");
		return NULL;
	}

	int i = numBloque * TAM_BLOQUE;

	memcpy(bloque, map + i, TAM_BLOQUE);

	if (munmap(map, sb.st_size) == -1) //cierro mmap()
			{
		perror("[-]Error cerrando map");
		return NULL;
	}
	close(fd); //cierro archivo
	return bloque;
}

int setBloque(int numBloque, char* bloque) {
	struct stat sb;
	char *map;
	int fd = open(rutaDatabin, O_RDWR); //abrir archivo data.bin
	fstat(fd, &sb);
	map = (char*) mmap(NULL, //donde comienza a guardar el mapeo, NULL significa "donde quiera el S.O"
			sb.st_size, //el tamaño del file
			PROT_READ | PROT_WRITE, //proteccion del file (PROT_READ = solo lectura)
			MAP_SHARED, //que comparta el mapeo con otros procesos creo, no se bien que hace
			fd, //el file descriptor
			0); //desde donde leer
	if (map == MAP_FAILED) {
		close(fd);
		perror("[-] Error mapeando el archivo");
		return -1;
	}
	int i = numBloque * TAM_BLOQUE;

	memcpy(map + i, bloque, TAM_BLOQUE);

	if (munmap(map, sb.st_size) == -1) //cierro mmap()
			{
		perror("[-]Error cerrando map");
		return -1;
	}
	close(fd); //cierro archivo
	return 0;
}

/*------------------------------Procesar paquetes------------------------------*/
void recibirSolicitud(t_paquete * unPaquete, int * client_socket) {

	switch (unPaquete->codigoOperacion) {
	case ENVIAR_SOLICITUD_LECTURA_BLOQUE:
		procesarSolicitudLecturaBloque(unPaquete, client_socket);
		break;

	case ENVIAR_SOLICITUD_ESCRITURA_BLOQUE:
		procesarSolicitudEscrituraBloque(unPaquete, client_socket);
		break;

	case ENVIAR_ERROR:
		procesarError(unPaquete);
		break;

	case ENVIAR_SOLICITUD_LECTURA_ARCHIVO_TEMPORAL:
		procesarSolicitudLecturaArchivoTemporal(unPaquete, client_socket);
		break;

	case ENVIAR_SOLICITUD_LECTURA_BLOQUE_GENERAR_COPIA:
		procesarSolicitudLecturaBloqueGenerarCopia(unPaquete, client_socket);
		break;
	case ENVIAR_SOLICITUD_INFO_DATANODE:
		procesarSolicitudInfoNodo(unPaquete, client_socket);
		break;
	case ENVIAR_SOLICITUD_NOMBRE:
		procesarSolicitudNombre(unPaquete, client_socket);
		break;
	default:
		break;
	}
	destruirPaquete(unPaquete);
}

void procesarSolicitudLecturaBloque(t_paquete * unPaquete, int * client_socket) {
	int numBloque = recibirSolicitudLecturaBloque(unPaquete);

	log_trace(logger,"Me llego una solicitud de lectura del bloque: %d",numBloque);

	char* bloque = malloc(TAM_BLOQUE);

	bloque = getBloque(numBloque);

	if (bloque == NULL) {
		log_error(logger, "Error buscando bloque");
	}

	enviarBloque(*client_socket, bloque);

	free(bloque);
}

void procesarSolicitudEscrituraBloque(t_paquete * unPaquete,
		int * client_socket) {
	t_pedidoEscritura * pedidoEscritura = recibirSolicitudEscrituraBloque(
			unPaquete);

	log_trace(logger,"Me llego una solicitud de escritura del bloque: %d",pedidoEscritura->bloqueAEscribir);

	bool exito = true;

	pedidoEscritura->buffer->data = realloc(pedidoEscritura->buffer->data,
	TAM_BLOQUE);

	if (setBloque(pedidoEscritura->bloqueAEscribir,
			pedidoEscritura->buffer->data) == -1) {
		log_error(logger, "Error guardando bloque");
		exito = false;
	}

	enviarRespuestaEscrituraBloque(*client_socket, exito,
			pedidoEscritura->bloqueAEscribir);

	free(pedidoEscritura->buffer->data);
	free(pedidoEscritura->buffer);
	free(pedidoEscritura);
}

void procesarError(t_paquete * unPaquete) {
	log_error(logger, "Me llego un error y dejo de recibir solicitudes \n");
	recibirSolicitudes = false;
}

void procesarSolicitudLecturaArchivoTemporal(t_paquete * unPaquete,
		int * client_socket) {

	t_lecturaArchTemp * lectura = recibirSolicitudLecturaBloqueArchTemp(
			unPaquete);

	log_trace(logger,"Me llego una solicitud de lectura del bloque: %d",lectura->numBloque);

	void * bloque = getBloque(lectura->numBloque);

	if (bloque == NULL) {
		log_error(logger, "Error buscando bloque");
	}

	enviarBloqueArchTemp(*client_socket, bloque, lectura->orden);

	free(bloque);
	free(lectura);
}

void procesarSolicitudLecturaBloqueGenerarCopia(t_paquete * unPaquete,
		int * client_socket) {
	t_lecturaGenerarCopia * lecturaGenerarCopia =
			recibirSolicitudLecturaBloqueGenerarCopia(unPaquete);

	log_trace(logger,"Me llego una solicitud de lectura del bloque: %d",lecturaGenerarCopia->numBloqueNodo);

	char* bloque = getBloque(lecturaGenerarCopia->numBloqueNodo);

	if (bloque == NULL) {
		log_error(logger, "Error buscando bloque");
	}

	enviarBloqueGenerarCopia(*client_socket, bloque,
			lecturaGenerarCopia->rutaArchivo,
			lecturaGenerarCopia->numBloqueArchivo,
			lecturaGenerarCopia->nomNodoAEscribir);

	free(bloque);
	free(lecturaGenerarCopia->nomNodoAEscribir);
	free(lecturaGenerarCopia->rutaArchivo);
	free(lecturaGenerarCopia);
}

void procesarSolicitudInfoNodo(t_paquete * unPaquete, int * client_socket) {
	log_trace(logger, "Me llego una solicitud de informacion \n");
	enviarInfoDataNode(*client_socket, nombreNodo, cantidadBloques, cantidadBloques);
}

void procesarSolicitudNombre(t_paquete * unPaquete, int * client_socket) {
	log_trace(logger, "Me llego una solicitud de nombre, ip y puerto \n");
	enviarNombre(*client_socket, nombreNodo, ipNodo, puertoNodo);
}
