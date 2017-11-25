#include "yama.h"
#include "balanceador.h"
#include <commons/collections/list.h>

//La función de Availability de un Worker, definida como A(w) responderá a lo siguiente:
//
//A(w) = Base + PWL(w)
//
//Siendo PWL(w)=0 bajo el algoritmo Clock y, bajo el algoritmo W-Clock, será la disponibilidad del Worker en función de su carga de trabajo actual. En este segundo caso, responderá a la fórmula:
//
//PWL(w) = WLmax - WL(w)
//
//Siendo WLmax: la mayor carga de trabajo existente entre todos Workers WL(w): la carga de trabajo actual del Worker.
//
//Nota: La cargas de trabajo deberán corresponder a un número entero sin signar de 32bits.

typedef struct {
	bool puntero;
	int disponibilidad;
	char * worker;
	t_list * bloques;
} t_elemento_tabla_jobs;

typedef struct {
	char * archivo;
	t_list * bloques;
} t_elemento_archivo_bloque;

int iniciarPlanificador(char * algoritmo) {

	t_list * tabla_jobs = list_create();

	t_elemento_tabla_jobs * elemento1 = malloc(sizeof(t_elemento_tabla_jobs));

	t_elemento_tabla_jobs * elemento2 = malloc(sizeof(t_elemento_tabla_jobs));

	//Completo y agrego el primer elemento
	elemento1->puntero = true;
	elemento1->disponibilidad = 2;
	elemento1->worker = "w1";
	int num1 = 2;
	list_add(elemento1->bloques, &num1 );

	list_add(tabla_jobs, elemento1);

	//completo y agrego el segundo elemento
	elemento2->puntero = false;
	elemento2->disponibilidad = 2;
	elemento2->worker = "w2";
	int num2 = 2;
	list_add(elemento2->bloques, &num2);

	list_add(tabla_jobs, elemento2);

	planificar(tabla_jobs);


	return 0;
}


void planificar(t_list * tabla_jobs){
	void modificador(t_elemento_tabla_jobs){

	}

	list_iterate(tabla_jobs, (void*)modificador);
}

