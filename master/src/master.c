#include "master.h"

int main(int argc, char **argv) {

	inicializarVariablesGlobales();
	// LOG
	log_info(logMaster, "arranca proceso master");

	//Tiempo de ejecucion
	struct timeval tiempoInicio;
	struct timeval tiempoFin;
	float total;
	gettimeofday(&tiempoInicio, NULL);

	//Inicializacion de parametros del main y socket

	rutaScriptTransformador = argv [1];
	rutaScriptReductor = argv [2];
	rutaArchivoParaArrancar = argv [3];
	rutaParaAlmacenarArchivo = argv [4];
	conexionYama = leerConfiguracion();

	// Arranca logica del proceso

	signal(SIGFPE,signal_capturer);

	enviarMensaje(conexionYama,rutaArchivoParaArrancar);

	// Agarra solicitudes de Transformacion y de reduccion

	while(finDeSolicitudes == false)
	{
	gestionarSolicitudes(conexionYama, (void *) procesarPaquete);

	}

	// Espera que terminen todos los hilos de reduccion local y asi esperar la solicitud de reduccion global

	int i=0;

	for (; i<=tareasTotalesReduccionLocal; i++)
	{
		pthread_join(hilosReduccionLocal[i],NULL);
	}

	finDeSolicitudes = false;

	while(finDeSolicitudes == false)
	{
	gestionarSolicitudes(conexionYama, (void *) procesarPaquete);
	}

	// almacenado final
	gestionarSolicitudes(conexionYama, (void *) procesarPaquete);

	gettimeofday(&tiempoFin, NULL);

	calcularTiempoTotalTransformacion();
	calcularTiempoTotalReduccionLocal();
	total = (tiempoFin.tv_sec - tiempoInicio.tv_sec) *1000 + (tiempoFin.tv_usec - tiempoInicio.tv_usec) / 1000;

	tablaMetricas.tiempoTotal = total;
	tablaMetricas.promedioJobs = (tiempoReduccionGlobal + tiempoReduccionLocal + tiempoTransformacion) / 3;
	tablaMetricas.cantidadTareasTotalesReduccionLocal = tareasTotalesReduccionLocal;

	imprimirMetricas();
	liberarMemoria();

	log_destroy(logMaster);
	log_info(logMaster, "termino el proceso master");
	return EXIT_SUCCESS;

}

void inicializarVariablesGlobales()
{
	logMaster = log_create("/home/utnso/workspace/tp-2017-2c-NULL/master/log/", "master.log",false,LOG_LEVEL_ERROR);

	hilosReduccionLocal = malloc(sizeof(pthread_t));
	if (hilosReduccionLocal == NULL)
	{
		log_error(logMaster, "No hay memoria disponible");
		exit(EXIT_SUCCESS);
	}
	hilosTransformacion = malloc(sizeof(pthread_t));
	if (hilosReduccionLocal == NULL)
	{
		log_error(logMaster, "No hay memoria disponible");
		exit(EXIT_SUCCESS);
	}
	pedidosDeTransformacion = list_create();
	pedidosDeReduccionGlobal = list_create();
	tiemposReduccionLocal = list_create();
	tiemposTransformacion= list_create();
	tiempoTransformacion = 0;
	tiempoReduccionLocal = 0;
	finDeSolicitudes = false;
	tablaMetricas.cantidadFallosAlmacenamiento = 0;
	tablaMetricas.cantidadFallosReduccionLocal = 0;
	tablaMetricas.cantidadFallosReduccionGlobal = 0;
	tablaMetricas.cantidadFallosAlmacenamiento = 0;
	tareasTotalesReduccionLocal = 0;

}


int leerConfiguracion(){

	char* ruta = "/home/utnso/workspace/tp-2017-2c-NULL/configuraciones/master.cfg";
	t_config * config = config_create(ruta);

	char * puerto = config_get_string_value(config, "YAMA_PUERTO");
	printf("%s", puerto);
	char * ip = config_get_string_value(config, "YAMA_IP");
    printf("%s", ip);

    int socketYama = conectarCliente(ip, puerto, MASTER);

    return socketYama;
}

