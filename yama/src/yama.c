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
#include <stdlib.h>
#include <commons/config.h>
#include <commons/collections/list.h>

typedef struct{
      int job;
      int master;
      int nodo;
      int bloque;
      int etapa;
	  char * archivoTemporal;
      int estado;
  } elemento_tabla_estado;

void leerArchivoDeConfiguracionYAMA(char* path){

		t_config * config = config_create(path);

		char* ip = config_get_string_value(config, "FS_IP");
		int puerto = config_get_int_value(config, "FS_PUERTO");
		int retardo = config_get_int_value(config, "RETARDO_PLANIFICACION");
		char* algoritmo = config_get_string_value(config, "ALGORITMO_BALANCEO");

		printf("IP: %s - PUERTO: %d - RETARDO: %d - ALGORITMO: %s\n", ip, puerto, retardo, algoritmo);
}

int agregarElementoEnTablaDeEstado(t_list* tabla_de_estados, elemento_tabla_estado fila_nueva){
	return list_add(tabla_de_estados, &fila_nueva);
}

int main(void) {

	t_list* tabla_de_estados = list_create();

	char* path_config_yama = "/home/utnso/workspace/tp-2017-2c-NULL/configuraciones/yama.cfg";
	leerArchivoDeConfiguracionYAMA(path_config_yama);




	return 0;
}

