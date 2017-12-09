#include "yama.h"

int main(void) {

	//Levanto el archivo de configuracion
	char* path_config_yama =
			"/home/utnso/workspace/tp-2017-2c-NULL/configuraciones/yama.cfg";

	configuracion = leerArchivoDeConfiguracionYAMA(path_config_yama);

	//Me conecto con el file system
	socketFS = conectarCliente(configuracion->ip, configuracion->puerto, YAMA); //todo VERIFICAR CONEXION CON FS ROMPER

	//Creo estructuras administrativas
	idJob = 0;
	cola_master = queue_create();
	masterConectados = list_create(); //TODO VER QUE ONDA COMO HAGO CON LOS MASTERS
	tabla_de_estados = list_create();

	//Creo el thread para escuchar conexiones
	pthread_t threadServerYama;

	if (pthread_create(&threadServerYama, NULL, (void*) iniciarServidor,
			configuracion->puerto_yama)) {

		perror("Error el crear el thread servidor.");
		exit(EXIT_FAILURE);
	}

	//iniciarPlanificador(config->algoritmo); todo

	destruirConfiguracion(configuracion);
//	list_destroy_and_destroy_elements(tabla_de_estados, (void*) eliminarElemento() ); //TODO PREGUNTAR SI ESTA BIEN LIBERAR DESPUES DE USAR

	return EXIT_SUCCESS;
}

/*------------------------Configuracion proyecto-------------------------*/
t_configuracion * leerArchivoDeConfiguracionYAMA(char* path) {

	t_config * config = config_create(path);

//Valido que el archivo de configuracion este con los datos correspondientes
	if (!config_has_property(config, "FS_IP")
			|| !config_has_property(config, "FS_PUERTO")
			|| !config_has_property(config, "RETARDO_PLANIFICACION")
			|| !config_has_property(config, "ALGORITMO_BALANCEO")) {
		printf(
				"ERROR: Alguna de las propiedades definidas en el archivo de configuracion es invalida o vacia \n");
		exit(1);
	}

	configuracion = malloc(sizeof(t_configuracion));

	configuracion->ip = config_get_string_value(config, "FS_IP");
	configuracion->puerto = config_get_string_value(config, "FS_PUERTO");
	configuracion->retardo = config_get_int_value(config,
			"RETARDO_PLANIFICACION");
	configuracion->algoritmo = config_get_string_value(config,
			"ALGORITMO_BALANCEO");
	configuracion->puerto_yama = config_get_string_value(config, "PUERTO_YAMA");
	configuracion->disponibilidad_base = config_get_int_value(config,
			"DISPONIBILIDAD_BASE");

	printf(
			"Se levanto el proceso YAMA con: YAMA_PUERTO: %s  FS_IP: %s - FS_PUERTO: %s - RETARDO: %d - ALGORITMO: %s - DISPONIBILIDAD BASE: %d \n",
			configuracion->puerto_yama, configuracion->ip,
			configuracion->puerto, configuracion->retardo,
			configuracion->algoritmo, configuracion->disponibilidad_base);

	config_destroy(config);

	return configuracion;
}

/*-------------------------Manejo de conexiones-------------------------*/
void iniciarServidor(char* unPuerto) {
	iniciarServer(unPuerto, (void *) procesarPaquete);
}

/*-------------------------Procesamiento paquetes-------------------------*/
void procesarPaquete(t_paquete * unPaquete, int * client_socket) {
	switch (unPaquete->codigoOperacion) {
	case HANDSHAKE:
		procesarRecibirHandshake(unPaquete, client_socket);
		break;
	case ENVIAR_MENSAJE:
		procesarRecibirMensaje(unPaquete);
		break;
	case ENVIAR_ARCHIVO:
		procesarRecibirArchivo(unPaquete);
		break;
	case ENVIAR_ERROR:
		procesarRecibirError(unPaquete);
		break;
	case ENVIAR_SOLICITUD_TRANSFORMACION:
		procesarEnviarSolicitudTransformacion(unPaquete, client_socket);
		break;
	case ENVIAR_INDICACION_TRANSFORMACION:
		procesarEnviarIndicacionTransformacion(unPaquete);
		break;
	case TAREA_COMPLETADA:
		procesarTareaCompleta(unPaquete, *client_socket);
		break;
	default:
		break;
	}
	destruirPaquete(unPaquete);
}

void procesarRecibirHandshake(t_paquete * unPaquete, int * client_socket) {
	int tipoCliente = recibirHandshake(unPaquete);

	switch (tipoCliente) {
	case MASTER:
		;
		int * a = malloc(sizeof(int));
		memcpy(a, client_socket, sizeof(int));
		list_add(masterConectados, a); // una lista de master conectados para distribuir job
		break;
	default:
		*client_socket = -1;
		break;
	}
}

void procesarRecibirMensaje(t_paquete * unPaquete) {
//char * mensaje = recibirMensaje(unPaquete); todo Comento para que no tire error
}

void procesarRecibirArchivo(t_paquete * unPaquete) {
//void * archivo = recibirArchivo(unPaquete);  todo Comento para que no tire error
}

