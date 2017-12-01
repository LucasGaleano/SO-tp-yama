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




void Planificador(char* algoritmo, t_list listaDeBloques, int cantWorkers, t_tablaPlanificador* tablaPlanificador){


	int punteroClock=0;
	int cantBloques = list_size(listaDeBloques);
	t_list* secuenciaDePlanificador = list_create();
	t_registro_planificador* registro = malloc(t_registro_planificador);

	int contworkers=0;
	for(;contworkers < cantWorkers;contworkers++)
	{
		//Se calcularán los valores de disponibilidades para cada Worker
		//Se posicionará el Clock en el Worker de mayor disponibilidad, desempatando por el primer worker que tenga menor cantidad de tareas realizadas históricamente.
		t_worker* worker = worker_create();
		registro->worker->id = contworkers;
		registro->worker->weight = config->disponibilidad_base;
		list_add(tablaPlanificador->registros,registro);
	}
	int contBloques = 0;
	for(;contBloques<cantBloques;contBloques++){
		t_bloque* bloque = list_get(listaDeBloques,contBloques); // TRAE BLOQUES UNO POR UNO.

		t_worker* workerPuntero= list_get(tablaPlanificador->registros,punteroClock);

		if(estaBloqueEnWorker(bloque, workerPuntero) && workerPuntero->weight > 0 ){
			//Se deberá evaluar si el Bloque a asignar se encuentra en el Worker apuntado por el Clock y el mismo tenga disponibilidad mayor a 0.
			//Si se encuentra, se deberá reducir en 1 el valor de disponibilidad y avanzar el Clock al siguiente Worker.
			//Si dicho clock fuera a ser asignado a un Worker cuyo nivel de disponibilidad fuera 0,
			//se deberá restaurar la disponibilidad al valor de la disponibilidad base y luego, avanzar el clock al siguiente Worker.

			punteroClock = avanzarPuntero(punteroClock,list_size(tablaPlanificador->registros));

			t_registro_planificador* registro = malloc(sizeof(t_registro_planificador));
			registro = list_get(tablaPlanificador->registros,workerPuntero);
			registro->worker->weight--;
			list_add(registro->listaBloques,bloque);


		}
		else{//En el caso de que no se encuentre, se deberá utilizar el siguiente Worker que posea una disponibilidad mayor a 0.
			//Para este caso, no se deberá modificar el Worker apuntado por el Clock.

			int proximoWorker = avanzarPuntero(punteroClock,list_size(tablaPlanificador->registros));
			while (proximoWorker != punteroClock) {

				if(estaBloqueEnWorker(t_bloque,proximoWorker) && workerPuntero->weight > 0) {

				}else{
					proximoWorker = avanzarPuntero(proximoWorker,list_size(tablaPlanificador->registros));

				}

			}
			//Si se volviera a caer en el Worker apuntado por el clock por no existir disponibilidad en los Workers (es decir, da una vuelta completa a los Workers),
			//se deberá sumar al valor de disponibilidad de todos los workers el valor de la Disponibilidad Base configurado al inicio de la pre-planificación.

			if(proximoWorker == punteroClock){   //suma base a todos los workers;
				contworkers=0;
					for(;contworkers<cantWorkers;contworkers++)
					{
						t_registro_planificador registro = list_get(tablaPlanificador->registros,contworkers);
						registro->worker->weight += config->disponibilidad_base;
					}




			}



		}




	}

}









bool estaBloqueEnWorker(t_bloque bloque,int worker){

		char* ubic0 = string_new();
		char* ubic1 = string_new();

		ubic0 = string_substring(bloque->copia0->nodo,0,4);  //"Nodo1" devuelve "1"
		ubic1 = string_substring(bloque->copia1->nodo,0,4);
		if(strcmp(ubic0,string_itoa(worker)) || strcmp(ubic1,string_itoa(worker))){
			return true;
		}
	return false;


}

int avanzarPuntero(int puntero,int  n){

	puntero++;
	if(puntero>n)
		puntero=0;
	return puntero;

}






//Listas -----------------------------------





//todo worker_destroy

