#include "balanceador.h"

static int PosicionarClockMayorDisp(t_list* tablaPlanificador);
static void CalcularDisponibilidadCadaWorkerW_Clock(t_list* tablaPlanificador, int DispBase);
static void CalcularDisponibilidadCadaWorkerClock(t_list* tablaPlanificador, int DispBase);   //check
static int avanzarPuntero(int puntero, int n);
static void SumarDisponibilidad(t_registro_Tabla_Planificador* registroWorker, int sub);
static void SumarDisponibilidadTodos(t_list* tablaPlanificador, int n);
static bool estaBloqueEnWorker(t_bloque* bloque, int worker);  //check
static void insertarBloqueEnRegistroWorker(t_list* tablaPlanificador,int Worker,int numBloque);

void planificador(int algoritmo, t_list * listaDeBloques,
		t_list* tablaPlanificador, int DispBase) {

	t_registro_Tabla_Planificador* RegistroWorker;
	int cantBloques = list_size(listaDeBloques);
	int cantRegistros = list_size(tablaPlanificador);
	int proximoWorker=0;

	//Se calcularán los valores de disponibilidades para cada Worker
	switch (algoritmo){

	case Clock:
		CalcularDisponibilidadCadaWorkerClock(tablaPlanificador, DispBase);
		break;

	case W_Clock:
		CalcularDisponibilidadCadaWorkerW_Clock(tablaPlanificador, DispBase);
		break;
	}

	//Se posicionará el Clock en el Worker de mayor disponibilidad, desempatando por el primer worker que tenga menor cantidad de tareas realizadas históricamente.
	int punteroClock = PosicionarClockMayorDisp(tablaPlanificador);

	int numBloque = 0;
	bool EstaNodo = true;
	for (; numBloque < cantBloques; numBloque++) {


		t_bloque* bloqueAsignar = list_get(listaDeBloques, numBloque); // TRAE BLOQUES UNO POR UNO.

		RegistroWorker = list_get(tablaPlanificador, punteroClock);

		//Se deberá evaluar si el Bloque a asignar se encuentra en el Worker apuntado por el Clock y el mismo tenga disponibilidad mayor a 0.
		//Si se encuentra, se deberá reducir en 1 el valor de disponibilidad y avanzar el Clock al siguiente Worker.
		//Si dicho clock fuera a ser asignado a un Worker cuyo nivel de disponibilidad fuera 0,
		//se deberá restaurar la disponibilidad al valor de la disponibilidad base y luego, avanzar el clock al siguiente Worker.

		if (estaBloqueEnWorker(bloqueAsignar, RegistroWorker->id)
				&& RegistroWorker->disponibilidad > 0) {

			insertarBloqueEnRegistroWorker(tablaPlanificador,RegistroWorker->id,numBloque);
			EstaNodo=true;
			punteroClock = avanzarPuntero(punteroClock, cantRegistros);

			RegistroWorker = list_get(tablaPlanificador, punteroClock);
			if (RegistroWorker->disponibilidad == 0) {
				RegistroWorker->disponibilidad = DispBase;
			}

		} else {
			//En el caso de que no se encuentre, se deberá utilizar el siguiente Worker que posea una disponibilidad mayor a 0.
			//Para este caso, no se deberá modificar el Worker apuntado por el Clock.
			//Si se volviera a caer en el Worker apuntado por el clock por no existir disponibilidad en los Workers (es decir, da una vuelta completa a los Workers),
			//se deberá sumar al valor de disponibilidad de todos los workers el valor de la Disponibilidad Base configurado al inicio de la pre-planificación.

			proximoWorker = avanzarPuntero(punteroClock, cantRegistros);
			while (proximoWorker != punteroClock) {
				RegistroWorker = list_get(tablaPlanificador, proximoWorker);

				if (estaBloqueEnWorker(bloqueAsignar, proximoWorker) && RegistroWorker->disponibilidad > 0) {
					insertarBloqueEnRegistroWorker(tablaPlanificador,proximoWorker,numBloque);
					EstaNodo=true;
					break;

				} else {
					proximoWorker = avanzarPuntero(proximoWorker,
							cantRegistros);
				}

			}

			if (proximoWorker == punteroClock) { //suma base a todos los workers;
				SumarDisponibilidadTodos(tablaPlanificador, DispBase);

				if(EstaNodo==true){
					numBloque--; //vuelvo a probar el mismo bloque}

				}
				if(!EstaNodo){
					char* msg = malloc(100);
					sprintf(msg,"imposible ubicar el nodo que contiene al bloque: %d, terminando proceso\n",numBloque);
					perror(msg);
					free(msg);
					exit(EXIT_FAILURE);
				}
				EstaNodo=false;

				}

			}

		}

} //END


static void insertarBloqueEnRegistroWorker(t_list* tablaPlanificador,int Worker,int numBloque){

	t_registro_Tabla_Planificador* registro = list_get(tablaPlanificador,Worker);
	list_add(registro->listaBloques,numBloque);
	registro->WL_actual++;
	registro->WL_Total++;
	SumarDisponibilidad(registro,-1);


}

static void SumarDisponibilidadTodos(t_list* tablaPlanificador, int n) {

	int contworkers = 0;
	int cantWorkers = list_size(tablaPlanificador);
	for (; contworkers < cantWorkers; contworkers++) {

		t_registro_Tabla_Planificador* registro = list_get(tablaPlanificador,
				contworkers);
		SumarDisponibilidad(registro,n);

	}

}

