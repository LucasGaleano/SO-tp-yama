#include "dataNode.h"

bool recibirSolicitudes;

int main(void) {

	recibirSolicitudes=true;
	t_config* conf;
	char* bloque = malloc (TAMBLOQUE);
	t_log_level logLevel = LOG_LEVEL_INFO;               //elijo enum de log
    t_log* logger = log_create("dataNode.log", "dataNode", true, logLevel ); //creo archivo log


	//LEER ARCHIVO DE CONFIGURACION ---------------------------------------------------------

    conf = config_create(PATHCONFIG);                 // path relativo al archivo nodo.cfg
    char* IP_FILESYSTEM = config_get_string_value(conf,"IP_FILESYSTEM");        // traigo los datos del archivo nodo.cfg
    char* PUERTO_FILESYSTEM = config_get_string_value(conf,"PUERTO_FILESYSTEM");
    char* NOMBRE_NODO = config_get_string_value(conf,"NOMBRE_NODO");
    int PUERTO_WORKER = config_get_int_value(conf,"PUERTO_WORKER");
    int PUERTO_DATANODE = config_get_int_value(conf,"PUERTO_DATANODE");
    rutaDatabin = config_get_string_value(conf,"RUTA_DATABIN");

    //log_warning(logger, "algo paso aca!!!!!");


    //CONECTARSE A FILESYSTEM, QUEDAR A LA ESPERA DE SOLICITUDES --------------------------------

    int socketFileSystem = conectarCliente(IP_FILESYSTEM,PUERTO_FILESYSTEM,DATANODE);



    enviarInfoDataNode(socketFileSystem,NOMBRE_NODO,20,20);

    while(recibirSolicitudes){

    	gestionarSolicitudes(socketFileSystem, (void*) recibirSolicitud);

    }

	int i=0;
		for(;i<10;i++)
		{
			printf("%s\n",getBloque(i));
		}


	log_destroy(logger);
    config_destroy(conf);
    free(bloque);
	return EXIT_SUCCESS;

}

void recibirSolicitud(t_paquete * unPaquete, int * client_socket){

	t_log_level logLevel = LOG_LEVEL_INFO;               //elijo enum de log
    t_log* logger = log_create("dataNode.log", "dataNode", true, logLevel ); //creo archivo log

	switch (unPaquete->codigoOperacion) {
		case ENVIAR_SOLICITUD_LECTURA_BLOQUE:
			;
			int numBloque;
			char* bloque = malloc(TAMBLOQUE);
			numBloque = recibirSolicitudLecturaBloque(unPaquete);

			bloque = getBloque(numBloque);

			if(bloque == NULL)
			{
				log_error(logger,"error buscando bloque");
			}

			enviarBloque(*client_socket, bloque) ;

			free(bloque);


			break;
		case ENVIAR_SOLICITUD_ESCRITURA_BLOQUE:
			;
			t_pedidoEscritura* pedidoEscritura;
			pedidoEscritura = recibirSolicitudEscrituraBloque(unPaquete);
			printf("%d\n",pedidoEscritura->numBloque);
			printf("%s\n",pedidoEscritura->data);

			if (setBloque(pedidoEscritura->numBloque,pedidoEscritura->data) == -1)
			{
				log_error(logger,"error guardando bloque");
			}

			free(pedidoEscritura->data);
			free(pedidoEscritura->numBloque);
			free(pedidoEscritura);

			break;
		case ENVIAR_ERROR:
			;
			recibirSolicitudes=false;
			default:
			break;
	}

	log_destroy(logger);
	destruirPaquete(unPaquete);

}

char* getBloque(int numBloque)
{
	struct stat sb;
	char *map;
    char *bloque = malloc(TAMBLOQUE);

	int fd = open(rutaDatabin,	O_RDONLY); //abrir archivo data.bin

	fstat(fd, &sb);

    map = mmap(NULL,        //donde comienza a guardar el mapeo, NULL significa "donde quiera el S.O"
    			sb.st_size,		//el tamaño del file
				PROT_READ,		//proteccion del file (PROT_READ = solo lectura)
				MAP_SHARED,		//que comparta el mapeo con otros procesos creo, no se bien que hace
				fd,				//el file descriptor
				0);				//desde donde leer

    if (map == MAP_FAILED)
    {
    	close(fd);
    	perror("[-] Error mapeando el archivo");
    	return NULL;
    }

    int i;
	int j = 0;
	for(i = numBloque*TAMBLOQUE; i<(numBloque*TAMBLOQUE + TAMBLOQUE); i++ )
	{
		bloque[j] = map[i]; //leer
		j++;
	}


    if(munmap(map,sb.st_size) == -1) //cierro mmap()
    {
    	perror("[-]Error cerrando map");
    	return NULL;
    }

    close(fd);				//cierro archivo
    return bloque;

}

int setBloque(int numBloque, char* bloque)
{
	struct stat sb;
	char *map;

	int fd = open(rutaDatabin,O_RDWR); //abrir archivo data.bin

	fstat(fd, &sb);

    map = (char*) mmap(NULL,        //donde comienza a guardar el mapeo, NULL significa "donde quiera el S.O"
    			sb.st_size,		//el tamaño del file
				PROT_READ | PROT_WRITE,		//proteccion del file (PROT_READ = solo lectura)
				MAP_SHARED,		//que comparta el mapeo con otros procesos creo, no se bien que hace
				fd,				//el file descriptor
				0);				//desde donde leer


    if (map == MAP_FAILED)
    {
    	close(fd);
    	perror("[-] Error mapeando el archivo");
    	return -1;
    }

	int i = numBloque*TAMBLOQUE;
	int j = 0;
	for(i; i<(numBloque*TAMBLOQUE + TAMBLOQUE); i++ )
	{
		map[i] = bloque[j]; //escribe
		j++;
	}


	if(munmap(map,sb.st_size) == -1) //cierro mmap()
	    {
	    	perror("[-]Error cerrando map");
	    	return -1;
	    }


    close(fd);				//cierro archivo
    return 0;


}


