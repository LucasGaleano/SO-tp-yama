#include "dataNode.h"

int main(void) {

	t_config* conf;
	char* bloque = malloc (TAMBLOQUE);
	t_log_level logLevel = LOG_LEVEL_INFO;               //elijo enum de log
    t_log* logger = log_create("dataNode_log", "dataNode", true, logLevel ); //creo archivo log


	//LEER ARCHIVO DE CONFIGURACION ---------------------------------------------------------

    conf = config_create(PATHCONFIG);                 // path relativo al archivo nodo.cfg
    char* IP_FILESYSTEM = config_get_string_value(conf,"IP_FILESYSTEM");        // traigo los datos del archivo nodo.cfg
    char* PUERTO_FILESYSTEM = config_get_string_value(conf,"PUERTO_FILESYSTEM");
    char* NOMBRE_NODO = config_get_string_value(conf,"NOMBRE_NODO");
    int PUERTO_WORKER = config_get_int_value(conf,"PUERTO_WORKER");
    int PUERTO_DATANODE = config_get_int_value(conf,"PUERTO_DATANODE");
    char* RUTA_DATABIN = config_get_string_value(conf,"RUTA_DATABIN");

    //log_warning(logger, "algo paso aca!!!!!");

    //setBloque(2,"11111111",RUTA_DATABIN);
    //bloque = getBloque(2,RUTA_DATABIN);
    //printf("bloque: %s \n",bloque );



    //CONECTARSE A FILESYSTEM, QUEDAR A LA ESPERA DE SOLICITUDES --------------------------------

    int socketFileSystem = conectarCliente(IP_FILESYSTEM,PUERTO_FILESYSTEM,DATANODE);


    char* msg = string_new();
    string_append(msg,"hola");

    enviarInfoDataNode(socketFileSystem,msg,100,0);

    while(true){

    	gestionarSolicitudes(socketFileSystem, (void*) recibirSolicitud);


    }

    free(bloque);
	return EXIT_SUCCESS;

}

void recibirSolicitud(t_paquete * unPaquete, int * client_socket){
	switch (unPaquete->codigoOperacion) {
		case ENVIAR_SOLICITUD_LECTURA_BLOQUE:
			;
			int numBloque;
			char* bloque = malloc(TAMBLOQUE);
			numBloque = recibirSolicitudLecturaBloque(unPaquete);
			bloque = getBloque(numBloque);

			enviarBloque(*client_socket, bloque);

			free(bloque);


			break;
		case ENVIAR_SOLICITUD_ESCRITURA_BLOQUE:
			;
			t_pedidoEscritura* pedidoEscritura = malloc(sizeof(t_pedidoEscritura));
			pedidoEscritura = recibirSolicitudEscrituraBloque(unPaquete);
			setBloque(pedidoEscritura->numBloque,pedidoEscritura->data);

		default:
			break;
	}

}

char* getBloque(int numBloque)
{
	struct stat sb;
	char *map;
    char *bloque = malloc(TAMBLOQUE);

	int fd = open(PATHCONFIG,	O_RDONLY); //abrir archivo data.bin

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
    	exit(EXIT_FAILURE);
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
    	exit(EXIT_FAILURE);
    }

    close(fd);				//cierro archivo
    return bloque;

}

void setBloque(int numBloque, char* bloque)
{
	struct stat sb;
	char *map;

	int fd = open(PATHCONFIG,O_RDWR); //abrir archivo data.bin

	fstat(fd, &sb);

    map = (char*) mmap(NULL,        //donde comienza a guardar el mapeo, NULL significa "donde quiera el S.O"
    			sb.st_size,		//el tamaño del file
				PROT_READ | PROT_WRITE,		//proteccion del file (PROT_READ = solo lectura)
				MAP_SHARED,		//que comparta el mapeo con otros procesos creo, no se bien que hace
				fd,				//el file descriptor
				0);				//desde donde leer

	int i = numBloque*TAMBLOQUE;
	int j = 0;
	for(i; i<(numBloque*TAMBLOQUE + TAMBLOQUE); i++ )
	{
		map[i] = bloque[j]; //escribe
		j++;
	}


    munmap(map,sb.st_size);  //cierro mmap()
    close(fd);				//cierro archivo



}


