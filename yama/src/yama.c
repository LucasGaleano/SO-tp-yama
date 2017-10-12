/*
 ============================================================================
 Name        : yama.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <biblioteca/sockets.h>

typedef struct {
	int job;
	int master;
	int nodo;
	int bloque;
	int etapa;
	char * archivoTemporal;
	int estado;
} t_elemento_tabla_estado;

typedef struct {
	char * ip;
	char * puerto;
	int retardo;
	char * algoritmo;
} t_configuracion;

unsigned short lfsr = 0xACE1u;
unsigned bit;

int numeroRandom() {
	bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
	return lfsr = (lfsr >> 1) | (bit << 15);
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

	printf("Se creo el proceso YAMA con: IP: %s - PUERTO: %s - RETARDO: %d - ALGORITMO: %s\n",
			configuracion->ip, configuracion->puerto, configuracion->retardo, configuracion->algoritmo);

	return configuracion;
}

int agregarElementoEnTablaDeEstado(t_list* tabla_de_estados,
		t_elemento_tabla_estado fila_nueva) {
	return list_add(tabla_de_estados, &fila_nueva);
}

#include <commons/string.h>

int main(void) {

	char* path_config_yama ="/home/utnso/workspace/tp-2017-2c-NULL/configuraciones/yama.cfg";

	t_configuracion * config = leerArchivoDeConfiguracionYAMA(path_config_yama);

	int socketFS = conectarCliente(config->ip, config->puerto, YAMA);

	char * prueba = string_new();
	string_append(&prueba, "hola");
	enviarMensaje(socketFS, prueba);

	while(1);

	return 0;
}

//elemento_tabla_estado elemento;
//
//	elemento.job = 1;
//	elemento.master = 2;
//	elemento.nodo = 3;
//	elemento.bloque = 4;
//	elemento.etapa = 5;
//	elemento.archivoTemporal = "/archivo.tmp";
//	elemento.estado = 6;
//
//	agregarElementoEnTablaDeEstado(tabla_de_estados, elemento);
//
//	printf("ARCHIVO_TEMPORAL: %s - BLOQUE: %d - MASTER: %d - NODO: %d\n", elem->archivoTemporal, elem->bloque, elem->master, elem->nodo);
//	elemento_tabla_estado* elem = list_get(tabla_de_estados, 0);

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
