#ifndef DATANODE_H_
#define DATANODE_H_
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h> //config
#include <commons/log.h> //log
#include <commons/string.h>
#include <fcntl.h> // O_RDONLY modo de abrir para el open()
#include <sys/stat.h> //fstat()
#include <sys/mman.h> //mmap()
#include <unistd.h> //PROT_READ del mmap()
#include <biblioteca/sockets.h>
//#include <stdbool.h>
#include <biblioteca/estructuras.h>
#include <biblioteca/paquetes.h>
#define PATHCONFIG "./configuraciones/nodo.cfg"
#define recibirPaquetes true
#define TAMBLOQUE 1048576
//---------globales---------
char* rutaDatabin;
/**
 * @NAME: getBloque
 * @DESC: Retorna el numero del bloque pedido
 */
char* getBloque(int numBLoque);
/**
 * @NAME: setBloque
 * @DESC: escribe el numero del bloque pedido
 */
int setBloque(int numBLoque, char* bloque);
void recibirSolicitud(t_paquete * unPaquete, int * client_socket);
void gestionarSolicitudes(int, void (*)(void*, int*));
#endif /*DATANODE_H_*/
