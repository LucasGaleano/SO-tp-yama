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

struct filaTablaDeEstado
  {
      int job;
      int master;
      int nodo;
      int bloque;
      int etapa;
	  char * archivoTemporal;
      int estado;
  } elemento_tabla_estado;

int main(void) {

	char* path = "/home/utnso/workspace/tp-2017-2c-NULL/configuraciones/yama.cfg";

	t_config * config = config_create(path);

	char* ip = config_get_string_value(config, "FS_IP");
	int puerto = config_get_int_value(config, "FS_PUERTO");
	int retardo = config_get_int_value(config, "RETARDO_PLANIFICACION");
	char* algoritmo = config_get_string_value(config, "ALGORITMO_BALANCEO");

	printf("IP: %s - PUERTO: %d - RETARDO: %d - ALGORITMO: %s\n", ip, puerto, retardo, algoritmo);

	return 0;
}

