#include "master.h"

int main(int argc, char **argv) {

	logMaster = t_log_create("/home/utnso/workspace/tp-2017-2c-NULL/master/log/", "master.log",false,LOG_LEVEL_ERROR);
	log_info(logMaster, "arranca proceso master");
	struct timeval tiempoInicio;
	struct timeval tiempoFin;
	float total;
	gettimeofday(&tiempoInicio, NULL);
	pedidosDeTransformacion = list_create();
	pedidosDeReduccionLocal = list_create();
	pedidosDeReduccionGlobal = list_create();
	finDeSolicitudes = false;
	terminoConError = false;

	rutaScriptTransformador = argv [1];
	rutaScriptReductor = argv [2];
	rutaArchivoParaArrancar = argv [3];
	rutaParaAlmacenarArchivo = argv [4];

	conexionYama = leerConfiguracion();

	signal(SIGFPE,signal_capturer);


	enviarMensaje(conexionYama,rutaArchivoParaArrancar);


	while(finDeSolicitudes == false){

	gestionarSolicitudes(conexionYama, (void *) procesarPaquete);

	}

	finDeSolicitudes = false;

	while(finDeSolicitudes == false)
	{

		gestionarSolicitudes(conexionYama,(void *) procesarPaquete);

	}

	finDeSolicitudes = false;

	while(finDeSolicitudes == false)
	{

		gestionarSolicitudes(conexionYama, (void *) procesarPaquete);

	}


	//para almacenado final
	gestionarSolicitudes(conexionYama, (void*) procesarPaquete);



	gettimeofday(&tiempoFin, NULL);

	total = (tiempoFin.tv_sec - tiempoInicio.tv_sec) *1000 + (tiempoFin.tv_usec - tiempoInicio.tv_usec) / 1000;

	tablaMetricas.tiempoTotal = total;
	tablaMetricas.promedioJobs = (tiempoReduccionGlobal + tiempoReduccionLocal + tiempoTransformacion) / 3;

	printf("El proceso Master finalizo con las siguientes metricas:\n");
	printf("cantidad tareas de transformacion : %d\n", tablaMetricas.cantidadTareasTotalesTransformacion);
	printf("cantidad tareas de Reduccion Local : %d\n", tablaMetricas.cantidadTareasTotalesReduccionLocal);
	printf("cantidad tareas de Reduccion Global : %d\n", tablaMetricas.cantidadTareasTotalesReduccionGlobal);
	printf("cantidad maxima de tareas de transformacion en paralelo : %d\n", tablaMetricas.cantMaximaTareasTransformacionParalelas);
	printf("cantidad maxima de tareas de reduccion local en paralelo : %d\n", tablaMetricas.cantMaximaTareasReduccionLocalParalelas);
	printf("tiempo total de ejecucion : %f\n", tablaMetricas.tiempoTotal);
	printf("tiempo promedio jobs: %f\n", tablaMetricas.promedioJobs);


	liberarListas();
	log_destroy(logMaster);

	log_info(logMaster, "termino el proceso master");
	return EXIT_SUCCESS;

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


void gestionarTransformacion(){

	log_info(logMaster, "arrancan las transformaciones");

	struct timeval t0;
	struct timeval t1;
	gettimeofday(&t0,NULL);


	int conexionWorker;
	int cantPedidos = list_size(pedidosDeTransformacion);
	int posActual = 0;
	pthread_t hiloTransformar[cantPedidos];
	t_pedidoTransformacion * solicitudesDeTransformacion[cantPedidos];
	int conexionesWorker[cantPedidos];

	while(posActual <= cantPedidos)
	{
		t_indicacionTransformacion * indicacion = list_get(pedidosDeTransformacion, posActual);
		t_pedidoTransformacion * pedido = malloc(sizeof(t_pedidoTransformacion));
		pedido->bloque = indicacion->bloque;
//		pedido->bytes = indicacion->bytes; faltan los bytes ocupados
		pedido->rutaArchivoTemporal = indicacion->rutaArchivoTemporal;
		solicitudesDeTransformacion[posActual] = pedido;
		conexionWorker = conectarCliente(indicacion->ip,indicacion->puerto,WORKER);
		conexionesWorker[posActual]=conexionWorker;
		posActual ++;
	}
	posActual = 0;

	while(posActual <= cantPedidos)
	{
		transformacion * transformacion= malloc(sizeof(transformacion));
		transformacion->conexion = conexionesWorker[posActual];
		transformacion->ind = solicitudesDeTransformacion[posActual];
		pthread_create(&hiloTransformar[posActual], NULL ,(void *) mandarDatosTransformacion,(void*) transformacion);
		posActual++;
	}
	int i=0;
	for(;cantPedidos>i;i++)
	{
		pthread_join(hiloTransformar[i],NULL);
	}

	enviarMensaje(conexionYama,SALIOBIEN);
	gettimeofday(&t1,NULL);

	tiempoTransformacion = (t1.tv_sec - t0.tv_sec) *1000 + (t1.tv_usec - t0.tv_usec) / 1000;

	pthread_mutex_lock(&mutexMetricas);
	tablaMetricas.cantidadTareasTotalesTransformacion = cantPedidos;
	pthread_mutex_unlock(&mutexMetricas);


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

void mandarDatosTransformacion(transformacion * transformacion){ //todo liberar memoria


		pthread_mutex_lock (&variableTareasTransformacion);
		tareasTransformacion ++;

		if(tareasTransformacion > tablaMetricas.cantMaximaTareasTransformacionParalelas)
		{
			tablaMetricas.cantMaximaTareasTransformacionParalelas = tareasTransformacion;
		}

		pthread_mutex_unlock (&variableTareasTransformacion);

		pthread_mutex_t mutexArchivo;

		pthread_mutex_lock(&mutexArchivo);
		enviarSolicitudTransformacion(transformacion->conexion,transformacion->ind);
		pthread_mutex_unlock(&mutexArchivo);

		pthread_mutex_lock (&variableTareasTransformacion);
		tareasTransformacion --;
		pthread_mutex_unlock (&variableTareasTransformacion);

		gestionarSolicitudes(transformacion->conexion, (void *) procesarPaquete);

		if(terminoConError)
		{
			enviarMensaje(conexionYama,ERROR);
			log_error(logMaster, "Error en la transformacion, worker de conexion %d", transformacion->conexion);
			free(transformacion->ind);
			free(transformacion);
			return;
		} else
		{
			log_info (logMaster, "una transformacionTerminada");
		}

		free(transformacion->ind);
		free(transformacion);

}

void gestionarReduccionLocal(){

	struct timeval t0;
	struct timeval t1;
	gettimeofday(&t0,NULL);
	int cantidadDeSolicitudes = list_size(pedidosDeReduccionLocal);
	pthread_t hiloReduLocal[cantidadDeSolicitudes];
	int posActual = 0;
	int conexionWorker;


	while(posActual<=cantidadDeSolicitudes)
	{
		log_info(logMaster, "arrancan las reducciones");
		reduLocal * reduccion = malloc(sizeof(reduccion));
		t_indicacionReduccionLocal * indicacion = list_get(pedidosDeTransformacion , list_size(pedidosDeTransformacion));
		t_pedidoReduccionLocal * pedido = malloc (sizeof (t_pedidoReduccionLocal));
		pedido->archivoReduccionLocal = indicacion->archivoTemporalReduccionLocal;
		pedido->archivoTransformacion = indicacion->archivoTemporalTransformacion;
		conexionWorker = conectarCliente(indicacion->ip,indicacion->puerto,WORKER);

		reduccion->conexion = conexionWorker;
		reduccion->ind = pedido;
		pthread_create(&hiloReduLocal[posActual],NULL,(void *) mandarDatosTransformacion, (void *) reduccion);
		posActual ++;


	}

	int i=0;
	for(;i<cantidadDeSolicitudes; i++)
	{
		pthread_join(hiloReduLocal[i],NULL);
	}

	enviarMensaje(conexionYama,SALIOBIEN);

	gettimeofday(&t1,NULL);
	tiempoReduccionLocal = (t1.tv_sec - t0.tv_sec) *1000 + (t1.tv_usec - t0.tv_usec) / 1000;

	pthread_mutex_lock (&mutexMetricas);
	tablaMetricas.cantidadTareasTotalesReduccionLocal = cantidadDeSolicitudes;
	pthread_mutex_unlock(&mutexMetricas);
}


void mandarDatosReduccionLocal(reduLocal * reduccion)
	{

		pthread_mutex_lock (&variableTareasReduccionLocal);
		tareasReduccion ++;

		if(tablaMetricas.cantMaximaTareasReduccionLocalParalelas < tareasReduccion)
		{
			tablaMetricas.cantMaximaTareasReduccionLocalParalelas = tareasReduccion;
		}

		pthread_mutex_unlock (&variableTareasTransformacion);

		enviarSolicitudReduccionLocal(reduccion->conexion, reduccion->ind); // acá va el cambio de estructura

		gestionarSolicitudes(reduccion->conexion,(void*) procesarPaquete);

		if(terminoConError)
		{
			enviarMensaje(conexionYama,ERROR);
			return;
		}

		pthread_mutex_lock (&variableTareasReduccionLocal);
		tareasReduccion --;
		pthread_mutex_unlock (&variableTareasReduccionLocal);

		free (reduccion->ind);
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
		while(posActual <= cantidadSolicitudes)
		{
			t_pedidoReduccionGlobal * pedido = malloc (sizeof(t_pedidoReduccionGlobal));
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
		pthread_mutex_lock(&mutexMetricas);
		tablaMetricas.cantidadTareasTotalesReduccionGlobal = cantidadSolicitudes;
		pthread_mutex_unlock(&mutexMetricas);

	}

void mandarDatosReduccionGlobal(reduGlobal * reduccion)
{
	enviarSolicitudReduccionGlobal(reduccion->conexion,reduccion->reduGlobal);
	gestionarSolicitudes(reduccion->conexion, (void *) procesarPaquete);

	if (terminoConError)
	{
		enviarMensaje(conexionYama,ERROR);
		log_error(logMaster, "Error en la reduccion, worker de conexion %d", reduccion->conexion);
		free(reduccion->reduGlobal);
		free(reduccion);
		return;
	}

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
	gestionarSolicitudes(conexionWorker,(void *) procesarPaquete);

	if (terminoConError)
	{
		enviarMensaje(conexionYama,ERROR);
		log_error(logMaster, "Error en el almacenado final");
		free(solicitud);
		free(indicacion);
		return;
	}

	enviarMensaje(conexionYama, "termino todo wacho");
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
		list_add(pedidosDeTransformacion,indicacionTransformacion); //leno lista con pedidos

		break;


	case ENVIAR_INDICACION_REDUCCION_LOCAL: //guarda, acá recibe una INDICACIONDEREDULOCAL, no una solicitud

		 ;
		t_indicacionReduccionLocal * indicacionReduLocal = recibirIndicacionReduccionLocal(unPaquete);
		list_add(pedidosDeReduccionLocal,indicacionReduLocal);
		break;


	case ENVIAR_INDICACION_REDUCCION_GLOBAL:
	 ;
		t_indicacionReduccionGlobal * indicacionesParaReduccionGlobal = recibirIndicacionReduccionGlobal(unPaquete);
	    list_add(pedidosDeReduccionGlobal, indicacionesParaReduccionGlobal);
		break;

	case ENVIAR_INDICACION_ALMACENADO_FINAL:
		 ;
		 t_indicacionAlmacenadoFinal * indicacionAlmacenamientoFinal = recibirIndicacionAlmacenadoFinal(unPaquete);
		 gestionarAlmacenadoFinal(indicacionAlmacenamientoFinal);
		 break;

	case ENVIAR_MENSAJE:
		  ;
		 char* mensaje = string_new();
		 string_append(&mensaje ,recibirMensaje(unPaquete));

		 if(string_equals_ignore_case(mensaje,FIN))
		 {
			 finDeSolicitudes = true;

		 } else

			 if(string_equals_ignore_case(mensaje,ERROR))
			 {
				 terminoConError = true;

			 } else

			     if(string_equals_ignore_case(mensaje,SALIOBIEN))
			     {
			    	 terminoConError = false;
			     }
		 free(mensaje);
		 break;


	default:

		 break;
	}

	destruirPaquete(unPaquete);
}


void liberarListas() {

	list_destroy_and_destroy_elements(pedidosDeTransformacion, (void *) free);
	list_destroy_and_destroy_elements(pedidosDeReduccionLocal, (void *) free);
	list_destroy_and_destroy_elements(pedidosDeReduccionGlobal, (void *) free);

}

