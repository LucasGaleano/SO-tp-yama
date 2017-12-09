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

enum{
	Clock = 0,
	W_Clock,
};

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
void planificador_agregarWorker(t_list* tablaPlanificador, int numWorkerId); //check
void planificador_sacarWorker(t_list* tablaPlanificador, int numWorkerId);
void planificador(int algoritmo, t_list * listaDeBloques, t_list* tablaPlanificador, int DispBase);
void planificador_sumarWLWorker(t_list* tablaPlanificador, int numWorkerId, int WL);

#endif /* BALANCEADOR_H_ */
