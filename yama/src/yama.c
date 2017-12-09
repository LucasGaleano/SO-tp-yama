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
	tablaPlanificador = Planificador_create();

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
	configuracion->retardo = config_get_int_value(config, "RETARDO_PLANIFICACION");
	configuracion->algoritmo = config_get_string_value(config, "ALGORITMO_BALANCEO");
	configuracion->puerto_yama = config_get_string_value(config, "PUERTO_YAMA");
	configuracion->disponibilidad_base = config_get_int_value(config, "DISPONIBILIDAD_BASE");

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
	case ENVIAR_SOLICITUD_TRANSFORMACION: //RECIBO SOLICITUD DE TRANSFORMACION CON PATH DE ARCHIVO
		procesarEnviarSolicitudTransformacion(unPaquete, client_socket); //ENVIO A FS PATH DE ARCHIVO
		break;
	case ENVIAR_LISTA_NODO_BLOQUES: //RECIBO LISTA DE ARCHIVOS DE FS CON UBICACIONES Y BLOQUES
		procesarEnviarListaNodoBloques(unPaquete); //
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

void procesarEnviarSolicitudTransformacion(t_paquete * unPaquete,int *client_socket) {
	queue_push(cola_master, client_socket);	// todo-->Verificar que esto haga lo que realmente quiero
	char * nomArchivo = recibirMensaje(unPaquete);
	enviarRutaArchivo(socketFS, nomArchivo);
}

void procesarEnviarListaNodoBloques(t_paquete * unPaquete){
	t_nodos_bloques * nodosBloques = recibirListaNodoBloques(unPaquete);

	t_list* listNodoBloque = nodosBloques.nodoBloque;

	t_list* listaBloquesConNodos = agruparNodosPorBloque(listNodoBloque);
	t_list* nodosSinRepetidos = nodosSinRepetidos(listNodoBloque);

	void agregarATablaPlanificador(char* nombreNodo){
		planificador_agregarWorker(tablaPlanificador, nombreNodo);
	}

	list_iterate(nodosSinRepetidos,(void*) agregarATablaPlanificador);

	void planificador(int algoritmo, t_list * listaDeBloques,t_list* tablaPlanificador, int DispBase);
	planificador(configuracion->algoritmo, listaBloquesConNodos, tablaPlanificador, configuracion->disponibilidad_base);


}
/*-------------------------Funciones auxiliares-------------------------*/

void destruirIndicacionDeTransformacion(t_indicacionTransformacion* indTransform) {
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

t_list* agruparNodosPorBloque(t_list* listaDeNodoBloque) {
	t_list* bloquesSinRepetidos = list_create(); //TODO LIBERAR LISTA

	bool existeNodoEnLaLista(int numeroBloque) {
		bool booleano = false;
		for (int x = 0; x < bloquesSinRepetidos->elements_count; x++) {
			if (list_get(bloquesSinRepetidos, x), numeroBloque) {
				booleano = true;
				break;
			}
		}
		return booleano;
	}

	void tomarBloquesSinRepetidos(t_nodo_bloque* t_nodo_bloque) {
		if (!existeNodoEnLaLista(t_nodo_bloque->bloqueArchivo)) {
			list_add(bloquesSinRepetidos, t_nodo_bloque->bloqueArchivo);
		}
	}

	list_iterate(listaDeNodoBloque, (void*) tomarBloquesSinRepetidos);

	t_list* listaBloquesConListaDeNodos = list_create(); //TODO LIBERAR MEMORIA

	void obtenerNodosDeBloque(int numeroBloque) {
		t_nodos_por_bloque* bloqueConListaNodos = malloc(sizeof(t_nodos_por_bloque)); //TODO LIBERAR MEMORIA
		for (int y = 0; y < listaDeNodoBloque->elements_count; y++) {
			t_nodo_bloque* nodoBloque = list_get(listaDeNodoBloque, y);
			if (numeroBloque == nodoBloque->bloqueArchivo) {
				list_add(bloqueConListaNodos->nodosEnLosQueEsta, nodoBloque->nomNodo);
			}
		}
		list_add(listaBloquesConListaDeNodos, bloqueConListaNodos);
	}

	list_iterate(bloquesSinRepetidos, (void*) obtenerNodosDeBloque);

	return listaBloquesConListaDeNodos;
}

t_list* nodosSinRepetidos(t_list* listaDeNodoBloque){

	t_list* nodosSinRepetidos = list_create(); //TODO LIBERAR LISTA

	bool existeNodoEnLaLista(char* nombreNodo) {
		bool booleano = false;
		for (int x = 0; x < nodosSinRepetidos->elements_count; x++) {
			if (string_equals_ignore_case(list_get(nodosSinRepetidos, x), nombreNodo)) {
				booleano = true;
				break;
			}
		}
		return booleano;
	}

	void tomarNodosSinRepetidos(t_nodo_bloque* t_nodo_bloque) {
		if (!existeNodoEnLaLista(t_nodo_bloque->nomNodo)) {
			list_add(nodosSinRepetidos, t_nodo_bloque->nomNodo);
		}
	}

	list_iterate(listaDeNodoBloque, (void*) tomarNodosSinRepetidos);

	return nodosSinRepetidos;
}

void destruirConfiguracion(t_configuracion * configuracion) {
	free(configuracion->algoritmo);
	free(configuracion->ip);
	free(configuracion->puerto);
	free(configuracion->puerto_yama);
	free(configuracion);
}