void imprimirMetricas()
{
	printf("El proceso Master finalizo con las siguientes metricas:\n");
	printf("cantidad tareas de transformacion : %d\n", tablaMetricas.cantidadTareasTotalesTransformacion);
	printf("cantidad tareas de Reduccion Local : %d\n", tablaMetricas.cantidadTareasTotalesReduccionLocal);
	printf("cantidad tareas de Reduccion Global : %d\n", tablaMetricas.cantidadTareasTotalesReduccionGlobal);
	printf("cantidad maxima de tareas de transformacion en paralelo : %d\n", tablaMetricas.cantMaximaTareasTransformacionParalelas);
	printf("cantidad maxima de tareas de reduccion local en paralelo : %d\n", tablaMetricas.cantMaximaTareasReduccionLocalParalelas);
	printf("tiempo total de ejecucion : %f\n", tablaMetricas.tiempoTotal);
	printf("tiempo promedio jobs: %f\n", tablaMetricas.promedioJobs);
	printf("cantidad de fallos en transformacion: %d\n", tablaMetricas.cantidadFallosTransformacion);
	printf("cantidad de fallos en reduccion local: %d\n", tablaMetricas.cantidadFallosReduccionLocal);
	printf("cantidad de fallos en reduccion local: %d\n", tablaMetricas.cantidadFallosReduccionGlobal);
	printf("cantidad de fallos en almacenamiento final: %d\n", tablaMetricas.cantidadFallosAlmacenamiento);


}

/*
bool chequearExistencia(char* array[],int tamanio, char* elemento){
	int contador = 0;
	bool encontroRepetido = false;
	while(contador<=tamanio){
		if(array[contador]==elemento){
			encontroRepetido = true;
		}
		contador++;
	}
	return encontroRepetido;

}
*/

void mandarDatosTransformacion(t_indicacionTransformacion * transformacion){

	    struct timeval t0;
		struct timeval t1;
		gettimeofday (&t0,NULL);

		pthread_mutex_lock (&mutexTareasTransformacionEnParalelo);
		tareasTransformacionEnParalelo ++;

		if(tareasTransformacionEnParalelo > tablaMetricas.cantMaximaTareasTransformacionParalelas)
		{
			tablaMetricas.cantMaximaTareasTransformacionParalelas = tareasTransformacionEnParalelo;
		}

		pthread_mutex_unlock (&mutexTareasTransformacionEnParalelo);


		int conexionWorker = conectarCliente(transformacion->ip,transformacion->puerto,WORKER);

		enviarIndicacionTransformacion(conexionWorker, transformacion);

		pthread_mutex_lock(&mutexErrorTransformacion);

		errorTransformacion = false;

		gestionarSolicitudes(conexionWorker, (void *) procesarPaquete);

		if(errorTransformacion)
		{
			tablaMetricas.cantidadFallosTransformacion ++;
			pthread_mutex_unlock(&mutexErrorTransformacion);
			enviarMensaje(conexionYama,ERROR_TRANSFORMACION);
			enviarIndicacionTransformacion(conexionYama,transformacion);
			log_error(logMaster, "Error en la transformacion, worker de conexion %s : %s", transformacion->ip, transformacion->puerto);

		} else
		{

			pthread_mutex_unlock(&mutexErrorTransformacion);
			log_info (logMaster, "una transformacion terminada");
			enviarMensaje(conexionYama, SALIOBIEN);
			enviarIndicacionTransformacion(conexionYama, transformacion);
		}

		pthread_mutex_lock (&mutexTareasTransformacionEnParalelo);
		tareasTransformacionEnParalelo --;
		pthread_mutex_unlock (&mutexTareasTransformacionEnParalelo);

		gettimeofday(t1,NULL);
		float tiempoTotal = (t1.tv_sec - t0.tv_sec) *1000 + (t1.tv_usec - t0.tv_usec) / 1000;

		list_add(tiemposTransformacion, &tiempoTotal);

		free(transformacion);

}

