#include "yama.h"

int main(void) {

	char* path_config_yama ="/home/utnso/workspace/tp-2017-2c-NULL/configuraciones/yama.cfg";

	leerArchivoDeConfiguracionYAMA(path_config_yama);

	//Creo el thread para escuchar conexiones
	pthread_t threadServerYama;
	idJob = 0;
	cola_master = queue_create();
	masterConectados = list_create();

	if (pthread_create(&threadServerYama, NULL, (void*) iniciarServidor, config->puerto_yama)) {
		perror("Error el crear el thread servidor.");
		exit(EXIT_FAILURE);
	}

	//Me conecto con el file system
	socketFS = conectarCliente(config->ip, config->puerto, YAMA);

	iniciarPlanificador(config->algoritmo);

	config_destroy(config);
//	list_destroy_and_destroy_elements(tabla_de_estados, (void*) eliminarElemento() ); //TODO PREGUNTAR SI ESTA BIEN LIBERAR DESPUES DE USAR
	return 0;
}

void iniciarServidor(char* unPuerto) {
	iniciarServer(unPuerto, (void *) procesarPaquete);
}

void procesarPaquete(t_paquete * unPaquete, int * client_socket) {
	switch (unPaquete->codigoOperacion) {
	case HANDSHAKE:
		recibirHandshake(unPaquete);
		list_add(masterConectados,client_socket); // una lista de master conectados para distribuir job
		break;
	case ENVIAR_MENSAJE:
		recibirMensaje(unPaquete);
		break;
	case ENVIAR_ARCHIVO:
		recibirArchivo(unPaquete);
		break;
	case ENVIAR_ERROR:
		recibirError(unPaquete);
		break;
	case ENVIAR_SOLICITUD_TRANSFORMACION:
		;
		queue_push(cola_master, client_socket);
		char * nomArchivo = recibirMensaje(unPaquete);
		enviarRutaArchivo(socketFS, nomArchivo);
		break;
	case ENVIAR_INDICACION_TRANSFORMACION:
		;
		//int socket_master = queue_pop(cola_master);

		//me llega la respuesta de FS y despues de procesarla la envio al master de la lista masterconectados.

		t_list* listaDeBloque = recibirListaDeBloques(unPaquete); //recibir lista de FS

		char* prefijo = string_new();    //prefijo del archivo temporal
		string_append(prefijo,"transformacion:");
		char* nombreTemp = nombreArchivoTemp(prefijo);

		//PLANIFICADOR-----------------------

		t_tablaPlanificador* tablaPlanificador = tablaPlanificador_create();
		Planificador(config->algoritmo, listaDeBloque, masterConectados, tablaPlanificador);


		while(listaDeBloque->elements_count!=0){   // lista elementos uno por uno
			t_bloque* bloque = list_remove(listaDeBloque,0);
			int socketMasterElegido = list_get(masterConectados,0); //todo [CAMBIAR] elige siempre al mismo master cuando elige de la lista masterConectados, deberia ser equitativo tipo RR



			t_bloque_ubicacion* bloqueUbicacion = bloque->copia0;

			//guardar en tabla de estado
			agregarRegistro(generarJob(), socketMasterElegido,bloqueUbicacion->nodo,bloqueUbicacion->numBloque
					, TRANSFORMACION ,nombreTemp, PROCESANDO);


			//todo [DUDA] enum etapa, lo usa master??, ponerlo en la biblioteca-propia sino

			//todo enviar lista a master de a un bloque
			t_indicacionTransformacion* bloqueEnviar = bloqueAT_indicacionTranformacion(bloque->tamanioOcupado,bloqueUbicacion,
					nombreTemp);  //paso de bloque a T_indicacionTranformacion
			enviarIndicacionTransformacion(socketMasterElegido, bloqueEnviar);


			//TODO free() al final

		}



		//TODO free() al final
		break;

	case TAREA_COMPLETADA:
		;

		t_indicacionTransformacion* paqueteTransform = recibirIndicacionTransformacion(unPaquete);

		modificarEstadoDeRegistro(paqueteTransform->nodo, paqueteTransform->bloque, TRANSFORMACION, FINALIZADO_OK);


		//todo fijarse si un nodo termino la transformacion.

		if(NULL == TerminoElNodo(paqueteTransform->nodo,TRANSFORMACION,FINALIZADO_OK)){//si terminó un nodo empezar reduccion local en ese nodo

			t_indicacionReduccionLocal* PaqueRedLocal = malloc(sizeof(t_indicacionReduccionLocal));

			char* prefijo = string_new();    //prefijo del archivo temporal
			string_append(prefijo,"reduccion:");
			char* nombreTempReduccion = nombreArchivoTemp(prefijo);

			PaqueRedLocal->nodo = malloc(sizeof(paqueteTransform->nodo));
			PaqueRedLocal->ip = malloc(sizeof(paqueteTransform->ip));
			PaqueRedLocal->puerto = malloc(sizeof(paqueteTransform->puerto));
			PaqueRedLocal->archivoTemporalTransformacion = malloc(sizeof(paqueteTransform->rutaArchivoTemporal));
			PaqueRedLocal->archivoTemporalReduccionLocal = malloc(sizeof(nombreTempReduccion));

			enviarIndicacionReduccionLocal(client_socket,PaqueRedLocal);
		}



	default:
		break;
	}
	destruirPaquete(unPaquete);
}







