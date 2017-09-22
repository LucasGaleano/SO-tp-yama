/*
 ============================================================================
 Name        : worker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Proceso workers
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h> //config
#include <commons/log.h> //log
#include <commons/txt.h> //txt
#include <fcntl.h>  // O_RDONLY modo de abrir para el open()
#include <sys/stat.h> //fstat()
#include <sys/mman.h>  //mmap()
#include <unistd.h>  //PROT_READ del mmap()

#define PATHCONFIG "../configuraciones/nodo.cfg"

int main(void) {

	t_config *conf;
	void transformacion(char* bloque);

	//LEER ARCHIVO DE CONFIGURACION ---------------------------------------------------------

   	conf = config_create(PATHCONFIG);                 // path relativo al archivo nodo.cfg
    char* IP_FILESYSTEM = config_get_string_value(conf,"IP_FILESYSTEM");        // traigo los datos del archivo nodo.cfg
    char* PUERTO_FILESYSTEM = config_get_string_value(conf,"PUERTO_FILESYSTEM");
    int NOMBRE_NODO = config_get_string_value(conf,"NOMBRE_NODO");
    int PUERTO_WORKER = config_get_int_value(conf,"PUERTO_WORKER");
    int PUERTO_DATANODE = config_get_int_value(conf,"PUERTO_DATANODE");
    char* RUTA_DATABIN = config_get_string_value(conf,"RUTA_DATABIN");


    transformacion("011010100101");






	return EXIT_SUCCESS;
}


void transformacion(char* bloque){

	//aplicar tranformacion aca

	char* time = temporal_get_string_time();
	FILE* archivo_temp = txt_open_for_append(strcat(time , ".tmp"));
	txt_write_in_file(archivo_temp,bloque);
	txt_close_file(archivo_temp);

}