void procesarRecibirError(t_paquete * unPaquete) {
//int error = recibirError(unPaquete);

//		char* prefijo = string_new();//prefijo del archivo temporal
//		string_append(prefijo, "reduccion:");
//		char* nombreTempReduccion = nombreArchivoTemp(prefijo);
//
//		PaqueRedLocal->nodo = malloc(sizeof(paqueteTransform->nodo));
//		PaqueRedLocal->ip = malloc(sizeof(paqueteTransform->ip));
//		PaqueRedLocal->puerto = malloc(sizeof(paqueteTransform->puerto));
//		PaqueRedLocal->archivoTemporalTransformacion = malloc(
//				sizeof(paqueteTransform->rutaArchivoTemporal));
//		PaqueRedLocal->archivoTemporalReduccionLocal = malloc(
//				sizeof(nombreTempReduccion));
//
//		enviarIndicacionReduccionLocal(client_socket, PaqueRedLocal);
//
//
//
//
//todo --> Si queres saber quien te envia el codigo de error lo haces con el client_socket de la funcion procesarPaquetes
//int cliente_desconectado;
//memcpy(&cliente_desconectado, unPaquete->buffer->data, sizeof(int));
//
//todo HACER ALGO ANTE EL ERROR
}

void procesarEnviarSolicitudTransformacion(t_paquete * unPaquete,
		int *client_socket) {
	queue_push(cola_master, client_socket);	// todo-->Verificar que esto haga lo que realmente quiero
	char * nomArchivo = recibirMensaje(unPaquete);
	enviarRutaArchivo(socketFS, nomArchivo);
}

void procesarEnviarIndicacionTransformacion(t_paquete * unPaquete) {
//int socket_master = queue_pop(cola_master);

//me llega la respuesta de FS y despues de procesarla la envio al master de la lista masterconectados.

	t_list* listaDeBloque = recibirListaDeBloques(unPaquete);//recibir lista de FS
}
/*-------------------------Funciones auxiliares-------------------------*/

t_indicacionTransformacion* bloqueAT_indicacionTranformacion(int tamanio,
		t_bloque_ubicacion* ubicacion, char* nombreTemp) {

	t_indicacionTransformacion* indTransform = malloc(
			sizeof(t_indicacionTransformacion));

//indTransform->bloque = malloc(ubicacion->numBloque);
//indTransform->bytes = malloc(tamanio);
	indTransform->ip = malloc(string_length(ubicacion->ip));
	indTransform->nodo = malloc(string_length(ubicacion->nodo));
	indTransform->puerto = malloc(string_length(ubicacion->puerto));
	indTransform->rutaArchivoTemporal = malloc(string_length(nombreTemp));

	indTransform->bloque = ubicacion->numBloque;
	indTransform->bytes = tamanio;
	indTransform->ip = ubicacion->ip;
	indTransform->nodo = ubicacion->nodo;
	indTransform->puerto = ubicacion->puerto;
	indTransform->rutaArchivoTemporal = nombreTemp;

	return indTransform;
}

void destruirIndicacionDeTransformacion(
		t_indicacionTransformacion* indTransform) {
	free(indTransform->nodo);
	free(indTransform->ip);
	free(indTransform->puerto);
	free(indTransform->rutaArchivoTemporal);

	free(indTransform);
}

//TODO funcion destruir el t_indicacionTranformacion

long generarJob() {
	idJob += 1;
	return idJob;
}

t_list* agruparBloquesPorNodo(t_list* listaDeNodoBloque) {
	t_list* nombreNodosSinRepetidos = list_create(); //TODO LIBERAR LISTA

	bool existeNodoEnLaLista(char* nombreNodo) {
		bool booleano = false;
		for (int x = 0; x < nombreNodosSinRepetidos->elements_count; x++) {
			if (string_equals_ignore_case(list_get(nombreNodosSinRepetidos, x),
					nombreNodo)) {
				booleano = true;
				break;
			}
		}
		return booleano;
	}

	void tomarNodosSinRepetidos(t_nodo_bloque* t_nodo_bloque) {
		if (!existeNodoEnLaLista(t_nodo_bloque->nomNodo)) {
			list_add(nombreNodosSinRepetidos, t_nodo_bloque->nomNodo);
		}
	}

	list_iterate(listaDeNodoBloque, (void*) tomarNodosSinRepetidos);

	t_list* listaNodosConListaDeBloques = list_create(); //TODO LIBERAR MEMORIA
	void obtenerBloquesDeNodo(char* nombreNodo) {
		t_nodo_lista_bloques* nodoConListaBloques = malloc(
				sizeof(t_nodo_lista_bloques)); //TODO LIBERAR MEMORIA
		for (int y = 0; y < listaDeNodoBloque->elements_count; y++) {
			t_nodo_bloque* nodoBloque = list_get(listaDeNodoBloque, y);
			if (string_equals_ignore_case(nodoBloque->nomNodo, nombreNodo)) {
				list_add(nodoConListaBloques->bloques, &nodoBloque->bloque);
			}
		}
		list_add(listaNodosConListaDeBloques, nodoConListaBloques);
	}

	list_iterate(nombreNodosSinRepetidos, (void*) obtenerBloquesDeNodo);

	return listaNodosConListaDeBloques;
}

void destruirConfiguracion(t_configuracion * configuracion) {
	free(configuracion->algoritmo);
	free(configuracion->ip);
	free(configuracion->puerto);
	free(configuracion->puerto_yama);
	free(configuracion);
}