void mandarDatosReduccionLocal(t_indicacionReduccionLocal * reduccion)
	{
	    struct timeval t0;
		struct timeval t1;
		gettimeofday (&t0,NULL);

		t_pedidoReduccionLocal * pedido = malloc(sizeof(t_pedidoReduccionLocal));
		if(pedido == NULL)
		{
			printf("no hay memoria disponible");
			exit(EXIT_FAILURE);
		}

		pedido->archivoReduccionLocal = reduccion -> archivoTemporalReduccionLocal;
		pedido->archivoTransformacion = reduccion -> archivoTemporalTransformacion;
		int worker = conectarCliente(reduccion->ip,reduccion->puerto, WORKER);

		// gestion de tareas paralelas

		pthread_mutex_lock (&mutexTareasParalelasReduccionLocal);
		tareasReduccionLocalEnParalelo ++;

		if(tablaMetricas.cantMaximaTareasReduccionLocalParalelas < tareasReduccionLocalEnParalelo)
		{
			tablaMetricas.cantMaximaTareasReduccionLocalParalelas = tareasReduccionLocalEnParalelo;
		}

		pthread_mutex_unlock (&mutexTareasParalelasReduccionLocal);

		enviarSolicitudReduccionLocal(worker, pedido); // acá va el cambio de estructura

		pthread_mutex_lock(&mutexErrorReduccionLocal);

		errorReduLocal = false;

		gestionarSolicitudes(worker,(void*) procesarPaquete);

		if(errorReduLocal)
		{
			tablaMetricas.cantidadFallosReduccionLocal ++;
			pthread_mutex_unlock(&mutexErrorReduccionLocal);
			log_error(logMaster, "error en la reduccion local para el nodo de conexion %s : %s", reduccion->ip, reduccion->puerto);
			enviarIndicacionReduccionLocal(conexionYama,reduccion);
			enviarMensaje(conexionYama,ERROR_REDUCCION_LOCAL);
		} else
		{
			pthread_mutex_unlock(&mutexErrorReduccionLocal);
			log_info(logMaster, "reduccion local completada");
			enviarIndicacionReduccionLocal(conexionYama,reduccion);
			enviarMensaje(conexionYama,SALIOBIEN);
		}


		pthread_mutex_lock (&mutexTareasParalelasReduccionLocal);
		tareasReduccionLocalEnParalelo --;
		pthread_mutex_unlock (&mutexTareasParalelasReduccionLocal);

		gettimeofday(t1,NULL);
		float tiempoTotal = (t1.tv_sec - t0.tv_sec) *1000 + (t1.tv_usec - t0.tv_usec) / 1000;

		list_add(tiemposReduccionLocal, &tiempoTotal);

		free (pedido);
		free (reduccion);
	}


void gestionarReduccionGlobal()
	{
		log_info(logMaster, "arranca la reduccion global");
		struct timeval t0;
		struct timeval t1;
		gettimeofday (&t0,NULL);
		int cantidadSolicitudes = list_size(pedidosDeReduccionGlobal);
		int posActual = 0;
		t_pedidoReduccionGlobal * pedidos[cantidadSolicitudes];
		int conexionWorker;
		pthread_t hiloReduGlobal[cantidadSolicitudes] ;
		while(posActual < cantidadSolicitudes)
		{
			t_pedidoReduccionGlobal * pedido = malloc (sizeof(t_pedidoReduccionGlobal));
			if(pedido == NULL)
			{
				log_error(logMaster, "no hay memoria disponible");
				exit(EXIT_FAILURE);
			}
			t_indicacionReduccionGlobal * indicacion = list_get(pedidosDeReduccionGlobal,posActual);
			pedido->ArchivoResultadoReduccionGlobal = indicacion->archivoDeReduccionGlobal;
			pedido->archivoReduccionPorWorker = indicacion->archivoDeReduccionLocal;
			pedido->workerEncargdo = indicacion->encargado;
			pedidos[posActual] = pedido;

			if ( string_equals_ignore_case( pedido -> workerEncargdo , "1"))
			{
				conexionWorker= conectarCliente(indicacion->ip,indicacion->puerto, WORKER);
			}

			posActual ++;

		}

		posActual = 0;
		while(posActual < cantidadSolicitudes)
		{
			reduGlobal * reduccion = malloc (sizeof(reduGlobal));
			if(reduccion == NULL)
			{
				log_error(logMaster, "no hay memoria disponible");
				exit(EXIT_FAILURE);
			}
			reduccion->conexion = conexionWorker;
			reduccion->reduGlobal = pedidos[posActual];
			pthread_create(&hiloReduGlobal[posActual],NULL,(void*) mandarDatosReduccionGlobal, (void *) reduccion);
			posActual++;

		}

		int i = 0;
		for(; i < cantidadSolicitudes; i++)
		{
			pthread_join (hiloReduGlobal[i],NULL);
		}

		enviarMensaje(conexionYama, SALIOBIEN);
		gettimeofday(t1,NULL);

		tiempoReduccionGlobal = (t1.tv_sec - t0.tv_sec) *1000 + (t1.tv_usec - t0.tv_usec) / 1000;
		pthread_mutex_lock(&mutexReduccionGlobal);
		tablaMetricas.cantidadTareasTotalesReduccionGlobal = cantidadSolicitudes;
		pthread_mutex_unlock(&mutexReduccionGlobal);

	}

