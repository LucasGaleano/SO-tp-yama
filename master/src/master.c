#include "master.h"

int main(int argc, char **argv) {

	inicializarVariablesGlobales();
	// LOG
	log_trace(logMaster, "arranca proceso master");

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

	conexionYama = leerConfiguracionYConectarYama();
	// Arranca logica del proceso

	signal(SIGFPE, signal_capturer);
	signal(SIGPIPE, signal_capturer);
	signal(SIGSEGV, signal_capturer);

	enviarRutaParaArrancarTransformacion(conexionYama, rutaArchivoParaArrancar);

	while (!finDeSolicitudes && !dejarDeRecibirSolicitudes) {
		log_trace(logMaster, "Esperando solicitud de Yama");
		gestionarSolicitudes(conexionYama, (void *) procesarPaquete, logMaster);

	}
	int i = 0;

	while (tareasTotalesTransformacion > i )
	{
		pthread_join(hilosTransformacion[i],NULL);
		i++;
	}

	i=0;

	while (tareasTotalesReduccionLocal > i )
	{
		pthread_join(hilosReduccionLocal[i],NULL);
		i++;
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

	log_trace(logMaster, "termino el proceso master");
	log_destroy(logMaster);

	return EXIT_SUCCESS;

}

void inicializarVariablesGlobales() {

	logMaster = log_create("master.log", "master", true, LOG_LEVEL_TRACE);

	dejarDeRecibirSolicitudes = false;

	hilosReduccionLocal = malloc(sizeof(pthread_t));
	if (hilosReduccionLocal == NULL) {
		log_trace(logMaster, "No hay memoria disponible");
		exit(EXIT_SUCCESS);
	}
	hilosTransformacion = malloc(sizeof(pthread_t));
	if (hilosReduccionLocal == NULL) {
		log_trace(logMaster, "No hay memoria disponible");
		exit(EXIT_SUCCESS);
	}
	pedidosDeTransformacion = list_create();
	indicacionesDeReduccionGlobal = list_create();
	tiemposReduccionLocal = list_create();
	tiemposTransformacion = list_create();
	tiempoTransformacion = 0;
	tiempoReduccionLocal = 0;
	finDeSolicitudes = false;
	errorWorker = false;
	tablaMetricas.cantidadFallosAlmacenamiento = 0;
	tablaMetricas.cantidadFallosReduccionLocal = 0;
	tablaMetricas.cantidadFallosReduccionGlobal = 0;
	tablaMetricas.cantidadFallosAlmacenamiento = 0;
	tareasTotalesReduccionLocal = 0;

}

int leerConfiguracionYConectarYama() {

	char* ruta =
			"/home/utnso/workspace/tp-2017-2c-NULL/configuraciones/master.cfg";
	t_config * config = config_create(ruta);

	char * puerto = config_get_string_value(config, "YAMA_PUERTO");
	char * ip = config_get_string_value(config, "YAMA_IP");

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

void mandarDatosTransformacion(t_indicacionTransformacion * transformacion) {

	struct timeval t0;
	struct timeval t1;
	gettimeofday(&t0, NULL);

	t_pedidoTransformacion * pedido = malloc(sizeof(t_pedidoTransformacion));
	if (pedido == NULL) {
		log_trace(logMaster, "no hay memoria suficiente");
		exit(EXIT_SUCCESS);
	}

	pedido->rutaArchivoTemporal = string_duplicate(
			transformacion->rutaArchivoTemporal);

	pedido->rutaScriptTransformacion = string_duplicate(
			rutaScriptTransformador);

	pedido->cantBytes = transformacion->bytes;
	pedido->numBloque = transformacion->bloque;
	pthread_mutex_lock(&mutexTareasTransformacionEnParalelo);

	tareasTransformacionEnParalelo++;

	if (tareasTransformacionEnParalelo
			> tablaMetricas.cantMaximaTareasTransformacionParalelas) {
		tablaMetricas.cantMaximaTareasTransformacionParalelas =
				tareasTransformacionEnParalelo;
	}

	pthread_mutex_unlock(&mutexTareasTransformacionEnParalelo);

	int conexionWorker = conectarCliente(transformacion->ip,
			transformacion->puerto, MASTER);

	enviarSolicitudTransformacion(conexionWorker, pedido);

	pthread_mutex_lock(&mutexErrorTransformacion);

	errorTransformacion = false;

	gestionarSolicitudes(conexionWorker, (void *) procesarPaquete, logMaster);

	if (errorTransformacion || errorWorker || dejarDeRecibirSolicitudes) {
		tablaMetricas.cantidadFallosTransformacion++;
		pthread_mutex_unlock(&mutexErrorTransformacion);
		transformacion->estado = ERROR;
		enviarIndicacionTransformacion(conexionYama, transformacion);
		log_trace(logMaster,
				"Error en la transformacion, worker de conexion %s : %s",
				transformacion->ip, transformacion->puerto);

	} else {
		pthread_mutex_unlock(&mutexErrorTransformacion);
		log_trace(logMaster, "una transformacion terminada");
		transformacion->estado = FINALIZADO_OK;
		enviarIndicacionTransformacion(conexionYama, transformacion);
	}

	pthread_mutex_lock(&mutexTareasTransformacionEnParalelo);
	tareasTransformacionEnParalelo--;
	pthread_mutex_unlock(&mutexTareasTransformacionEnParalelo);

	gettimeofday(t1, NULL);
	float tiempoTotal = (t1.tv_sec - t0.tv_sec) * 1000
			+ (t1.tv_usec - t0.tv_usec) / 1000;

	list_add(tiemposTransformacion, &tiempoTotal);

	free(pedido->rutaArchivoTemporal);
	free(pedido->rutaScriptTransformacion);
	free(pedido);

	free(transformacion->ip);
	free(transformacion->nodo);
	free(transformacion->puerto);
	free(transformacion->rutaArchivoTemporal);
	free(transformacion);

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

	pedido->rutaScript = string_duplicate(rutaScriptReductor);

	pedido->archivoReduccionLocal = string_duplicate(
			reduccion->archivoTemporalReduccionLocal);

	pedido->archivoTransformacion = string_duplicate(
			reduccion->archivoTemporalTransformacion);

	int worker = conectarCliente(reduccion->ip, reduccion->puerto, MASTER);

	// gestion de tareas paralelas

	pthread_mutex_lock(&mutexTareasParalelasReduccionLocal);
	tareasReduccionLocalEnParalelo++;

	if (tablaMetricas.cantMaximaTareasReduccionLocalParalelas
			< tareasReduccionLocalEnParalelo) {
		tablaMetricas.cantMaximaTareasReduccionLocalParalelas =
				tareasReduccionLocalEnParalelo;
	}

	pthread_mutex_unlock(&mutexTareasParalelasReduccionLocal);

	enviarSolicitudReduccionLocal(worker, pedido); // acá va el cambio de estructura

	pthread_mutex_lock(&mutexErrorReduccionLocal);

	errorReduLocal = false;

	gestionarSolicitudes(worker, (void*) procesarPaquete, logMaster);

	if (errorReduLocal || errorWorker || dejarDeRecibirSolicitudes) {
		tablaMetricas.cantidadFallosReduccionLocal++;
		pthread_mutex_unlock(&mutexErrorReduccionLocal);
		enviarError(conexionYama, ERROR_REDUCCION_LOCAL);
		log_trace(logMaster,
				"error en la reduccion local para el nodo de conexion %s : %s",
				reduccion->ip, reduccion->puerto);

	} else {
		pthread_mutex_unlock(&mutexErrorReduccionLocal);
		log_trace(logMaster, "reduccion local completada");
		enviarIndicacionReduccionLocal(conexionYama, reduccion);
	}

	pthread_mutex_lock(&mutexTareasParalelasReduccionLocal);
	tareasReduccionLocalEnParalelo--;
	pthread_mutex_unlock(&mutexTareasParalelasReduccionLocal);

	gettimeofday(t1, NULL);
	float tiempoTotal = (t1.tv_sec - t0.tv_sec) * 1000
			+ (t1.tv_usec - t0.tv_usec) / 1000;

	list_add(tiemposReduccionLocal, &tiempoTotal);

	free(pedido->archivoReduccionLocal);
	free(pedido->archivoTransformacion);
	free(pedido->rutaScript);
	free(pedido);

	free(reduccion->archivoTemporalReduccionLocal);
	free(reduccion->archivoTemporalTransformacion);
	free(reduccion->ip);
	free(reduccion->nodo);
	free(reduccion->puerto);
	free(reduccion);
}

void neutro(t_paquete* unPaquete, int *s) {
	destruirPaquete(unPaquete);
}

void gestionarReduccionGlobal() {
	log_trace(logMaster, "arranca la reduccion global");
	struct timeval t0;
	struct timeval t1;
	gettimeofday(&t0, NULL);
	int cantidadSolicitudes = list_size(indicacionesDeReduccionGlobal);
	int posActual = 0;
	t_pedidoReduccionGlobal ** pedidos = calloc(cantidadSolicitudes - 1,
			sizeof(t_pedidoReduccionGlobal));
	int conexionWorker;
	while (posActual < (cantidadSolicitudes - 1)) // no sumo posActual si encuentro al encargado
	{
		t_pedidoReduccionGlobal * pedido = malloc(
				sizeof(t_pedidoReduccionGlobal));
		if (pedido == NULL) {
			log_trace(logMaster, "no hay memoria disponible");
			exit(EXIT_FAILURE);
		}
		t_indicacionReduccionGlobal * indicacion = list_get(
				indicacionesDeReduccionGlobal, posActual);

		pedido->cantWorkerInvolucradros = cantidadSolicitudes;

		pedido->ArchivoResultadoReduccionGlobal = string_duplicate(
				indicacion->archivoDeReduccionGlobal);

		pedido->archivoReduccionPorWorker = string_duplicate(
				indicacion->archivoDeReduccionLocal);

		pedido->workerEncargado = indicacion->encargado;

		pedido->ip = string_duplicate(indicacion->ip);

		pedido->puerto = string_duplicate(indicacion->puerto);

		if (pedido->workerEncargado == 1) {
			conexionWorker = conectarCliente(indicacion->ip, indicacion->puerto,
					MASTER);
			enviarSolicitudReduccionGlobal(conexionWorker, pedidos[posActual]);

			free(pedido->ArchivoResultadoReduccionGlobal);
			free(pedido->archivoReduccionPorWorker);
			free(pedido->ip);
			free(pedido->puerto);
			free(pedido);

		} else {
			pedidos[posActual] = pedido;
			posActual++;
		}
	}

	gestionarSolicitudes(conexionWorker, (void*) neutro, logMaster);

	posActual = 0;
	while (posActual < (cantidadSolicitudes - 1)) {
		conexionWorker = conectarCliente(pedidos[posActual]->ip,
				pedidos[posActual]->puerto, MASTER);
		enviarSolicitudReduccionGlobal(conexionWorker, pedidos[posActual]);

		free(pedidos[posActual]->ArchivoResultadoReduccionGlobal);
		free(pedidos[posActual]->archivoReduccionPorWorker);
		free(pedidos[posActual]->ip);
		free(pedidos[posActual]->puerto);
		free(pedidos[posActual]);
		posActual++;
	}

	gestionarSolicitudes(conexionWorker, (void *) procesarPaquete, logMaster);

	if (errorReduGlobal || errorWorker || dejarDeRecibirSolicitudes) {
		tablaMetricas.cantidadFallosReduccionGlobal++;
		enviarError(conexionYama, ERROR_REDUCCION_GLOBAL);
		log_trace(logMaster, "Error en la reduccion, worker de conexion %d",
				conexionWorker);

	} else {
		enviarTareaCompletada(conexionYama, REDUCCION_COMPLETADA);
	}

	gettimeofday(t1, NULL);

	tiempoReduccionGlobal = (t1.tv_sec - t0.tv_sec) * 1000
			+ (t1.tv_usec - t0.tv_usec) / 1000;

	tablaMetricas.cantidadTareasTotalesReduccionGlobal = cantidadSolicitudes;

	free(pedidos);
}

void gestionarAlmacenadoFinal(t_indicacionAlmacenadoFinal * indicacion) {
	log_trace(logMaster, "arranca el almacenado final");
	t_pedidoAlmacenadoFinal * solicitud = malloc(
			sizeof(t_pedidoAlmacenadoFinal));
	if (solicitud == NULL) {
		log_trace(logMaster, " no hay memoria disponible");
		exit(EXIT_FAILURE);
	}

	solicitud->archivoReduccionGlobal = string_duplicate(
			indicacion->rutaArchivoReduccionGlobal);

	solicitud->rutaAlmacenadoFinal = string_duplicate(rutaParaAlmacenarArchivo);

	int conexionWorker = conectarCliente(indicacion->ip, indicacion->puerto,
			MASTER);
	enviarSolicitudAlmacenadoFinal(conexionWorker, solicitud);

	gestionarSolicitudes(conexionWorker, (void *) procesarPaquete, logMaster);

	if (errorAlmacenamiento || errorWorker || dejarDeRecibirSolicitudes) {
		tablaMetricas.cantidadFallosAlmacenamiento++;
		enviarError(conexionYama, ERROR_ALMACENAMIENTO_FINAL);
		enviarIndicacionAlmacenadoFinal(conexionYama, indicacion);
		log_trace(logMaster, "Error en el almacenado final");
	} else {
		enviarTareaCompletada(conexionYama, ALMACENAMIENTO_COMPLETADO);
		enviarIndicacionAlmacenadoFinal(conexionYama, indicacion);
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
		enviarError(conexionYama, ERROR_MASTER);
		log_trace(logMaster,
				"PROCESO MASTER CIERRA POR ERROR DE COMA FLOTANTE");
		exit(EXIT_FAILURE);
		break;
	case SIGSEGV:
		enviarError(conexionYama, ERROR_MASTER);
		log_trace(logMaster, "PROCESO MASTER CIERRA POR SEGMENTATION FAULT");
		exit(EXIT_FAILURE);
		break;
	case SIGPIPE:
		enviarError(conexionYama, ERROR_MASTER);
		log_trace(logMaster, "PROCESO MASTER CIERRA POR PIPE ROTA");
		exit(EXIT_FAILURE);
		break;
	default:
		enviarError(conexionYama, ERROR_MASTER);
		log_trace(logMaster, "PROCESO MASTER CIERRA POR NUMERO DE SEÑAL %d",
				numeroSenial);
		exit(EXIT_FAILURE);
		break;

	}

	return;
}

void procesarPaquete(t_paquete * unPaquete, int * client_socket) { // contesto a *client_socket

	switch (unPaquete->codigoOperacion) {

	case ENVIAR_INDICACION_TRANSFORMACION:
		;
		log_trace(logMaster, "Llega solicitud de transformacion");

		t_indicacionTransformacion * indicacionTransformacion =
				recibirIndicacionTransformacion(unPaquete);
		tareasTotalesTransformacion++;

		hilosTransformacion = realloc(&hilosReduccionLocal,
				sizeof(pthread_t) * tareasTotalesTransformacion);
		pthread_create(&hilosTransformacion[tareasTotalesTransformacion - 1],
				NULL, (void *) mandarDatosTransformacion,
				(void*) indicacionTransformacion);

		break;

	case ENVIAR_INDICACION_REDUCCION_LOCAL:

		;

		log_trace(logMaster, "LLega solicitud de reduccion local");
		tareasTotalesReduccionLocal++;

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
		log_trace(logMaster, "LLega solicitud de Reduccion global");
		indicacionesDeReduccionGlobal = recibirIndicacionReduccionGlobal(unPaquete);
		gestionarReduccionGlobal();

		break;

	case ENVIAR_INDICACION_ALMACENADO_FINAL:
		;
		log_trace(logMaster, "LLega solicitud de almacenado final");
		t_indicacionAlmacenadoFinal * indicacionAlmacenamientoFinal =
				recibirIndicacionAlmacenadoFinal(unPaquete);
		gestionarAlmacenadoFinal(indicacionAlmacenamientoFinal);
		break;

	case CONTINUA_MENSAJES:
		;
		int continuacion = recibirTareaCompletada(unPaquete);
		if (continuacion == NO_CONTINUA) {
			finDeSolicitudes = true;
		}

		break;

	case ENVIAR_ERROR:
		;
		int error = recibirError(unPaquete);
		switch (error) {
		case ERROR_TRANSFORMACION:
			errorTransformacion = true;
			break;
		case ERROR_REDUCCION_LOCAL:

			errorReduLocal = true;
			break;

		case ERROR_REDUCCION_GLOBAL:

			errorReduGlobal = true;
			break;

		case ERROR_ALMACENAMIENTO_FINAL:

			errorAlmacenamiento = true;
			break;

		case ERROR_MASTER:

			errorWorker = true;
			break;

		default:
			procesarError();
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

	list_destroy_and_destroy_elements(indicacionesDeReduccionGlobal,
			(void *) liberarReduGlobal);
	list_destroy_and_destroy_elements(tiemposReduccionLocal, (void *) free);
	list_destroy_and_destroy_elements(tiemposTransformacion, (void *) free);
	int i = 0;

	for (; i <= tablaMetricas.cantidadTareasTotalesReduccionLocal; i++) {
		free(&hilosReduccionLocal[i]);
	}

	int j = 0;

	for (; j < tareasTotalesTransformacion; j++) {
		free(&hilosTransformacion[j]);
	}

}

void liberarReduGlobal(t_indicacionReduccionGlobal * ind) {
	free(ind->archivoDeReduccionGlobal);
	free(ind->archivoDeReduccionLocal);
	free(ind->ip);
	free(ind->nodo);
	free(ind->puerto);
	free(ind);
}

void procesarError() {
	log_trace(logMaster, "Me llego un error y dejo de recibir solicitudes \n");
	dejarDeRecibirSolicitudes = true;
}
