#ifndef TABLAS_H_
#define TABLAS_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <string.h>
#include <commons/temporal.h>


t_list* tabla_de_estados;


enum etapa{
	TRANSFORMACION,
	REDUCCION_LOCAL,
	REDUCCION_GLOBAL
};

enum estado{
	PROCESANDO,
	ERROR,
	FINALIZADO_OK
};

typedef struct {
	int job;
	int master;
	char * nodo;
	int bloque;
	int etapa;
	char * nombreArchivoTemporal;
	int estado;
} t_elemento_tabla_estado;

/*------------------------Manejo de tabla estado-------------------------*/
#include "tablas.h"


t_elemento_tabla_estado* crearRegistro(int job, int master, char * nombreNodo, int bloque, int etapa, char * nombreArchivoTemporal, int estado);

void agregarRegistro(int job, int master, char * nombreNodo, int bloque, int etapa, char * nombreArchivoTemporal, int estado);

t_elemento_tabla_estado* buscarRegistro(char* nombreNodo, int bloque, int etapa);

void modificarEstadoDeRegistro(char* nombreNodo, int bloque, int etapa, int nuevoEstado);

void eliminarRegistro(char* nombreNodo, int bloque, int etapa);



#endif /* TABLAS_H_ */