int recibirHandshake(t_paquete * unPaquete) {
	int tipoCliente;
	memcpy(&tipoCliente, unPaquete->buffer->data, sizeof(int));
	switch (tipoCliente) {
 	case MASTER:

		return 0;
		break;
	default:
		//TODO ENVIAR ERROR A MASTER
		return -1;
		break;
	}
}

void recibirError(t_paquete * unPaquete) {
	int cliente_desconectado;
	memcpy(&cliente_desconectado, unPaquete->buffer->data, sizeof(int));

// todo HACER ALGO ANTE EL ERROR

}

void leerArchivoDeConfiguracionYAMA() {


	//Valido que el archivo de configuracion este con los datos correspondientes
	if(!config_has_property(config, "FS_IP") || !config_has_property(config, "FS_PUERTO")
			|| !config_has_property(config, "RETARDO_PLANIFICACION") || !config_has_property(config, "ALGORITMO_BALANCEO")){
		printf("ERROR: Alguna de las propiedades definidas en el archivo de configuracion es invalida o vacia \n");
		exit(1);
	}


	config->ip = config_get_string_value(config, "FS_IP");
	config->puerto = config_get_string_value(config, "FS_PUERTO");
	config->retardo = config_get_int_value(config, "RETARDO_PLANIFICACION");
	config->algoritmo = config_get_string_value(config, "ALGORITMO_BALANCEO");
	config->puerto_yama = config_get_string_value(config, "PUERTO_YAMA");
	config->disponibilidad_base = config_get_int_value(config, "DISPONIBILIDAD_BASE");

	printf("Se levanto el proceso YAMA con: YAMA_PUERTO: %s  FS_IP: %s - FS_PUERTO: %s - RETARDO: %d - ALGORITMO: %s - DISPONIBILIDAD BASE: %d \n",
			config.puerto_yama, config.ip, config.puerto, config.retardo, config.algoritmo, config.disponibilidad_base);

}

t_indicacionTransformacion* bloqueAT_indicacionTranformacion(int tamanio, t_bloque_ubicacion* ubicacion,char* nombreTemp){

	t_indicacionTransformacion* indTransform = malloc(sizeof(t_indicacionTransformacion));

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

//TODO funcion destruir el t_indicacionTranformacion

long generarJob(){
	idJob += 1;
	return idJob;
}

t_worker* worker_create(){
	t_worker* worker = malloc(sizeof(t_worker));
	return worker;

}

t_tablaPlanificador* tablaPlanificador_create(){

	t_tablaPlanificador* tb = malloc(t_tablaPlanificador);
	tb->registros = list_create();

	return tb;

}




