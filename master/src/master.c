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

	rutaScriptTransformador = argv[1];
	rutaScriptReductor = argv[2];
	rutaArchivoParaArrancar = argv[3];
	rutaParaAlmacenarArchivo = argv[4];
	conexionYama = leerConfiguracion();

	// Arranca logica del proceso

	signal(SIGFPE, signal_capturer);

	enviarRutaParaArrancarTransformacion(conexionYama,rutaArchivoParaArrancar);

	// Agarra solicitudes de Transformacion y de reduccion

	while (finDeSolicitudes == false) {
		gestionarSolicitudes(conexionYama, (void *) procesarPaquete);

	}

	// Espera que terminen todos los hilos de reduccion local y asi esperar la solicitud de reduccion global

	int i = 0;

	for (; i <= tareasTotalesReduccionLocal; i++) {
		pthread_join(hilosReduccionLocal[i], NULL);
	}

	finDeSolicitudes = false;

	while (finDeSolicitudes == false) {
		gestionarSolicitudes(conexionYama, (void *) procesarPaquete);
	}

	if (errorReduGlobal == false) {
		// almacenado final
		gestionarSolicitudes(conexionYama, (void *) procesarPaquete);
	}

	gettimeofday(&tiempoFin, NULL);

	calcularTiempoTotalTransformacion();
	calcularTiempoTotalReduccionLocal();
	total = (tiempoFin.tv_sec - tiempoInicio.tv_sec) * 1000
			+ (tiempoFin.tv_usec - tiempoInicio.tv_usec) / 1000;

	tablaMetricas.tiempoTotal = total;
	tablaMetricas.promedioJobs = (tiempoReduccionGlobal + tiempoReduccionLocal
			+ tiempoTransformacion) / 3;
	tablaMetricas.cantidadTareasTotalesReduccionLocal =
			tareasTotalesReduccionLocal;

	imprimirMetricas();
	liberarMemoria();

	log_destroy(logMaster);
	log_info(logMaster, "termino el proceso master");
	return EXIT_SUCCESS;

}

void inicializarVariablesGlobales()
{
	logMaster = log_create("master.log", "master",false,LOG_LEVEL_ERROR);

	hilosReduccionLocal = malloc(sizeof(pthread_t));
	if (hilosReduccionLocal == NULL) {
		log_error(logMaster, "No hay memoria disponible");
		exit(EXIT_SUCCESS);
	}
	hilosTransformacion = malloc(sizeof(pthread_t));
	if (hilosReduccionLocal == NULL) {
		log_error(logMaster, "No hay memoria disponible");
		exit(EXIT_SUCCESS);
	}
	pedidosDeTransformacion = list_create();
	indicacionesDeReduccionGlobal = list_create();
	tiemposReduccionLocal = list_create();
	tiemposTransformacion = list_create();
	tiempoTransformacion = 0;
	tiempoReduccionLocal = 0;
	finDeSolicitudes = false;
	tablaMetricas.cantidadFallosAlmacenamiento = 0;
	tablaMetricas.cantidadFallosReduccionLocal = 0;
	tablaMetricas.cantidadFallosReduccionGlobal = 0;
	tablaMetricas.cantidadFallosAlmacenamiento = 0;
	tareasTotalesReduccionLocal = 0;

}

int leerConfiguracion() {

	char* ruta =
			"/home/utnso/workspace/tp-2017-2c-NULL/configuraciones/master.cfg";
	t_config * config = config_create(ruta);

	char * puerto = config_get_string_value(config, "YAMA_PUERTO");
	printf("%s", puerto);
	char * ip = config_get_string_value(config, "YAMA_IP");
	printf("%s", ip);

	int socketYama = conectarCliente(ip, puerto, MASTER);

    return socketYama;

    free(ip);
    free(puerto);
}

