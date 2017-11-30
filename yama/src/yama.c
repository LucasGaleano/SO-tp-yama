#include "yama.h"

int main(void) {

	char* path_config_yama ="/home/utnso/workspace/tp-2017-2c-NULL/configuraciones/yama.cfg";

	t_configuracion * config = leerArchivoDeConfiguracionYAMA(path_config_yama);

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
	case RESPUESTA_INFO_ARCHIVO:
		;
		//int socket_master = queue_pop(cola_master);

		//me llega la respuesta de FS y despues de procesarla la envio al master de la lista masterconectados.

		t_list* listaDeBloque = recibirListaDeBloques(unPaquete); //recibir lista de FS



		while(listaDeBloque->elements_count!=0){   // lista elementos uno por uno
			t_bloque* bloque = list_remove(listaDeBloque,0);

			//guardar en tabla de estado
			agregarRegistro(generarJob(), list_get(masterConectados,0),bloque->copia0->nodo,bloque->copia0->numBloque
					, TRANSFORMACION ,nombreArchivoTemp(), PROCESANDO);
			//todo [CAMBIAR] elige siempre al mismo master cuando elije de la lista masterConectados
			//todo [CAMBIAR] hacer planificador para elegir bien el nodo
			//todo [DUDA] enum etapa lo usa master??, ponerlo en la biblioteca-propia sino

			//todo enviar lista a master de a un bloque

			//enviarIndicacionTransformacion();

		}


		break;
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

t_configuracion * leerArchivoDeConfiguracionYAMA(char* path) {

	t_config * config = config_create(path);

	//Valido que el archivo de configuracion este con los datos correspondientes
	if(!config_has_property(config, "FS_IP") || !config_has_property(config, "FS_PUERTO")
			|| !config_has_property(config, "RETARDO_PLANIFICACION") || !config_has_property(config, "ALGORITMO_BALANCEO")){
		printf("ERROR: Alguna de las propiedades definidas en el archivo de configuracion es invalida o vacia \n");
		exit(1);
	}

	t_configuracion * configuracion = malloc(sizeof(t_configuracion));

	configuracion->ip = config_get_string_value(config, "FS_IP");
	configuracion->puerto = config_get_string_value(config, "FS_PUERTO");
	configuracion->retardo = config_get_int_value(config, "RETARDO_PLANIFICACION");
	configuracion->algoritmo = config_get_string_value(config, "ALGORITMO_BALANCEO");
	configuracion->puerto_yama = config_get_string_value(config, "PUERTO_YAMA");
	configuracion->disponibilidad_base = config_get_int_value(config, "DISPONIBILIDAD_BASE");

	printf("Se levanto el proceso YAMA con: YAMA_PUERTO: %s  FS_IP: %s - FS_PUERTO: %s - RETARDO: %d - ALGORITMO: %s - DISPONIBILIDAD BASE: %d \n",
			configuracion->puerto_yama, configuracion->ip, configuracion->puerto, configuracion->retardo, configuracion->algoritmo, configuracion->disponibilidad_base);

	return configuracion;
}

long generarJob(){
	idJob += 1;
	return idJob;
}



