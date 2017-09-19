/*
 ============================================================================
 Name        : dataNode.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h> //config
#include <commons/log.h> //log
#include <fcntl.h>  // O_RDONLY modo de abrir para el open()
#include <sys/stat.h> //fstat()
#include <sys/mman.h>  //mmap()
#include <unistd.h>  //PROT_READ del mmap()

#define PATHDATA "./data.bin"
#define TAMBLOQUE 8

char* getBloque(int);

int main(void) {



	t_config* conf;
    conf = config_create("../configuraciones/nodo.cfg");                 // path relativo al archivo nodo.cfg

    char* IP_FILESYSTEM = config_get_string_value(conf,"IP_FILESYSTEM");        // traigo los datos del archivo nodo.cfg
    char* PUERTO_FILESYSTEM = config_get_string_value(conf,"PUERTO_FILESYSTEM");
    int NOMBRE_NODO = config_get_string_value(conf,"NOMBRE_NODO");
    int PUERTO_WORKER = config_get_int_value(conf,"PUERTO_WORKER");
    int PUERTO_DATANODE = config_get_int_value(conf,"PUERTO_DATANODE");
    char* RUTA_DATABIN = config_get_string_value(conf,"RUTA_DATABIN");

    t_log_level logLevel = LOG_LEVEL_INFO;               //elijo enum
    t_log* logger = log_create("dataNode_log", "dataNode", true, logLevel ); //creo archivo log

    //log_warning(logger, "algo paso aca!!!!!");

    char* bloque = getBloque(2);

    puts(bloque);
	return EXIT_SUCCESS;

}


char* getBloque(int numBloque)
{
	struct stat sb;
	int *map;
    char *bloque  = calloc(9,1);

	int fd = open(PATHDATA,	O_RDONLY); //abrir archivo data.bin

	fstat(fd, &sb);

    map = mmap(NULL,        //donde comienza a guardar el mapeo, NULL significa "donde quiera el S.O"
    		sb.st_size,		//el tamaño del file
			PROT_READ,		//proteccion del file (PROT_READ = solo lectura)
			MAP_SHARED,		//que comparta el mapeo con otros procesos creo, no se bien que hace
			fd,				//el file descriptor
			0);				//desde donde leer

	char* a = (char*)map;
	int i = numBloque*TAMBLOQUE - TAMBLOQUE;
	int j = 0;
	for(i; i<(numBloque*TAMBLOQUE); i++ )
	{
		bloque[j] = a[i];
		j++;
	}


    munmap(map,sb.st_size);
    close(fd);

    return bloque;

}