void imprimirMetricas() {
	printf("El proceso Master finalizo con las siguientes metricas:\n");
	printf("cantidad tareas de transformacion : %d\n",
			tablaMetricas.cantidadTareasTotalesTransformacion);
	printf("cantidad tareas de Reduccion Local : %d\n",
			tablaMetricas.cantidadTareasTotalesReduccionLocal);
	printf("cantidad tareas de Reduccion Global : %d\n",
			tablaMetricas.cantidadTareasTotalesReduccionGlobal);
	printf("cantidad maxima de tareas de transformacion en paralelo : %d\n",
			tablaMetricas.cantMaximaTareasTransformacionParalelas);
	printf("cantidad maxima de tareas de reduccion local en paralelo : %d\n",
			tablaMetricas.cantMaximaTareasReduccionLocalParalelas);
	printf("tiempo total de ejecucion : %f\n", tablaMetricas.tiempoTotal);
	printf("tiempo promedio jobs: %f\n", tablaMetricas.promedioJobs);
	printf("cantidad de fallos en transformacion: %d\n",
			tablaMetricas.cantidadFallosTransformacion);
	printf("cantidad de fallos en reduccion local: %d\n",
			tablaMetricas.cantidadFallosReduccionLocal);
	printf("cantidad de fallos en reduccion local: %d\n",
			tablaMetricas.cantidadFallosReduccionGlobal);
	printf("cantidad de fallos en almacenamiento final: %d\n",
			tablaMetricas.cantidadFallosAlmacenamiento);

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

void mandarDatosTransformacion(t_indicacionTransformacion * transformacion) {

	struct timeval t0;
	struct timeval t1;
	gettimeofday(&t0, NULL);

	t_pedidoTransformacion * pedido = malloc(sizeof(t_pedidoTransformacion));
	if (pedido == NULL) {
		log_error(logMaster, "no hay memoria suficiente");
		exit(EXIT_SUCCESS);
	}

	return encontroRepetido;
}
*/

	pthread_mutex_unlock(&mutexTareasTransformacionEnParalelo);

	int conexionWorker = conectarCliente(transformacion->ip,
			transformacion->puerto, WORKER);

		t_pedidoTransformacion * pedido = malloc(sizeof(t_pedidoTransformacion));
		if(pedido == NULL){
			log_error(logMaster, "no hay memoria suficiente");
			exit(EXIT_SUCCESS);
		}
		pedido->rutaArchivoTemporal = calloc(string_length(transformacion->rutaArchivoTemporal),sizeof(char));
		if(pedido->rutaArchivoTemporal == NULL)
		{
			log_error(logMaster, "no hay memoria suficiente");
			exit(EXIT_SUCCESS);
		}
		pedido->rutaArchivoTemporal = transformacion->rutaArchivoTemporal;

		pedido->rutaScriptTransformacion= calloc(string_length(rutaScriptTransformador),sizeof(char));
		if(pedido->rutaScriptTransformacion == NULL)
		{
			log_error(logMaster, "no hay memoria suficiente");
			exit(EXIT_SUCCESS);
		}
		pedido->rutaScriptTransformacion = rutaScriptTransformador;

		pedido->cantBytes = transformacion->bytes;
		pedido->numBloque = transformacion->bloque;
		pthread_mutex_lock (&mutexTareasTransformacionEnParalelo);
		tareasTransformacionEnParalelo ++;

	errorTransformacion = false;

	gestionarSolicitudes(conexionWorker, (void *) procesarPaquete);

	if (errorTransformacion) {
		tablaMetricas.cantidadFallosTransformacion++;
		pthread_mutex_unlock(&mutexErrorTransformacion);
		enviarMensaje(conexionYama, ERROR_TRANSFORMACION);
		enviarIndicacionTransformacion(conexionYama, transformacion);
		log_error(logMaster,
				"Error en la transformacion, worker de conexion %s : %s",
				transformacion->ip, transformacion->puerto);

	} else {
		pthread_mutex_unlock(&mutexErrorTransformacion);
		log_info(logMaster, "una transformacion terminada");
		enviarMensaje(conexionYama, SALIOBIEN);
		enviarIndicacionTransformacion(conexionYama, transformacion);
	}

	pthread_mutex_lock(&mutexTareasTransformacionEnParalelo);
	tareasTransformacionEnParalelo--;
	pthread_mutex_unlock(&mutexTareasTransformacionEnParalelo);

	gettimeofday(t1, NULL);
	float tiempoTotal = (t1.tv_sec - t0.tv_sec) * 1000
			+ (t1.tv_usec - t0.tv_usec) / 1000;

	list_add(tiemposTransformacion, &tiempoTotal);

		if(errorTransformacion)
		{
			tablaMetricas.cantidadFallosTransformacion ++;
			pthread_mutex_unlock(&mutexErrorTransformacion);
			enviarError(conexionYama,ERROR_TRANSFORMACION);
			enviarIndicacionTransformacion(conexionYama,transformacion);
			log_error(logMaster, "Error en la transformacion, worker de conexion %s : %s", transformacion->ip, transformacion->puerto);

		} else
		{
			pthread_mutex_unlock(&mutexErrorTransformacion);
			log_info (logMaster, "una transformacion terminada");
			enviarTareaCompletada(conexionYama, TAREA_COMPLETADA);
			enviarIndicacionTransformacion(conexionYama, transformacion);
		}

void mandarDatosReduccionLocal(t_indicacionReduccionLocal * reduccion) {
	struct timeval t0;
	struct timeval t1;
	gettimeofday(&t0, NULL);

	t_pedidoReduccionLocal * pedido = malloc(sizeof(t_pedidoReduccionLocal));
	if (pedido == NULL) {
		printf("no hay memoria disponible");
		exit(EXIT_FAILURE);
	}
	pedido->rutaScript = rutaScriptReductor;
	pedido->archivoReduccionLocal = reduccion->archivoTemporalReduccionLocal;
	pedido->archivoTransformacion = reduccion->archivoTemporalTransformacion;
	int worker = conectarCliente(reduccion->ip, reduccion->puerto, WORKER);

	// gestion de tareas paralelas


		free(pedido->rutaArchivoTemporal);
		free(pedido->rutaScriptTransformacion);
		free(pedido);

		free(transformacion->ip);
		free(transformacion->nodo);
		free(transformacion->puerto);
		free(transformacion->rutaArchivoTemporal);
		free(transformacion);

}


	pthread_mutex_unlock(&mutexTareasParalelasReduccionLocal);

		t_pedidoReduccionLocal * pedido = malloc(sizeof(t_pedidoReduccionLocal));
		if(pedido == NULL)
		{
			printf("no hay memoria disponible");
			exit(EXIT_FAILURE);
		}

		pedido->rutaScript = calloc(string_length(rutaScriptReductor), sizeof(char));
		if(pedido->rutaScript == NULL)
		{
			printf("no hay memoria disponible");
			exit(EXIT_FAILURE);
		}
		pedido->rutaScript = rutaScriptReductor;

		pedido->archivoReduccionLocal = calloc(string_length(reduccion->archivoTemporalReduccionLocal), sizeof(char));
		if(pedido->archivoReduccionLocal == NULL)
		{
			printf("no hay memoria disponible");
			exit(EXIT_FAILURE);
		}
		pedido->archivoReduccionLocal = reduccion -> archivoTemporalReduccionLocal;

		pedido->archivoTransformacion = calloc(string_length(reduccion -> archivoTemporalTransformacion), sizeof(char));
		if(pedido->archivoTransformacion == NULL)
		{
			printf("no hay memoria disponible");
			exit(EXIT_FAILURE);
		}
		pedido->archivoTransformacion = reduccion -> archivoTemporalTransformacion;

		int worker = conectarCliente(reduccion->ip,reduccion->puerto, WORKER);

	pthread_mutex_lock(&mutexErrorReduccionLocal);

	errorReduLocal = false;

	gestionarSolicitudes(worker, (void*) procesarPaquete);

	if (errorReduLocal) {
		tablaMetricas.cantidadFallosReduccionLocal++;
		pthread_mutex_unlock(&mutexErrorReduccionLocal);
		log_error(logMaster,
				"error en la reduccion local para el nodo de conexion %s : %s",
				reduccion->ip, reduccion->puerto);
		enviarIndicacionReduccionLocal(conexionYama, reduccion);
		enviarMensaje(conexionYama, ERROR_REDUCCION_LOCAL);
	} else {
		pthread_mutex_unlock(&mutexErrorReduccionLocal);
		log_info(logMaster, "reduccion local completada");
		enviarIndicacionReduccionLocal(conexionYama, reduccion);
		enviarMensaje(conexionYama, SALIOBIEN);
	}

	pthread_mutex_lock(&mutexTareasParalelasReduccionLocal);
	tareasReduccionLocalEnParalelo--;
	pthread_mutex_unlock(&mutexTareasParalelasReduccionLocal);

	gettimeofday(t1, NULL);
	float tiempoTotal = (t1.tv_sec - t0.tv_sec) * 1000
			+ (t1.tv_usec - t0.tv_usec) / 1000;

	list_add(tiemposReduccionLocal, &tiempoTotal);

	free(pedido);
	free(reduccion);
}

		if(errorReduLocal)
		{
			tablaMetricas.cantidadFallosReduccionLocal ++;
			pthread_mutex_unlock(&mutexErrorReduccionLocal);
			log_error(logMaster, "error en la reduccion local para el nodo de conexion %s : %s", reduccion->ip, reduccion->puerto);
			enviarError(conexionYama,ERROR_REDUCCION_LOCAL);
			enviarIndicacionReduccionLocal(conexionYama,reduccion);
		} else
		{
			pthread_mutex_unlock(&mutexErrorReduccionLocal);
			log_info(logMaster, "reduccion local completada");
			enviarTareaCompletada(conexionYama,TAREA_COMPLETADA);
			enviarIndicacionReduccionLocal(conexionYama,reduccion);
		}

		posActual++;

		pthread_mutex_lock (&mutexTareasParalelasReduccionLocal);
		tareasReduccionLocalEnParalelo --;
		pthread_mutex_unlock (&mutexTareasParalelasReduccionLocal);

		gettimeofday(t1,NULL);
		float tiempoTotal = (t1.tv_sec - t0.tv_sec) *1000 + (t1.tv_usec - t0.tv_usec) / 1000;

		list_add(tiemposReduccionLocal, &tiempoTotal);

		free(pedido->archivoReduccionLocal);
		free(pedido->archivoTransformacion);
		free(pedido->rutaScript);
		free (pedido);

		free (reduccion->archivoTemporalReduccionLocal);
		free (reduccion->archivoTemporalTransformacion);
		free (reduccion->ip);
		free (reduccion->nodo);
		free (reduccion->puerto);
		free (reduccion);
	}

	posActual = 0;
	while (posActual < cantidadSolicitudes) {
		enviarSolicitudReduccionGlobal(conexionWorker, pedidos[posActual]);
		free(pedidos[posActual]);
		posActual++;
	}

