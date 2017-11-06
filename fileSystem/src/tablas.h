#ifndef TABLAS_H_
#define TABLAS_H_

#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include <biblioteca/sockets.h>

/*---------	---------------Estructuras-------------------------*/
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
t_list * tablaSockets;
t_tabla_nodo * tablaNodos;
t_list * tablaDirectorios;

t_config * configTablaDirectorios;
t_config * configTablaNodo;
t_config * configTablaBitmap;

bool bitMapDirectorio[100];

/*-------------------------Tabla de directorios-------------------------*/
void 				crearTablaDirectorios			(char *);
void 				crearArchivoTablaDirectorios	(char *);
void 				agregarDirectorioTabla			(t_directory *);
void 				eliminarDirectorioTabla			(char *, int);

/*-------------------------Tabla de archivos-------------------------*/
void 				crearArchivoTablaArchivo		(char *, char *);

///*-------------------------Tabla de nodos-------------------------*/
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
char * 				armarRegistroDirectorio			(char *, int);
int 				buscarIndexLibre				(void);
int 				obtenerIndexPadre				(char *);
void 				llenarBitmap					(void);
void 				destruirSubstring				(char **);

#endif /* TABLAS_H_ */
