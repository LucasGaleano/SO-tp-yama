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
FILE* apareoGlobal;
char* rutaFinalGlobal;
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
	paquete_esclavo* loc;
	switch (unPaquete->codigoOperacion) {
	case HANDSHAKE:
		recibirHandshake(unPaquete, client_socket);
		log_info(logger, "Saludo de Master recibido");
		break;
	case ENVIAR_INDICACION_TRANSFORMACION:
		pid = fork();
		if (pid < 0) {
			enviarError(*client_socket, ERROR_TRANSFORMACION);
			log_error(logger,
					"No pudo crearse proceso hijo para antender operacion");
		}
		if (pid == 0) {
			auxTransf = recibirSolicitudTransformacion(unPaquete);
			log_info(logger, "Se inicia etapa de transformacion");
			transformacion(auxTransf->bloque, auxTransf->rutaArchivoTemporal);
			log_info(logger, "Transformacion exitosa");
			enviarTareaCompletada(*client_socket, 1);
			_Exit(EXIT_SUCCESS);
		}
		break;
	case ENVIAR_INDICACION_REDUCCION_LOCAL:
		pid = fork();
		if (pid < 0) {
			enviarError(*client_socket, ERROR_REDUCCION_LOCAL);
			log_error(logger,
					"No pudo crearse proceso hijo para antender operacion");
		}
		if (pid == 0) {
			auxRedL = recibirSolicitudReduccionLocal(unPaquete);
			log_info(logger, "Se inicia etapa de reduccion local");
			char** rutas = string_split(auxRedL->archivoTransformacion, "#");
			int i = 0;
			while(rutas[i] != NULL)
				i++;
			reduccionLocal(rutas, i, auxRedL->archivoReduccionLocal);
			log_info(logger, "Reduccion local exitosa");
			enviarTareaCompletada(*client_socket, 1);
			_Exit(EXIT_SUCCESS);
		}
		break;
	case ENVIAR_INDICACION_REDUCCION_GLOBAL:
		auxRedG = recibirSolicitudReduccionGlobal(unPaquete);
		if (auxRedG->workerEncargdo) {
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
			fgets(loc->palabra, TAM_MAX, local);
			loc->socket_cliente = -1;
			list_add(paquetesEsclavos, loc);
			cantidadWorker = auxRedG->cantWorkerInvolucradros;
			rutaFinalGlobal = auxRedG->ArchivoResultadoReduccionGlobal;
			encargado = 1;
		} else {
			if (encargado) {
				loc = (paquete_esclavo*) malloc(sizeof(paquete_esclavo));
				loc->socket_cliente = conectarCliente(auxRedG->ip,
						auxRedG->puerto, WORKER);
				if (loc->socket_cliente == -1) {
					log_error(logger,
							"No se puede conectar con uno de los Worker para reduccion global");
					enviarError(*client_socket, ERROR_REDUCCION_GLOBAL);
					_Exit(EXIT_FAILURE);
				}
				enviarSolicitudReduccionGlobal(loc->socket_cliente, auxRedG);
				sleep(1);
				loc->socket_cliente = conectarCliente(auxRedG->ip,
				PUERTO_REDUCCION_GLOBAL, WORKER);
				list_add(paquetesEsclavos, loc);
			} else {
				pid = fork();
				if (pid < 0) {
					enviarError(*client_socket, ERROR_REDUCCION_GLOBAL);
					log_error(logger,
							"No pudo crearse proceso hijo para antender operacion");
				}
				if (pid == 0) {
					local = fopen(auxRedG->archivoReduccionPorWorker, "r");
					if (local == NULL) {
						log_error(logger,
								"El archivo de reduccion local no pudo ser abiero");
						enviarError(*client_socket, ERROR_REDUCCION_GLOBAL);
						_Exit(EXIT_FAILURE);
					}
					iniciarEsclavo(auxRedG->ip,
							auxRedG->archivoReduccionPorWorker);
				}
			}
		}
		if (paquetesEsclavos->elements_count == cantidadWorker) {
			pid = fork();
			if (pid < 0) {
				enviarError(*client_socket, ERROR_REDUCCION_GLOBAL);
				log_error(logger,
						"No pudo crearse proceso hijo para antender operacion");
			}
			if (pid == 0)
			{
				iniciarEncargado();
				enviarTareaCompletada(*client_socket, 1);
			}
			fclose(local);
			fclose(apareoGlobal);
			list_clean(paquetesEsclavos);
		}
		break;
	case ENVIAR_INDICACION_ALMACENADO_FINAL:
		pid = fork();
		if (pid == 0) {
			log_info(logger, "Se inicio el almacenado final");
			t_pedidoAlmacenadoFinal* algo =
					recibirSolicitudAlmacenadoFinal(unPaquete);
			int socketFS = conectarCliente(IP_FILESYSTEM, PUERTO_FILESYSTEM,
					WORKER);
			if (socketFS < 0) {
				log_error(logger, "No se pudo conectar con el File System");
				enviarError(*client_socket, ERROR_ALMACENAMIENTO_FINAL);
				_Exit(EXIT_FAILURE);
			}
			apareoGlobal = fopen(algo->rutaAlmacenadoFinal, "r");
			if (apareoGlobal == NULL) {
				log_error(logger,
						"No se pudo abrir el archivo de reduccion global");
				enviarError(*client_socket, ERROR_ALMACENAMIENTO_FINAL);
				_Exit(EXIT_FAILURE);
			}
			char leido[TAM_MAX];
			while (fgets(leido, TAM_MAX, apareoGlobal) != NULL) {
				enviarMensaje(socketFS, leido);
			}
			enviarTareaCompletada(*client_socket, 1);
		}
		break;
	default:
		break;
	}
	destruirPaquete(unPaquete);
}