void gestionarReduccionGlobal()
	{
		log_info(logMaster, "arranca la reduccion global");
		struct timeval t0;
		struct timeval t1;
		gettimeofday (&t0,NULL);
		int cantidadSolicitudes = list_size(indicacionesDeReduccionGlobal);
		int posActual = 0;
		t_pedidoReduccionGlobal * pedidos[cantidadSolicitudes-1];
		int conexionWorker;
		while(posActual < cantidadSolicitudes)
		{
			t_pedidoReduccionGlobal * pedido = malloc (sizeof(t_pedidoReduccionGlobal));
			if(pedido == NULL)
			{
				log_error(logMaster, "no hay memoria disponible");
				exit(EXIT_FAILURE);
			}
			t_indicacionReduccionGlobal * indicacion = list_get(indicacionesDeReduccionGlobal,posActual);

			pedido->cantWorkerInvolucradros = cantidadSolicitudes;

			pedido->ArchivoResultadoReduccionGlobal = calloc (string_length(indicacion->archivoDeReduccionGlobal),sizeof(char));
			if(pedido->ArchivoResultadoReduccionGlobal == NULL)
			{
				log_error(logMaster, "no hay memoria disponible");
				exit(EXIT_FAILURE);
			}
			pedido->ArchivoResultadoReduccionGlobal = indicacion->archivoDeReduccionGlobal;

			pedido->archivoReduccionPorWorker = calloc (string_length(indicacion->archivoDeReduccionLocal),sizeof(char));
			if(pedido->archivoReduccionPorWorker == NULL)
			{
				log_error(logMaster, "no hay memoria disponible");
				exit(EXIT_FAILURE);
			}
			pedido->archivoReduccionPorWorker = indicacion->archivoDeReduccionLocal;

			pedido->workerEncargado = indicacion->encargado;

			pedido->ip = calloc (string_length(indicacion->ip),sizeof(char));
			if(pedido->ip == NULL)
			{
				log_error(logMaster, "no hay memoria disponible");
				exit(EXIT_FAILURE);
			}
			pedido->ip = indicacion->ip;

			pedido->puerto = calloc (string_length(indicacion->puerto),sizeof(char));
			if(pedido->puerto == NULL)
			{
				log_error(logMaster, "no hay memoria disponible");
				exit(EXIT_FAILURE);
			}
			pedido->puerto = indicacion->puerto;


			pedidos[posActual] = pedido;

			if ( pedido -> workerEncargado == 1)
			{
				conexionWorker= conectarCliente(indicacion->ip,indicacion->puerto, WORKER);
			}

			posActual ++;

			liberarReduGlobal(indicacion);
		}

		posActual = 0;
		while(posActual < cantidadSolicitudes)
		{
			enviarSolicitudReduccionGlobal(conexionWorker,pedidos[posActual]);

			free(pedidos[posActual]->ArchivoResultadoReduccionGlobal);
			free(pedidos[posActual]->archivoReduccionPorWorker);
			free(pedidos[posActual]->ip);
			free(pedidos[posActual]->puerto);
			free(pedidos[posActual]);
			posActual++;
		}

		gestionarSolicitudes(conexionWorker,(void *) procesarPaquete);

		if (errorReduGlobal)
		{
			tablaMetricas.cantidadFallosReduccionGlobal ++;
			enviarError(conexionYama,ERROR_REDUCCION_GLOBAL);
			log_error(logMaster, "Error en la reduccion, worker de conexion %d", conexionWorker);

		} else
		{
		enviarTareaCompletada(conexionYama, TAREA_COMPLETADA);
		}

		gettimeofday(t1,NULL);

	gettimeofday(t1, NULL);

	tiempoReduccionGlobal = (t1.tv_sec - t0.tv_sec) * 1000
			+ (t1.tv_usec - t0.tv_usec) / 1000;

	tablaMetricas.cantidadTareasTotalesReduccionGlobal = cantidadSolicitudes;

}