void mandarDatosReduccionGlobal(reduGlobal * reduccion)
{
	enviarSolicitudReduccionGlobal(reduccion->conexion,reduccion->reduGlobal);

	pthread_mutex_lock(&mutexErrorReduccionGlobal);

	errorReduGlobal = false;

	gestionarSolicitudes(reduccion->conexion, (void *) procesarPaquete);

	if (errorReduGlobal)
	{
		tablaMetricas.cantidadFallosReduccionGlobal ++;
		pthread_mutex_unlock(&mutexErrorReduccionGlobal);
		enviarMensaje(conexionYama,ERROR_REDUCCION_GLOBAL);
		log_error(logMaster, "Error en la reduccion, worker de conexion %d", reduccion->conexion);

	} else pthread_mutex_unlock(&mutexErrorReduccionGlobal);


	free(reduccion->reduGlobal);
	free(reduccion);
}


void gestionarAlmacenadoFinal(t_indicacionAlmacenadoFinal * indicacion)
{
	log_info (logMaster, "arranca el almacenado final");
	t_pedidoAlmacenadoFinal * solicitud = malloc (sizeof(t_pedidoAlmacenadoFinal));
	solicitud->archivoReduccionGlobal = indicacion->rutaArchivoReduccionGlobal; //hay que rajarle ip y puerto al pedido
	int conexionWorker = conectarCliente(indicacion->ip,indicacion->puerto,WORKER);
	enviarIndicacionAlmacenadoFinal(conexionWorker,indicacion);

	pthread_mutex_lock(&mutexErrorAlmacenamiento);

	errorAlmacenamiento = false;

	gestionarSolicitudes(conexionWorker,(void *) procesarPaquete);

	if (errorAlmacenamiento)
	{
		tablaMetricas.cantidadFallosAlmacenamiento ++;
		pthread_mutex_unlock(&mutexErrorAlmacenamiento);
		enviarMensaje(conexionYama,ERROR_ALMACENAMIENTO);
		enviarIndicacionAlmacenadoFinal(conexionYama,indicacion);
		log_error(logMaster, "Error en el almacenado final");
	}
	else
	{
		pthread_mutex_unlock(&mutexErrorAlmacenamiento);
		enviarMensaje(conexionYama, SALIOBIEN);
		enviarIndicacionAlmacenadoFinal(conexionYama,indicacion);
	}
	free(solicitud);
	free(indicacion);
}

void signal_capturer(int numeroSenial){

	switch (numeroSenial)
	{

		case 8:
			enviarMensaje(conexionYama,"division por 0");
			break;
		case 11:
			enviarMensaje(conexionYama,"Segmentation fault");
			break;
		case 16:
			enviarMensaje(conexionYama,"Stack Overflow");
			break;
		default:
			break;

	}

	return;
}


