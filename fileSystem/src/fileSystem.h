#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <biblioteca/sockets.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "consola.h"

/*---------	---------------Estructuras-------------------------*/
#define PUERTO_FILESYSTEM "3200"

//Tabla de sockets
typedef struct {
	int socket;
	char * nombre;
} t_tabla_sockets;


//Tabla de nodos
typedef struct {
	int tamanio;
	int libres;
	t_list * nomNodos; //Lista de nombres de los nodos
	t_list * infoDeNodo; //Lista con cada nodo en particular
} t_tabla_nodo;

//Registro tabla de Directorios
typedef struct {
	int index;
	char nombre[255];
	int padre;
} t_directory;


/*------------------------Variables globales-------------------------*/
t_log* logFileSystem;

t_list * tablaSockets;
t_tabla_nodo * tablaNodos;
t_list * tablaDirectorios;

t_config * configTablaDirectorios;
t_config * configTablaNodo;
t_config * configTablaBitmap;

/*------------------------Procesamiento paquetes-------------------------*/
void 				procesarPaquete					(t_paquete *, int *);
void 				recibirHandshake				(t_paquete *, int *);
void 				recibirInfoNodo					(t_paquete *, int);
void 				recibirError					(t_paquete *);

/*-------------------------Tabla de directorios-------------------------*/
void 				crearTablaDirectorios			(char *);
void 				crearArchivoTablaDirectorios	(char *);
void 				agregarDirectorioTabla			(t_directory *);
void 				eliminarDirectorioTabla			(char *, int);
void 				persistirTablaDirectorios		(void);

/*-------------------------Tabla de nodos-------------------------*/
void	 			crearTablaNodos					(char *);
void 				crearArchivoTablaNodos			(char *);
void 				agregarNodoTablaNodos			(t_nodo_info *);
void 				eliminarNodoTablaNodos			(char *);
void 				persistirTablaNodos				(void);

/*-------------------------Tabla de sockets-------------------------*/
void 				crearTablaSockets				(void);
void 				agregarNodoTablaSockets			(char *, int);
char * 				eliminarNodoTablaSockets		(int);

/*-------------------------Tabla de Bitmap-------------------------*/
void 				crearArchivoTablaBitmap			(t_nodo_info *);

/*-------------------------Funciones auxiliares-------------------------*/
void 				iniciarServidor					(char*);
void 				mostrarTablas					(void);


#endif /* FILESYSTEM_H_ */
