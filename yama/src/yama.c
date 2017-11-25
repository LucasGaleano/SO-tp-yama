#include "yama.h"

int main(void) {

	char* path_config_yama ="/home/utnso/workspace/tp-2017-2c-NULL/configuraciones/yama.cfg";

	t_configuracion * config = leerArchivoDeConfiguracionYAMA(path_config_yama);

	//Creo el thread para escuchar conexiones
	pthread_t threadServerYama;

	if (pthread_create(&threadServerYama, NULL, (void*) iniciarServidor, config->puerto_yama)) {
		perror("Error el crear el thread servidor.");
		exit(EXIT_FAILURE);
	}

	int socketFS = conectarCliente(config->ip, config->puerto, YAMA);

	char * prueba = string_new();
	string_append(&prueba, "hola");
	enviarMensaje(socketFS, prueba);

	iniciarPlanificador(config->algoritmo);

	while(1);

	config_destroy(config);
//	list_destroy_and_destroy_elements(tabla_de_estados, (void*) eliminarElemento() ); //TODO PREGUNTAR SI ESTA BIEN LIBERAR DESPUES DE USAR
	return 0;
}

void eliminarElemento(){
}

void iniciarServidor(char* unPuerto) {
	iniciarServer(unPuerto, (void *) procesarPaquete);
}

void procesarPaquete(t_paquete * unPaquete, int * client_socket) {
	switch (unPaquete->codigoOperacion) {
	case HANDSHAKE:
		recibirHandshake(unPaquete, client_socket);
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
	default:
		break;
	}
	destruirPaquete(unPaquete);
}

void recibirHandshake(t_paquete * unPaquete, int * client_socket) {
	int tipoCliente;
	memcpy(&tipoCliente, unPaquete->buffer->data, sizeof(int));
	switch (tipoCliente) {
	case MASTER:
		break;
	default:
		*client_socket = -1;
		break;
	}
}

void recibirError(t_paquete * unPaquete) {
	int cliente_desconectado;
	memcpy(&cliente_desconectado, unPaquete->buffer->data, sizeof(int));

// HACER ALGO

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


int actualizarTablaDeEstados(t_list* tabla_de_estados, t_elemento_tabla_estado fila_nueva) {
	return list_add(tabla_de_estados, &fila_nueva);
}

int numeroRandom() {
	unsigned short lfsr = 0xACE1u;
	unsigned bit;

	bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
	return lfsr = (lfsr >> 1) | (bit << 15);
}



//int a = 0;
//	int num = 1;
//	int numAnt = 2;
//	bool guardoAnt = false;
//	while(num != numAnt){
//		a += 1;
//		if(guardoAnt){
//			numAnt = num;
//		}
//		num = rand();
//		guardoAnt = true;
//		printf("Num: %d y NumAnt: %d\n", num, numAnt);
//	}
//	printf("ENCONTRE NUMEROS REPEDITOS EN POS: %d Num: %d y NumAnt: %d\n",a, num, numAnt);
