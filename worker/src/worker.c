/*
 ============================================================================
 Name        : worker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Proceso workers
 ============================================================================
 */

#include "worker.h"

t_log* logger;
t_config *conf;

//Datos para reduccion global
t_list* paquetesEsclavos;
int cantidadWorker;
FILE* local;
char* rutaFinalGlobal;
char* rutaScriptReduccion;
paquete_esclavo* aux;
int socketMaster;
int encargado = 0;

//Datos para almacenado final
char* RUTA_DATABIN;
char* IP_FILESYSTEM;
char* PUERTO_FILESYSTEM;

int main(void) {

	conf = config_create(PATHCONFIG);
	logger = log_create(PATHLOG, "Worker", 1, LOG_LEVEL_INFO);

	//LEER ARCHIVO DE CONFIGURACION ---------------------------------------------------------

	//int NOMBRE_NODO = config_get_string_value(conf,"NOMBRE_NODO");
	char* PUERTO_WORKER = config_get_string_value(conf, "PUERTO_WORKER");
	RUTA_DATABIN = config_get_string_value(conf, "RUTA_DATABIN");
	IP_FILESYSTEM = config_get_string_value(conf, "IP_FILESYSTEM"); // traigo los datos del archivo nodo.cfg
	PUERTO_FILESYSTEM = config_get_string_value(conf, "PUERTO_FILESYSTEM");

	signal(SIGFPE,signal_capturer);
	signal(SIGSEGV,signal_capturer);
	signal(16,signal_capturer);

	iniciarServer(PUERTO_WORKER, (void *) procesarPaquete);

	config_destroy(conf);
	log_destroy(logger);
	return EXIT_SUCCESS;
}

