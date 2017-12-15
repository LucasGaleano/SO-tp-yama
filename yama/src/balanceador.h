#ifndef BALANCEADOR_H_
#define BALANCEADOR_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <biblioteca/estructuras.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include "string.h"
#include <stdint.h>
#include <commons/log.h>

typedef struct{
	int bloqueArchivo;
	t_list* nodosEnLosQueEsta;
} t_nodos_por_bloque;

typedef struct {
	int id;
	int disponibilidad;
	uint32_t WL_actual;
	uint32_t WL_Total;
	t_list* listaBloques; //lista de int.
} t_registro_Tabla_Planificador;



//-------------------------------------Funciones balanceador--------------------------------------//



void Planificador_destroy(t_list* planificador);
t_list* Planificador_create(); //check
void planificador_agregarWorker(t_list* tablaPlanificador, char* numWorkerId); //check
void planificador_sacarWorker(t_list* tablaPlanificador, char* numWorkerId);
bool planificador(char* algoritmo, t_list * listaDeBloques, t_list* tablaPlanificador, int DispBase);
void planificador_sumarWLWorker(t_list* tablaPlanificador, int numWorkerId, int WL);
int extraerIddelNodo(char* nodo);

char* obtenerEncargadoReduccionGlobal();

#endif /* BALANCEADOR_H_ */