void gestionarAlmacenadoFinal(t_indicacionAlmacenadoFinal * indicacion) {
	log_info(logMaster, "arranca el almacenado final");
	t_pedidoAlmacenadoFinal * solicitud = malloc(
			sizeof(t_pedidoAlmacenadoFinal));
	if (solicitud == NULL) {
		log_error(logMaster, " no hay memoria disponible");
		exit(EXIT_FAILURE);
	}
  
	solicitud->archivoReduccionGlobal = calloc(string_length(indicacion->rutaArchivoReduccionGlobal), sizeof(char));
	if(solicitud->archivoReduccionGlobal == NULL)
	{
		log_error(logMaster, " no hay memoria disponible");
		exit(EXIT_FAILURE);
	}
	solicitud->archivoReduccionGlobal = indicacion->rutaArchivoReduccionGlobal;

	solicitud->rutaAlmacenadoFinal = calloc(string_length(rutaParaAlmacenarArchivo),sizeof(char));
	if(solicitud->rutaAlmacenadoFinal == NULL)
	{
		log_error(logMaster, " no hay memoria disponible");
		exit(EXIT_FAILURE);
	}
	solicitud->rutaAlmacenadoFinal = rutaParaAlmacenarArchivo;


	int conexionWorker = conectarCliente(indicacion->ip,indicacion->puerto,WORKER);
	enviarSolicitudAlmacenadoFinal(conexionWorker,solicitud);

	gestionarSolicitudes(conexionWorker, (void *) procesarPaquete);

	if (errorAlmacenamiento)
	{
		tablaMetricas.cantidadFallosAlmacenamiento ++;
		enviarError(conexionYama,ERROR_ALMACENAMIENTO_FINAL);
		enviarIndicacionAlmacenadoFinal(conexionYama,indicacion);
		log_error(logMaster, "Error en el almacenado final");
	}
	else
	{
		enviarTareaCompletada(conexionYama, TAREA_COMPLETADA);
		enviarIndicacionAlmacenadoFinal(conexionYama,indicacion);
	}

	free(solicitud->archivoReduccionGlobal);
	free(solicitud->rutaAlmacenadoFinal);
	free(solicitud);

	free(indicacion->ip);
	free(indicacion->nodo);
	free(indicacion->puerto);
	free(indicacion->rutaArchivoReduccionGlobal);
	free(indicacion);
}

