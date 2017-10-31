#ifndef DATANODE_H_
#define DATANODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h> //config
#include <commons/log.h> //log
#include <commons/string.h>
#include <fcntl.h>  // O_RDONLY modo de abrir para el open()
#include <sys/stat.h> //fstat()
#include <sys/mman.h>  //mmap()
#include <unistd.h>  //PROT_READ del mmap()
#include <biblioteca/sockets.h>
#include <stdbool.h>
#include <biblioteca/estructuras.h>
#include <biblioteca/paquetes.h>

#define PATHCONFIG "./configuraciones/nodo.cfg"

#define TAMBLOQUE 8

/**
* @NAME: getBloque
* @DESC: Retorna el numero del bloque pedido
*/

char* getBloque (int numBLoque,char* pathFile);

/**
* @NAME: setBloque
* @DESC: escribe el numero del bloque pedido
*/

void setBloque (int numBLoque, char* bloque,char* pathFile);

void recibirSolicitud(t_paquete * unPaquete, int * client_socket);

#endif /*DATANODE_H_*/