static void SumarDisponibilidad(t_registro_Tabla_Planificador* registroWorker, int n) {

	registroWorker->disponibilidad = registroWorker->disponibilidad + n;

}

static bool estaBloqueEnWorker(t_bloque* bloque, int worker) {

	char* ubic0 = malloc(string_length(bloque->copia0->nodo) );
	char* ubic1 = malloc(string_length(bloque->copia0->nodo) );
	strcpy(ubic0, bloque->copia0->nodo);
	strcpy(ubic1, bloque->copia1->nodo);

	ubic0 = string_substring(ubic0, 4, 1); //ejemlo "Nodo1" devuelve "1"
	ubic1 = string_substring(ubic1, 4, 1);

	if ( (!strcmp(string_itoa(worker),ubic0)) || (!strcmp(string_itoa(worker),ubic1)) ) {
		free(ubic0);
		free(ubic1);
		return true;
	}
	free(ubic0);
	free(ubic1);
	return false;

}

static int avanzarPuntero(int puntero, int n) {

	puntero++;
	if (puntero >= n)
		puntero = 0;
	return puntero;
}

void planificador_agregarWorker(t_list* tablaPlanificador, int numWorkerId) {

	t_registro_Tabla_Planificador* registro = malloc(
			sizeof(t_registro_Tabla_Planificador));

	registro->id = numWorkerId;
	registro->disponibilidad = 0;
	registro->WL_Total = 0;
	registro->WL_actual = 0;
	registro->listaBloques = list_create();
	list_add(tablaPlanificador, registro);
}

void planificador_sacarWorker(t_list* tablaPlanificador, int numWorkerId){

	int contworkers = 0;
	int cantWorkers = list_size(tablaPlanificador);
	for (; contworkers < cantWorkers; contworkers++) {

		t_registro_Tabla_Planificador* registro = list_get(tablaPlanificador, contworkers);
		if(registro->id == numWorkerId){

			list_remove(tablaPlanificador, contworkers);
			cantWorkers--;
		}
	}
}

t_list* Planificador_create() {

	t_list* nuevaTabla = list_create();

	return nuevaTabla;
}

void Planificador_destroy(t_list* planificador) {

	int cont = 0;
	int cantElementos = list_size(planificador);
	for (; cont < cantElementos; cont++) {
		t_registro_Tabla_Planificador* registro = list_get(planificador, cont);
		int j = 0;
		int elementos = list_size(registro->listaBloques);
		for (; j < elementos; j++) {
			t_registro_Tabla_Planificador* registro = list_get(planificador, j);
			list_destroy(registro->listaBloques);
		}

	}
	list_destroy(planificador);

}

static void CalcularDisponibilidadCadaWorkerClock(t_list* tablaPlanificador,
		int DispBase) {

	int contworkers = 0;
	int cantWorkers = list_size(tablaPlanificador);
	for (; contworkers < cantWorkers; contworkers++) {

		t_registro_Tabla_Planificador* registro = list_get(tablaPlanificador,
				contworkers);
		registro->disponibilidad = DispBase;

	}

}

static void CalcularDisponibilidadCadaWorkerW_Clock(t_list* tablaPlanificador,
		int DispBase) {

	int contworkers = 0;
	int cantWorkers = list_size(tablaPlanificador);
	uint32_t WLmax = 0;
	for (; contworkers < cantWorkers; contworkers++) {
		t_registro_Tabla_Planificador* registro = list_get(tablaPlanificador,
				contworkers);
		uint32_t WL = registro->WL_actual;
		if (WLmax < WL)
			WLmax = WL;
	}

	contworkers = 0;
	for (; contworkers < cantWorkers; contworkers++) {

		t_registro_Tabla_Planificador* registro = list_get(tablaPlanificador,
				contworkers);
		uint32_t PWL = WLmax - registro->WL_actual;
		registro->disponibilidad = DispBase + PWL;

	}

}

void planificador_sumarWLWorker(t_list* tablaPlanificador, int numWorkerId, int WL){

	int contworkers = 0;
	int cantWorkers = list_size(tablaPlanificador);
	for (; contworkers < cantWorkers; contworkers++) {

		t_registro_Tabla_Planificador* registro = list_get(tablaPlanificador, contworkers);
		if(registro->id == numWorkerId){
			registro->WL_actual += WL;

		}
	}
}

static int PosicionarClockMayorDisp(t_list* tablaPlanificador) {

	int contworkers = 0;
	int cantWorkers = list_size(tablaPlanificador);
	int clockP = -1;
	int maxDisp = 0;
	int WLtotal = 0;
	for (; contworkers < cantWorkers; contworkers++) {

		t_registro_Tabla_Planificador* registro = list_get(tablaPlanificador,
				contworkers);

		if (maxDisp == registro->disponibilidad && WLtotal > registro->WL_Total) {
			maxDisp = registro->disponibilidad;
			WLtotal = registro->WL_Total;
			clockP += 1;
		}

		if (maxDisp < registro->disponibilidad) {
			maxDisp = registro->disponibilidad;
			WLtotal = registro->WL_Total;
			clockP += 1;
		}



	}
	if (clockP == -1){
		return 0;
	}
	return clockP;
}