void signal_capturer(int numeroSenial) {

	switch (numeroSenial) {
		case 8:
			enviarMensaje(conexionYama,"division por 0");
			log_error(logMaster, "PROCESO MASTER CIERRA POR ERROR DE COMA FLOTANTE");
			exit(EXIT_FAILURE);
			break;
		case 11:
			enviarMensaje(conexionYama,"Segmentation fault");
			log_error(logMaster, "PROCESO MASTER CIERRA POR SEGMENTATION FAULT");
			exit(EXIT_FAILURE);
			break;
		case 16:
			enviarMensaje(conexionYama,"Stack Overflow");
			log_error(logMaster, "PROCESO MASTER CIERRA POR STACK OVERFLOW");
			exit(EXIT_FAILURE);
			break;
		default:
			enviarMensaje(conexionYama, "Rompi por una razon desconocida");
			log_error(logMaster, "PROCESO MASTER CIERRA POR NUMERO DE SEÑAL %d", numeroSenial);
			exit(EXIT_FAILURE);
			break;

	}

	return;
}

void procesarPaquete(t_paquete * unPaquete, int * client_socket) { // contesto a *client_socket

	switch (unPaquete->codigoOperacion) {

	case ENVIAR_INDICACION_TRANSFORMACION:
		;
		log_info(logMaster, "LLega solicitud de trasnformacion");

		t_indicacionTransformacion * indicacionTransformacion =
				recibirIndicacionTransformacion(unPaquete);
		pthread_mutex_lock(&mutexTransformaciones);
		tareasTotalesTransformacion++;
		pthread_mutex_unlock(&mutexTransformaciones);

		hilosTransformacion = realloc(&hilosReduccionLocal,
				sizeof(pthread_t) * tareasTotalesTransformacion);
		pthread_create(&hilosTransformacion[tareasTotalesTransformacion - 1],
				NULL, (void *) mandarDatosTransformacion,
				(void*) indicacionTransformacion);

		break;

	case ENVIAR_INDICACION_REDUCCION_LOCAL:

		;

		log_info(logMaster, "LLega solicitud de reduccion local");
		pthread_mutex_lock(&mutexReduLocal);
		tareasTotalesReduccionLocal++;
		pthread_mutex_unlock(&mutexReduLocal);

		t_indicacionReduccionLocal * indicacionReduLocal =
				recibirIndicacionReduccionLocal(unPaquete);
		hilosReduccionLocal = realloc(&hilosReduccionLocal,
				sizeof(pthread_t) * tareasTotalesReduccionLocal);
		pthread_create(&hilosReduccionLocal[tareasTotalesReduccionLocal - 1],
				NULL, (void *) mandarDatosReduccionLocal,
				(void*) indicacionReduLocal);

		break;

	case ENVIAR_INDICACION_REDUCCION_GLOBAL:
	 ;
	    log_info(logMaster,"LLega solicitud de Reduccion global");
		t_indicacionReduccionGlobal * indicacionesParaReduccionGlobal = recibirIndicacionReduccionGlobal(unPaquete);
	    list_add(indicacionesDeReduccionGlobal, indicacionesParaReduccionGlobal);
		break;

	case ENVIAR_INDICACION_ALMACENADO_FINAL:
		;
		log_info(logMaster, "LLega solicitud de almacenado final");
		t_indicacionAlmacenadoFinal * indicacionAlmacenamientoFinal =
				recibirIndicacionAlmacenadoFinal(unPaquete);
		gestionarAlmacenadoFinal(indicacionAlmacenamientoFinal);
		break;

	case CONTINUA_MENSAJES:
		  ;
		 int continuacion = recibirTareaCompletada(unPaquete);
		 if (continuacion == NO_CONTINUA)
		 {
			 finDeSolicitudes = true;
		 }

		 break;

	case ENVIAR_ERROR_JOB:
		  ;
		 int error= recibirError(unPaquete);
		 switch (error){
		    case ERROR_REDUCCION_LOCAL:

				 errorReduLocal = true;
				 break;

			case ERROR_REDUCCION_GLOBAL:

				 errorReduGlobal = true;
				 break;

			case ERROR_ALMACENAMIENTO_FINAL:

				errorAlmacenamiento = true;
				break;

			default:
				break;
			}
		 break;

	default:
		 break;

	}



	destruirPaquete(unPaquete);
}

