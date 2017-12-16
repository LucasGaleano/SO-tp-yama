#ifndef TABLAS_H_
#define TABLAS_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <string.h>
#include <commons/temporal.h>
#include "balanceador.h"

t_list* tabla_de_estados;

enum etapa{
	TRANSFORMACION,
	REDUCCION_LOCAL,
	REDUCCION_GLOBAL
};


typedef struct {
	int job;
	int master;
	char * nodo;
	int bloque;
	int etapa;
	char * nombreArchivoTemporal;
	int estado;
	t_nodos_por_bloque* nodosPorBloque;
} t_elemento_tabla_estado;

/*------------------------Manejo de tabla estado-------------------------*/
#include "tablas.h"

t_elemento_tabla_estado * crearRegistro(int job, int master, char * nombreNodo, int bloque, int etapa, char * nombreArchivoTemporal, int estado, t_nodos_por_bloque* nodosPorBloque);

void agregarRegistro(int job, int master, char * nombreNodo, int bloque, int etapa, char * nombreArchivoTemporal, int estado, t_nodos_por_bloque* nodosPorBloque);

t_elemento_tabla_estado* buscarRegistro(int job, int master, char* nombreNodo, int bloque, int etapa,int estado, char* archivo);

void modificarEstadoDeRegistro(int job, int master, char* nombreNodo, int bloque, int etapa, int nuevoEstado);

void eliminarRegistro(int job, int master, char* nombreNodo, int bloque, int etapa);

t_elemento_tabla_estado* buscarRegistroPorNodoYBloque(int master, char* nombreNodo, int bloque, int etapa);

void modificarEstadoDeRegistroPorNodoYBloque(int master, char* nombreNodo, int bloque, int etapa, int nuevoEstado);

bool terminoUnNodoLaTransformacion(char* nomNodo,int etapa, int estado);

/*------------------------Auxiliares-------------------------*/

char* 				nombreArchivoTemp			(char* prefijo);


#endif /* TABLAS_H_ */