void procesarPaquete(t_paquete * unPaquete, int * client_socket) { // contesto a *client_socket


	switch (unPaquete->codigoOperacion) {


	case ENVIAR_INDICACION_TRANSFORMACION:
		 ;
		log_info(logMaster,"LLega solicitud de trasnformacion");


		t_indicacionTransformacion * indicacionTransformacion = recibirIndicacionTransformacion(unPaquete);
		pthread_mutex_lock(&mutexTransformaciones);
		tareasTotalesTransformacion ++;
		pthread_mutex_unlock(&mutexTransformaciones);

		hilosTransformacion = realloc(&hilosReduccionLocal, sizeof(pthread_t) * tareasTotalesTransformacion);
	    pthread_create(&hilosTransformacion[tareasTotalesTransformacion-1],NULL, (void *) mandarDatosTransformacion, (void*) indicacionTransformacion);

		break;


	case ENVIAR_INDICACION_REDUCCION_LOCAL:

		 ;

		log_info(logMaster,"LLega solicitud de reduccion local");
		pthread_mutex_lock(&mutexReduLocal);
		tareasTotalesReduccionLocal ++;
		pthread_mutex_unlock(&mutexReduLocal);

		t_indicacionReduccionLocal * indicacionReduLocal = recibirIndicacionReduccionLocal(unPaquete);
		hilosReduccionLocal = realloc(&hilosReduccionLocal, sizeof(pthread_t) * tareasTotalesReduccionLocal);
		pthread_create(&hilosReduccionLocal[tareasTotalesReduccionLocal-1],NULL, (void *) mandarDatosReduccionLocal, (void*) indicacionReduLocal);

		break;


	case ENVIAR_INDICACION_REDUCCION_GLOBAL:
	 ;
	    log_info(logMaster,"LLega solicitud de Reduccion global");
		t_indicacionReduccionGlobal * indicacionesParaReduccionGlobal = recibirIndicacionReduccionGlobal(unPaquete);
	    list_add(pedidosDeReduccionGlobal, indicacionesParaReduccionGlobal);
		break;


	case ENVIAR_INDICACION_ALMACENADO_FINAL:
		 ;
		 log_info(logMaster,"LLega solicitud de almacenado final");
		 t_indicacionAlmacenadoFinal * indicacionAlmacenamientoFinal = recibirIndicacionAlmacenadoFinal(unPaquete);
		 gestionarAlmacenadoFinal(indicacionAlmacenamientoFinal);
		 break;


	case ENVIAR_MENSAJE:
		  ;
		 char* mensaje = string_new();
		 string_append(&mensaje ,recibirMensaje(unPaquete));

		 if(string_equals_ignore_case(mensaje,FIN)) // me odio por esto
		 {
			 finDeSolicitudes = true;

		 } else

			 if(string_equals_ignore_case(mensaje,ERROR_TRANSFORMACION))
			 {
				 errorTransformacion = true;

			 } else

			     if(string_equals_ignore_case(mensaje,ERROR_REDUCCION_LOCAL))
			     {
			    	 errorReduLocal = true;

			     } else

			    	 if(string_equals_ignore_case(mensaje,ERROR_REDUCCION_GLOBAL))
			    	 {
			    		 errorReduGlobal = true;
			    	 } else

			    		 if(string_equals_ignore_case(mensaje,ERROR_ALMACENAMIENTO))
			    		 {
			    			 errorAlmacenamiento = true;
			    		 }

		 free(mensaje);
		 break;


	default:

		 break;
	}

	destruirPaquete(unPaquete);
}


void calcularTiempoTotalTransformacion()
{
	int i = 0;
	for (; i< list_size(tiemposTransformacion);i++)
	{
		float * a = list_get(tiemposTransformacion, i);
		tiempoTransformacion = tiempoTransformacion + *a;
	}
}

void calcularTiempoTotalReduccionLocal()
{

	int i = 0;
	for (; i< list_size(tiemposReduccionLocal);i++)
	{
		float * a = list_get(tiemposReduccionLocal, i);
		tiempoReduccionLocal = tiempoReduccionLocal + *a;
	}
}
void liberarMemoria() {

	list_destroy_and_destroy_elements(pedidosDeReduccionGlobal, (void *) free);
	list_destroy_and_destroy_elements(tiemposReduccionLocal, (void *) free);
	list_destroy_and_destroy_elements(tiemposTransformacion, (void *) free);
	int i=0;

	for(; i<= tablaMetricas.cantidadTareasTotalesReduccionLocal;i++)
	{
		free(&hilosReduccionLocal[i]);
	}

	free(hilosReduccionLocal);
	int j = 0;

	for(;j<tareasTotalesTransformacion; j++)
	{
		free(&hilosTransformacion[j]);
	}

	free(hilosTransformacion);
}