void procesarPaquete(t_paquete * unPaquete, int * client_socket) {
	//log_info(logger, "Se intenta conectar Master");
	int pid = 0;
	socketMaster = *client_socket;
	t_pedidoTransformacion *auxTransf;
	t_pedidoReduccionLocal *auxRedL;
	t_pedidoReduccionGlobal *auxRedG;
	t_pedidoAlmacenadoFinal *auxPedF;
	paquete_esclavo* loc;
	switch (unPaquete->codigoOperacion) {
	case HANDSHAKE:
		recibirHandshakeLocal(unPaquete, client_socket);
		log_info(logger, "Saludo de Master recibido");
		break;
	case ENVIAR_SOLICITUD_TRANSFORMACION:
		pid = fork();
		if (pid < 0) {
			//enviarError(*client_socket, ERROR_TRANSFORMACION);
			log_error(logger,
					"No pudo crearse proceso hijo para antender operacion");
			_Exit(EXIT_FAILURE);
		}
		if (pid == 0) {
			auxTransf = recibirSolicitudTransformacion(unPaquete);
			log_info(logger, "Se inicia etapa de transformacion");
			transformacion(auxTransf->numBloque, auxTransf->cantBytes,
					auxTransf->rutaArchivoTemporal,
					auxTransf->rutaScriptTransformacion);
			log_info(logger, "Transformacion exitosa");
			enviarTareaCompletada(*client_socket, 1);
			_Exit(EXIT_SUCCESS);
		}
		break;
	case ENVIAR_SOLICITUD_REDUCCION_LOCAL:
		pid = fork();
		auxRedL = recibirSolicitudReduccionLocal(unPaquete);
		rutaScriptReduccion = auxRedL->rutaScript;
		if (pid < 0) {
			enviarError(*client_socket, ERROR_REDUCCION_LOCAL);
			log_error(logger,
					"No pudo crearse proceso hijo para antender operacion");
			_Exit(EXIT_FAILURE);
		}
		if (pid == 0) {
			log_info(logger, "Se inicia etapa de reduccion local");
			char** rutas = string_split(auxRedL->archivoTransformacion, "#");
			int i = 0;
			while (rutas[i] != NULL)
				i++;
			reduccionLocal(rutas, i, auxRedL->archivoReduccionLocal,
					auxRedL->rutaScript);
			log_info(logger, "Reduccion local exitosa");
			enviarTareaCompletada(*client_socket, 1);
			_Exit(EXIT_SUCCESS);
		}
		break;
	case ENVIAR_SOLICITUD_REDUCCION_GLOBAL:
		pid = fork();
		if (pid < 0) {
			enviarError(*client_socket, ERROR_REDUCCION_GLOBAL);
			log_error(logger,
					"No pudo crearse proceso hijo para antender operacion");
			_Exit(EXIT_FAILURE);
		}
		if (pid == 0) {
			auxRedG = recibirSolicitudReduccionGlobal(unPaquete);
			if (auxRedG->workerEncargado) {
				loc = (paquete_esclavo*) malloc(sizeof(paquete_esclavo));
				log_info(logger, "Se inicia etapa de reduccion global");
				paquetesEsclavos = list_create();
				local = fopen(auxRedG->archivoReduccionPorWorker, "r");
				if (local == NULL) {
					log_error(logger,
							"El archivo de reduccion local no pudo ser abiero");
					enviarError(*client_socket, ERROR_REDUCCION_GLOBAL);
					_Exit(EXIT_FAILURE);
				}
				rutaFinalGlobal = auxRedG->ArchivoResultadoReduccionGlobal;
				loc->palabra = calloc(TAM_MAX, sizeof(char));
				fgets(loc->palabra, TAM_MAX, local);
				loc->socket_cliente = -1;
				list_add(paquetesEsclavos, loc);
				cantidadWorker = auxRedG->cantWorkerInvolucradros;
				iniciarEncargado();
			} else {
				local = fopen(auxRedG->archivoReduccionPorWorker, "r");
				iniciarEsclavo(auxRedG->ip, auxRedG->puerto);
			}
		}
		break;
	case ENVIAR_SOLICITUD_ALMACENADO_FINAL:
		pid = fork();
		if (pid == 0) {
			int s = conectarCliente(IP_FILESYSTEM, PUERTO_FILESYSTEM, WORKER);
			auxPedF = recibirSolicitudAlmacenadoFinal(unPaquete);
			enviarRutaArchivoRutaDestino(s, auxPedF->archivoReduccionGlobal, auxPedF->rutaAlmacenadoFinal);
			enviarTareaCompletada(*client_socket, 1);
		}
		break;
	default:
		break;
	}
	destruirPaquete(unPaquete);
}

void recibirHandshakeLocal(t_paquete * unPaquete, int * client_socket) {
	int tipoCliente;
	memcpy(&tipoCliente, unPaquete->buffer->data, sizeof(int));
	switch (tipoCliente) {
	case WORKER:
	case MASTER:
		break;
	default:
		*client_socket = -1;
		break;
	}
}

void signal_capturer(int numeroSenial){

	switch (numeroSenial)
	{

		case 8:
			enviarError(socketMaster,ERROR_MASTER);
			log_error(logger, "Error de coma flotante");
			_Exit(EXIT_FAILURE);
			break;
		case 11:
			enviarError(socketMaster,ERROR_MASTER);
			log_error(logger, "Se cierra por violacion al segmento");
			_Exit(EXIT_FAILURE);
			break;
		case 16:
			enviarError(socketMaster,ERROR_MASTER);
			log_error(logger, "Se cierra por desbordamiento de pila");
			_Exit(EXIT_FAILURE);
			break;
		default:
			enviarError(socketMaster,ERROR_MASTER);
			log_error(logger, "PROCESO WORKER CIERRA POR NUMERO DE SEÑAL %d", numeroSenial);
			_Exit(EXIT_FAILURE);
			break;

	}

	return;
}

