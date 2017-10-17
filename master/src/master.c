/*
 ============================================================================
 /*
 ============================================================================
 Name        : master.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <commons/config.h>
#include "estructurasWorkerMaster.h"
#include "estructurasMasterYama.h"


void leerConfiguracion();
void mandarRutaInicial(char* ruta);
void gestionarTransformacion(peticionDeTransformacion pedido[], int tam);
void pedir_transformacion(peticionDeTransformacion pedido[], int a);
void gestionarReduccionLocal(peticionDeReduccionLocal pedido[], int tam);
void pedir_reduccion();



// Resultado de operaciones

struct metricas {
	float tiempoTotal;
	float promedioJobs;
	unsigned short int tareasEnParalelo;
	unsigned short int tareasPrincipalesDelJob;
	unsigned short int fallosDelJob;

};




int main(void) {

	leerConfiguracion();
	peticionDeReduccionLocal reduLo[5];
	mandarRutaInicial("/home/texto1.txt");
	int tam = 3;
	peticionDeTransformacion pedido[tam];
	pedido[0].archivoTemporal = "sldklskd";
	pedido[0].direccion= "putoElQueLee";
	pedido[0].worker="laburante";
	pedido[1].archivoTemporal = "sldklskd";
	pedido[1].direccion= "putoElQueLee";
	pedido[1].worker="laburante";
	pedido[2].archivoTemporal = "sldklskd";
	pedido[2].direccion= "putoElQueLee";
	pedido[2].worker="laburante";

	gestionarTransformacion(pedido, tam);
	gestionarReduccionLocal(reduLo, 5);

*/
	return EXIT_SUCCESS;

}


void leerConfiguracion(){

	char* ruta = "/home/utnso/workspace/tp-2017-2c-NULL/configuraciones/master.cfg";
	t_config * config = config_create(ruta);

	int puerto = config_get_int_value(config, "YAMA_PUERTO");
	printf("%d", puerto);
	int ip = config_get_int_value(config, "YAMA_IP");
    printf("%d", ip);
}



void mandarRutaInicial(char* ruta){

}



void gestionarTransformacion(peticionDeTransformacion pedido[], int tam){


	pthread_t nuevoHilo;
	int i;
	for (i= 0; i<tam; i++){
	pthread_create(&nuevoHilo,NULL, (void*) pedir_transformacion, (&pedido,i));
	}
	pthread_join(nuevoHilo, NULL);

}

void pedir_transformacion(peticionDeTransformacion pedido[], int numeroHilo){


	printf("me llego");



	printf ("%s",pedido[numeroHilo].archivoTemporal);
	printf("%s",pedido[numeroHilo].direccion);
	printf("%s",pedido[numeroHilo].worker);
	printf("%d",numeroHilo);

}


void gestionarReduccionLocal(peticionDeReduccionLocal pedido[], int tam){
	int aux;
	indicacionesParaReduccionLocal solicitud[aux];
	crearDatosParaReduccionLocal(&solicitud, aux);
	pthread_t hiloInformar;
	char* archivo;
	int i;
	for (i = 0; i<aux ; i++){
		if (archivo != solicitud[i].nodo){
			pthread_create(&hiloInformar, NULL,(void *) pedir_reduccion,NULL);
			archivo = solicitud[i].nodo;
		}
	pthread_join(hiloInformar,NULL);
	}


}

void crearDAtosParaReduccionLocal (indicacionesParaReduccionLocal solicitud[], int tam){}


void pedir_reduccion(){



}