void calcularTiempoTotalTransformacion() {
	int i = 0;
	for (; i < list_size(tiemposTransformacion); i++) {
		float * a = list_get(tiemposTransformacion, i);
		tiempoTransformacion = tiempoTransformacion + *a;
	}
}

void calcularTiempoTotalReduccionLocal() {

	int i = 0;
	for (; i < list_size(tiemposReduccionLocal); i++) {
		float * a = list_get(tiemposReduccionLocal, i);
		tiempoReduccionLocal = tiempoReduccionLocal + *a;
	}
}

void liberarMemoria() {

	list_destroy_and_destroy_elements(indicacionesDeReduccionGlobal, (void *) liberarReduGlobal);
	list_destroy_and_destroy_elements(tiemposReduccionLocal, (void *) free);
	list_destroy_and_destroy_elements(tiemposTransformacion, (void *) free);
	int i = 0;

	for (; i <= tablaMetricas.cantidadTareasTotalesReduccionLocal; i++) {
		free(&hilosReduccionLocal[i]);
	}

	free(hilosReduccionLocal);
	int j = 0;

	for (; j < tareasTotalesTransformacion; j++) {
		free(&hilosTransformacion[j]);
	}

	free(hilosTransformacion);
}

void liberarReduGlobal(t_indicacionReduccionGlobal * ind)
{
	free(ind->archivoDeReduccionGlobal);
	free(ind->archivoDeReduccionLocal);
	free(ind->ip);
	free(ind->nodo);
	free(ind->puerto);
	free(ind);
}