char* getBloque(int numBloque) {
	struct stat sb;
	char *map;
	char *bloque = malloc(TAM_BLOQUE);

	int fd = open(RUTA_DATABIN, O_RDONLY); //abrir archivo data.bin
	fstat(fd, &sb);
	map = mmap(NULL, //donde comienza a guardar el mapeo, NULL significa "donde quiera el S.O"
			sb.st_size, //el tamaño del file
			PROT_READ, //proteccion del file (PROT_READ = solo lectura)
			MAP_SHARED, //que comparta el mapeo con otros procesos creo, no se bien que hace
			fd, //el file descriptor
			0); //desde donde leer
	if (map == MAP_FAILED) {
		close(fd);
		log_error(logger, "Error al traer el bloque de data.bin");
		return NULL;
	}

	int i = numBloque * TAM_BLOQUE;

	memcpy(bloque, map + i, TAM_BLOQUE);

	munmap(map, sb.st_size); //cierro mmap()

	close(fd); //cierro archivo
	return bloque;
}


void transformacion(unsigned int bloque, unsigned int bytes, char* rutaArchivo,
		char* rutaScript) {

	char* traido = getBloque(bloque);

	traido = string_substring(traido, 0, bytes);

	//"echo hola pepe | ./script_transformacion.py > /tmp/resultado"

	char* comando = (char*) malloc(
			(strlen(rutaScript) + strlen(rutaArchivo) + strlen(traido) + 25)
					* sizeof(char));

	strcpy(comando, "cat ");
	strcat(comando, traido);
	strcat(comando, " | ./");
	strcat(comando, rutaScript);
	strcat(comando, " | sort > ");
	strcat(comando, rutaArchivo);

	system(comando);

	free(comando);
}

inline int sonTodosVerdaderos(int *fdt, int cant) {
	int i;
	for (i = 0; i < cant; i++) {
		if (fdt[i] == 0)
			return 0;
	}
	return 1;
}

void reduccionLocal(char** rutas, int cant, char* rutaFinal, char* rutaScript) {
	FILE** loc = (FILE**) calloc(cant, sizeof(FILE*));
	FILE* apareado = fopen(rutaFinal, "w");
	char* comando;
	if (apareado == NULL) {
		log_error(logger, "El archivo de salida no puede crearse");
		enviarError(socketMaster, ERROR_REDUCCION_LOCAL);
		_Exit(EXIT_FAILURE);
	}
	int i = 0, j = 0, esPrimero = 0;
	int* fdt = calloc(cant, sizeof(int)); //Para saber si ya llego a EOF. 0 para no, 1 para si
	char** leido = (char**) calloc(cant, sizeof(char*));
	char* ret;
	for (i = 0; i < cant; i++) {
		loc[i] = fopen(rutas[i], "r");
		if (loc[i] == NULL) {
			log_error(logger,
					"Uno o mas archivos transformados no pueden ser abiertos");
			fclose(apareado);
			enviarError(socketMaster, ERROR_REDUCCION_LOCAL);
			_Exit(EXIT_FAILURE);
		}
		leido[i] = (char*) calloc(TAM_MAX, sizeof(char));
		fgets(leido[i], TAM_MAX, loc[i]);
	}
	for (i = 0; !sonTodosVerdaderos(fdt, cant); i++) {
		if (fdt[i % cant] == 1)
			continue;
		for (j = 0; j < cant; j++) {
			if (strcmp(leido[i % cant], leido[j]) < 0 || j == (i % cant))
				esPrimero++;
		}
		if (esPrimero == cant) {
			fputs(leido[i % cant], apareado);
			ret = fgets(leido[i % cant], TAM_MAX, loc[i % cant]);
			if (ret == NULL) {
				fdt[i % cant] = 1;
				leido[i % cant][0] = 255; //Le asigno el ASCII mas grande para que pueda comparar el resto, sino compara el ultimo registro y tira bucle infinito
			}
		}
		esPrimero = 0;
	}

	comando = calloc((strlen(rutaScript) + strlen(rutaFinal) + 1),
			sizeof(char));

	strcpy(comando, rutaScript);
	strcat(comando, " ");
	strcat(comando, rutaFinal);
	system(comando);

	free(comando);

	for (i = 0; i < cant; i++) {
		fclose(loc[i]);
		free(leido[i]);
	}
	fclose(apareado);
	free(loc);
	free(fdt);
	free(leido);

}

