/*
 ============================================================================

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
#include <signal.h>
#include <commons/config.h>
#include <biblioteca/estructurasMasterYama.h>
#include <biblioteca/estructurasWorkerMaster.h>

/*

Señales extraidas de /usr/bin/include/bits/signum.h



#define SIG_ERR	((__sighandler_t) -1)		/* Error return.
#define SIG_DFL	((__sighandler_t) 0)		/* Default action.
#define SIG_IGN	((__sighandler_t) 1)		/* Ignore signal.

/* Signals.
#define	SIGHUP		1	/* Hangup (POSIX).
#define	SIGINT		2	/* Interrupt (ANSI).
#define	SIGQUIT		3	/* Quit (POSIX).
#define	SIGILL		4	/* Illegal instruction (ANSI).
#define	SIGTRAP		5	/* Trace trap (POSIX).
#define	SIGABRT		6	/* Abort (ANSI).
#define	SIGIOT		6	/* IOT trap (4.2 BSD).
#define	SIGBUS		7	/* BUS error (4.2 BSD).
#define	SIGFPE		8	/* Floating-point exception (ANSI).
#define	SIGKILL		9	/* Kill, unblockable (POSIX).
#define	SIGUSR1		10	/* User-defined signal 1 (POSIX).
#define	SIGSEGV		11	/* Segmentation violation (ANSI).
#define	SIGUSR2		12	/* User-defined signal 2 (POSIX).
#define	SIGPIPE		13	/* Broken pipe (POSIX).
#define	SIGALRM		14	/* Alarm clock (POSIX).
#define	SIGTERM		15	/* Termination (ANSI).
#define	SIGSTKFLT	16	/* Stack fault.
#define	SIGCLD		SIGCHLD	/* Same as SIGCHLD (System V).
#define	SIGCHLD		17	/* Child status has changed (POSIX).
#define	SIGCONT		18	/* Continue (POSIX).
#define	SIGSTOP		19	/* Stop, unblockable (POSIX).
#define	SIGTSTP		20	/* Keyboard stop (POSIX).
#define	SIGTTIN		21	/* Background read from tty (POSIX).
#define	SIGTTOU		22	/* Background write to tty (POSIX).
#define	SIGURG		23	/* Urgent condition on socket (4.2 BSD).
#define	SIGXCPU		24	/* CPU limit exceeded (4.2 BSD).
#define	SIGXFSZ		25	/* File size limit exceeded (4.2 BSD).
#define	SIGVTALRM	26	/* Virtual alarm clock (4.2 BSD).
#define	SIGPROF		27	/* Profiling alarm clock (4.2 BSD).
#define	SIGWINCH	28	/* Window size change (4.3 BSD, Sun).
#define	SIGPOLL		SIGIO	/* Pollable event occurred (System V).
#define	SIGIO		29	/* I/O now possible (4.2 BSD).
#define	SIGPWR		30	/* Power failure restart (System V).
#define SIGSYS		31	/* Bad system call.
#define SIGUNUSED	31

*/

void leerConfiguracion();
void mandarRutaInicial(char* ruta);
void gestionarTransformacion(peticionDeTransformacion pedido[], int tam);
void pedir_transformacion(peticionDeTransformacion pedido[], int a);
void gestionarReduccionLocal(peticionDeReduccionLocal pedido[], int tam);
void gestionarReduccionGlobal(peticionDeReduccionGlobal pedido[], int tam);
void pedir_reduccion();
void signal_capturer(int numeroSeñal);
void dividirPorCero(); //para ejemplo de señales



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
	signal(SIGFPE,signal_capturer);

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

void gestionarReduccionGlobal(peticionDeReduccionGlobal pedido[], int tam){


}

void crearDAtosParaReduccionLocal (indicacionesParaReduccionLocal solicitud, int tam){}


void pedir_reduccion(){

}

void signal_capturer(int numeroSeñal){

	switch(numeroSeñal){
	case 8: printf("division por 0");
	break;
	}
}
void dividirPorCero(){
	int a = 4/0;
	return;
}
