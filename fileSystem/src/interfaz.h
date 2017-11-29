#ifndef INTERFAZ_H_
#define INTERFAZ_H_

#include <stdbool.h>

#include "tablas.h"

t_list * listaTemporal;

//Tabla de sockets
typedef struct {
	char * archivo;
	int tamLectura;
	int bloqueAEscribir;
	char * nomNodo;
} t_hilo_enviar;

/*-------------------------Almacenar archivo-------------------------*/
void				almacenarArchivo				(char *, char *, char *, int);
void * 				dividirBloqueArchivoBinario		(void *, int *);
void * 				dividirBloqueArchivoTexto		(void *, int *);
char * 				buscarNodoMenosCargado			(void);
int 				buscarBloqueAEscribir			(char *);

/*-------------------------Leer archivo-------------------------*/
char* 				leerArchivo						(char *);
t_tarea * 			nodoMenosSaturado				(char **, char **);
int 				cantidadTareas					(char **);


void dividirBloqueArchivoTexto2(void * archivo, t_config * configTablaArchivo);
#endif /* INTERFAZ_H_ */