void reduccionGlobal(void) {
	int i = 0, j = 0, esPrimero = 0;
	char* comando;
	paquete_esclavo *fijo, *compar;
	FILE* apareoGlobal = fopen(rutaFinalGlobal, "w");
	for (i = 0; cantidadWorker > 0; i++) {
		fijo = (paquete_esclavo*) list_get(paquetesEsclavos,
				i % cantidadWorker);
		for (j = 0; j < cantidadWorker; j++) {
			compar = (paquete_esclavo*) list_get(paquetesEsclavos, j);
			if (strcmp(fijo->palabra, compar->palabra) < 0
					|| j == (i % cantidadWorker))
				esPrimero++;
		}
		if (esPrimero == cantidadWorker) {
			fputs(fijo->palabra, apareoGlobal);
			if (fijo->socket_cliente == -1) {
				if (fgets(fijo->palabra, TAM_MAX, local) == NULL) {
					free(fijo->palabra);
					list_remove_and_destroy_element(paquetesEsclavos,
							i % cantidadWorker, (void*) free);
					cantidadWorker--;
				}
			} else {
				aux = fijo;
				enviarMensaje(fijo->socket_cliente, " ");
				gestionarSolicitudes(fijo->socket_cliente,
						(void*) recibirDatos);
				if (fijo->palabra[0] == '\0') {
					list_remove_and_destroy_element(paquetesEsclavos,
							i % cantidadWorker, (void*) free);
					cantidadWorker--;
				}
			}
		}
		esPrimero = 0;
	}
	fclose(apareoGlobal);

	comando = calloc(
			(strlen(rutaScriptReduccion) + strlen(rutaFinalGlobal) + 5),
			sizeof(char));
	strcpy(comando, rutaScriptReduccion);
	strcat(comando, " ");
	strcat(comando, rutaFinalGlobal);
	system(comando);
	free(comando);
}

void recibirDatos(t_paquete * unPaquete, int * client_socket) {
	paquete_esclavo* p;
	switch (unPaquete->codigoOperacion) {
	case HANDSHAKE:
		recibirHandshakeLocal(unPaquete, client_socket);
		if (*client_socket != -1)
			enviarHandshake(*client_socket, WORKER);
		break;
	case ENVIAR_MENSAJE:
		p = malloc(sizeof(paquete_esclavo));
		p->socket_cliente = *client_socket;
		p->palabra = recibirMensaje(unPaquete);
		list_add(paquetesEsclavos, p);
		if (paquetesEsclavos->elements_count == cantidadWorker)
		{
			reduccionGlobal();
			list_destroy(paquetesEsclavos);
			enviarTareaCompletada(socketMaster, 1);
			log_info(logger, "Se completo con exito la reduccion global");
			_Exit(EXIT_SUCCESS);
		}
		break;
	case ENVIAR_PALABRA:
		aux->palabra = recibirMensaje(unPaquete);
		break;
	default:
		break;
	}
	destruirPaquete(unPaquete);
}

void iniciarEncargado() {
	iniciarServer(PUERTO_REDUCCION_GLOBAL, (void*)recibirDatos);
}

void iniciarEsclavo(char* ip, char* puerto) {
	int s = conectarCliente(ip, PUERTO_REDUCCION_GLOBAL, WORKER);
	while (1) {
		gestionarSolicitudes(s, (void*) recibirPedido);
	}
}

void recibirPedido(t_paquete* unPaquete, int* socket_server) {
	char leido[TAM_MAX];
	switch (unPaquete->codigoOperacion) {
	case HANDSHAKE:
		fgets(leido, TAM_MAX, local);
		enviarMensaje(*socket_server, leido);
		break;
	case ENVIAR_MENSAJE:
		if (fgets(leido, TAM_MAX, local) != NULL)
			enviarPalabra(*socket_server, leido);
		else {
			leido[0] = '\0';
			enviarPalabra(*socket_server, leido);
			fclose(local);
			log_info(logger,
					"Se termino el envio del archivo para reduccion global");
			_Exit(EXIT_SUCCESS);
		}
		break;
	default:
		break;
	}
}
