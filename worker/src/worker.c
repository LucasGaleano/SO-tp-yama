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

#define PATHCONFIG "../configuraciones/nodo.cfg"
#define TAM_MAX 100
#define TAM_BLOQUE 1048576 //1024 * 1024, un mega
#define PUERTO_REDUCCION_GLOBAL 9999

int main(void) {

	t_config *conf;

	//LEER ARCHIVO DE CONFIGURACION ---------------------------------------------------------

	conf = config_create(PATHCONFIG);       // path relativo al archivo nodo.cfg
	//int NOMBRE_NODO = config_get_string_value(conf,"NOMBRE_NODO");
	int PUERTO_WORKER = config_get_int_value(conf, "PUERTO_WORKER");
	char* RUTA_DATABIN = config_get_string_value(conf, "RUTA_DATABIN");

	iniciarServer(PUERTO_WORKER, (void *) procesarPaquete);

	config_destroy(conf);
	return EXIT_SUCCESS;
}

void procesarPaquete(t_paquete * unPaquete, int * client_socket) {
	int pid = 0;
	t_indicacionTransformacion *auxTransf;
	t_indicacionReduccionLocal *auxRedL;
	t_indicacionReduccionGlobal *auxRedG;
	switch (unPaquete->codigoOperacion) {
	case HANDSHAKE:
		recibirHandshake(unPaquete, client_socket);
		break;
	case ENVIAR_INDICACION_TRANSFORMACION:
		auxTransf = recibirIndicacionTransformacion(unPaquete);
		pid = fork();
		if (pid == 0) {
			transformacion(auxTransf->bloque, auxTransf->rutaArchivoTemporal);
			_exit(EXIT_SUCCESS);
		}
		break;
	case ENVIAR_INDICACION_REDUCCION_LOCAL:
		auxRedL = recibirIndicacionReduccionLocal(unPaquete);
		/*if(pid == 0)
		 {
		 reduccion();
		 _exit(EXIT_SUCCESS);
		 }*/
		//hablar con grupo sobre cambio a estructura
		break;
	case ENVIAR_INDICACION_REDUCCION_GLOBAL:
		auxRedG = recibirIndicacionReduccionGlobal(unPaquete);
		pid = fork();
		if (pid == 0) {
			if (auxRedG->encargado)
				iniciarEncargado();
			else
				iniciarEsclavo(auxRedG->ip,
						auxRedG->archivoDeReduccionLocal);
			_exit(EXIT_SUCCESS);
		}
		break;
		/*case ENVIAR_INDICACION_ALMACENADO_FINAL:
		 break;*/
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

	//remove("salidaDeTransformacion");

	free(comando);
}

inline int sonTodosVerdaderos(int *fdt, int cant)
 {
 int i;
 for (i = 0; i < cant; i++)
 {
 if (fdt[i] == 0)
 return 0;
 }
 return 1;
 }

void reduccionLocal(char** rutas, int cant, char* rutaFinal) {
	FILE** loc = (FILE**)calloc(cant, sizeof(FILE*));
	FILE* apareado = fopen(rutaFinal, "w");
	if (apareado == NULL)
		_exit(EXIT_FAILURE);
	int i = 0, j = 0, esPrimero = 0;
	int* fdt = calloc(cant, sizeof(int)); //Para saber si ya llego a EOF. 0 para no, 1 para si
	char** leido = (char**) calloc(cant, sizeof(char*));
	char* ret;
	for (i = 0; i < cant; i++) {
		loc[i] = fopen(rutas[i], "r");
		 if (loc[i] == NULL)
		 {
			 fclose(apareado);
			 _exit(EXIT_FAILURE);
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
		remove(rutas[i]);
	}
	fclose(apareado);
	free(loc);
	free(fdt);
	free(leido);

}

void recibirDatos(t_paquete * unPaquete) {
	void* paq = recibirArchivo(unPaquete);
}

void iniciarEncargado() {
	iniciarServer(PUERTO_REDUCCION_GLOBAL, (void*) recibirDatos);
}

void iniciarEsclavo(char* ip, char* rutaLocal) {
	int socket = conectarCliente(ip, PUERTO_REDUCCION_GLOBAL, WORKER);
	enviarArchivo(socket, rutaLocal);
}
