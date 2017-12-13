//#ifndef DATANODE_H_
//#define DATANODE_H_
//#include <stdio.h>
//#include <stdlib.h>
//#include <commons/config.h> //config
//#include <commons/log.h> //log
//#include <commons/string.h>
//#include <fcntl.h> // O_RDONLY modo de abrir para el open()
//#include <sys/stat.h> //fstat()
//#include <sys/mman.h> //mmap()
//#include <unistd.h> //PROT_READ del mmap()
//#include <biblioteca/sockets.h>
////#include <stdbool.h>
//#include <biblioteca/estructuras.h>
//#include <biblioteca/paquetes.h>
//
//#define PATHCONFIG "/home/utnso/workspace/tp-2017-2c-NULL/configuraciones/nodo.cfg"
//
////---------globales---------
//t_log* logger;
//
//char* nombreNodo;
//char * rutaDatabin;
//char * ipNodo;
//char * puertoNodo;
//
//int socketFileSystem;
//bool recibirSolicitudes;
//int cantidadBloques;
//
//
///**
// * @NAME: getBloque
// * @DESC: Retorna el numero del bloque pedido
// */
//char* getBloque(int numBLoque);
///**
// * @NAME: setBloque
// * @DESC: escribe el numero del bloque pedido
// */
//int setBloque(int numBLoque, char* bloque);
//
///*------------------------------Procesar paquetes------------------------------*/
//void recibirSolicitud(t_paquete * unPaquete, int * client_socket);
//void procesarSolicitudLecturaBloque(t_paquete * unPaquete, int * client_socket);
//void procesarSolicitudEscrituraBloque(t_paquete * unPaquete,int * client_socket);
//void procesarError(t_paquete * unPaquete);
//void procesarSolicitudLecturaArchivoTemporal(t_paquete * unPaquete,int * client_socket);
//void procesarSolicitudLecturaBloqueGenerarCopia(t_paquete * unPaquete,int * client_socket);
//void procesarSolicitudInfoNodo(t_paquete * unPaquete, int * client_socket);
//void procesarSolicitudNombre(t_paquete * unPaquete, int * client_socket);
//
//#endif /*DATANODE_H_*/

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
#define PATHCONFIG "/home/utnso/workspace/tp-2017-2c-NULL/configuraciones/nodo.cfg"
#define recibirPaquetes true

//---------globales---------
char* rutaDatabin;
t_log* logger;
bool recibirSolicitudes;
int cantidadBloques;
char * nomNodo;

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

/*------------------------------Procesar paquetes------------------------------*/
void recibirSolicitud(t_paquete * unPaquete, int * client_socket);
void procesarSolicitudLecturaBloque(t_paquete * unPaquete, int * client_socket);
void procesarSolicitudEscrituraBloque(t_paquete * unPaquete,int * client_socket);
void procesarError(t_paquete * unPaquete);
void procesarSolicitudLecturaArchivoTemporal(t_paquete * unPaquete,int * client_socket);
void procesarSolicitudLecturaBloqueGenerarCopia(t_paquete * unPaquete,int * client_socket);
void procesarSolicitudInfoNodo(t_paquete * unPaquete, int * client_socket);
void procesarSolicitudNombre(t_paquete * unPaquete, int * client_socket);

#endif /*DATANODE_H_*/