void recibirHandshake(t_paquete * unPaquete, int * client_socket) {
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

void transformacion(unsigned int bloque, char* ruta) {

	//aplicar transformacion

	char* aux = "sort salidaDeTransformacion > ";

	char* comando = (char*) malloc((strlen(aux) + strlen(ruta)) * sizeof(char));

	strcat(comando, aux);
	strcat(comando, ruta);

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

void reduccionLocal(char** rutas, int cant, char* rutaFinal) {
	FILE** loc = (FILE**) calloc(cant, sizeof(FILE*));
	FILE* apareado = fopen(rutaFinal, "w");
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

	//Por aca va la reduccion

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
	paquete_esclavo *fijo, *compar;
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
					list_remove_and_destroy_element(paquetesEsclavos,
							i % cantidadWorker, (void*) free);
					cantidadWorker--;
				}
			} else {
				aux = fijo;
				enviarMensaje(aux->socket_cliente, " ");
				gestionarSolicitudes(fijo->socket_cliente,
						(void*) recibirSolicitud);
				if (aux->palabra[0] == '\0') {
					list_remove_and_destroy_element(paquetesEsclavos,
							i % cantidadWorker, (void*) free);
					cantidadWorker--;
				}
			}
		}
		esPrimero = 0;
	}
}

void recibirDatos(t_paquete * unPaquete, int * client_socket) {
	char leido[TAM_MAX];
	switch (unPaquete->codigoOperacion) {
	case HANDSHAKE:
		fgets(leido, TAM_MAX, local);
		enviarMensaje(*client_socket, leido);
		break;
	case ENVIAR_MENSAJE:
		if (fgets(leido, TAM_MAX, local) == NULL) {
			log_info(logger,
					"Se completo el envio para reduccion global a encargado");
			leido[0] = '\0';
			enviarMensaje(*client_socket, leido);
			fclose(local);
			_Exit(EXIT_SUCCESS);
		}
		enviarMensaje(*client_socket, leido);
		break;
	default:
		break;
	}
	destruirPaquete(unPaquete);
}

void iniciarEncargado() {
	int i = 0;
	for (i = 0; i < cantidadWorker; i++) {
		aux = (paquete_esclavo*) list_get(paquetesEsclavos, i);
		enviarHandshake(aux->socket_cliente, WORKER);
		gestionarSolicitudes(aux->socket_cliente, (void*) recibirSolicitud);
	}
	reduccionGlobal();
}

void iniciarEsclavo(char* ip, char* rutaLocal) {
	iniciarServer(PUERTO_REDUCCION_GLOBAL, (void*) recibirDatos);
}

void recibirSolicitud(t_paquete* unPaquete, int* socket_server) {
	switch (unPaquete->codigoOperacion) {
	case ENVIAR_MENSAJE:
		strcpy(aux->palabra, recibirMensaje(unPaquete));
		break;
	default:
		break;
	}
}
