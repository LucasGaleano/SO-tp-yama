#include "master.h"

int main(int argc, char **argv) {

	logMaster = t_log_create("/home/utnso/workspace/tp-2017-2c-NULL/master/log/", "master.log",false,LOG_LEVEL_ERROR);
	struct timeval tiempoInicio;
	struct timeval tiempoFin;
	float total;
	gettimeofday(&tiempoInicio, NULL);
	pedidosDeTransformacion = list_create();
	pedidosDeReduccionLocal = list_create();
	pedidosDeReduccionGlobal = list_create();
	finDeSolicitudes = false;

	if(argv[1]==NULL)
	{
		log_error(logMaster,"no se ingreso primer parametro");
	}

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

	while(finDeSolicitudes == false){

		gestionarSolicitudes(conexionYama,(void *) procesarPaquete);

	}

	while(finDeSolicitudes == false){

		gestionarSolicitudes(conexionYama, (void *) procesarPaquete);

	}

	gestionarSolicitudes(conexionYama, (void*) procesarPaquete); //para almacenado final



	gettimeofday(&tiempoFin, NULL);

	total = (tiempoFin.tv_sec - tiempoInicio.tv_sec) *1000 + (tiempoFin.tv_usec - tiempoInicio.tv_usec) / 1000;

	tablaMetricas.tiempoTotal = total;
	tablaMetricas.promedioJobs = (tiempoReduccionGlobal + tiempoReduccionLocal + tiempoTransformacion) / 3;


	list_destroy(pedidosDeTransformacion);
	list_destroy(pedidosDeReduccionLocal);
	list_destroy(pedidosDeReduccionGlobal);
	log_destroy(logMaster);

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


	struct timeval t0;
	struct timeval t1;
	gettimeofday(&t0,NULL);



	int conexionWorker;
	int cantPedidos = list_size(pedidosDeTransformacion);
	int posActual = 0;
	pthread_t hiloTransformar[cantPedidos];
	t_indicacionTransformacion * indicacionesDeTransformacion[cantPedidos];
	int conexionesWorker[cantPedidos];
	while(posActual < cantPedidos)
	{
		t_pedidoTransformacion * pedido = list_get(pedidosDeTransformacion, posActual);
		t_indicacionTransformacion * indicacion = malloc(sizeof(t_indicacionTransformacion));
		indicacion->bloque = pedido->bloque;
		indicacion->ip = pedido->ip;
		indicacion->bytes = 2; // cómo sé cuántos bytes ocupados hay en un bloque?
		indicacion->rutaArchivoTemporal = pedido->rutaArchivoTemporal;
		*indicacionesDeTransformacion[posActual] = *indicacion;
		conexionWorker = conectarCliente(indicacion->ip,indicacion->puerto,WORKER);
		conexionesWorker[posActual]=conexionWorker;
		posActual ++;
	}
	posActual = 0;

	while(posActual <= cantPedidos)
	{
		transformacion * transformacion= malloc(sizeof(transformacion));
		transformacion->conexion = conexionesWorker[posActual];
		transformacion->ind = indicacionesDeTransformacion[posActual];
		pthread_create(&hiloTransformar[posActual], NULL ,(void *) mandarDatosTransformacion,(void*) transformacion);
		posActual++;
	}
	int i=0;
	for(;cantPedidos>i;i++)
	{
		pthread_join(hiloTransformar[i],NULL);
	}

	enviarMensaje(conexionYama,"salio todo piola la transformacion");
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

void mandarDatosTransformacion(transformacion transformacion){ //todo liberar memoria


		pthread_mutex_lock (&variableTareasTransformacion);
		tareasTransformacion ++;

		if(tareasTransformacion > tablaMetricas.cantMaximaTareasTransformacionParalelas)
		{
			tablaMetricas.cantMaximaTareasTransformacionParalelas = tareasTransformacion;
		}

		pthread_mutex_unlock (&variableTareasTransformacion);

		pthread_mutex_t mutexArchivo;

		pthread_mutex_lock(&mutexArchivo);
		enviarIndicacionTransformacion(transformacion.conexion,transformacion.ind);
		pthread_mutex_unlock(&mutexArchivo);

		pthread_mutex_lock (&variableTareasTransformacion);
		tareasTransformacion --;
		pthread_mutex_unlock (&variableTareasTransformacion);

//   recibirMensaje(conexion,algo)  --> CONFIRMACION DEL WORKER


}

void gestionarReduccionLocal(){

	struct timeval t0;
	struct timeval t1;
	gettimeofday(&t0,NULL);
	int cantidadDeSolicitudes = list_size(pedidosDeReduccionLocal);
	pthread_t hiloReduLocal[cantidadDeSolicitudes];
	int posActual = 0;
	int conexionWorker;


	while(posActual<cantidadDeSolicitudes)
	{
		reduLocal * reduccion = malloc(sizeof(reduccion));
		t_indicacionReduccionLocal * pedido = list_get(pedidosDeTransformacion , list_size(pedidosDeTransformacion));
		t_pedidoReduccionLocal * indicacion = malloc (sizeof (t_pedidoReduccionLocal));  // hay que cambiarle el nombre a esto
		indicacion->archivoReduccionLocal = pedido->archivoTemporalReduccionLocal;
		indicacion->archivoTransformacion = pedido->archivoTemporalTransformacion;
		conexionWorker = conectarCliente(pedido->ip,pedido->puerto,WORKER);

		reduccion->conexion = conexionWorker;
		reduccion->ind = indicacion;
		pthread_create(&hiloReduLocal[posActual],NULL,(void *) mandarDatosTransformacion, (void *) reduccion);
		posActual ++;


	}

	int i=0;
	for(;i<cantidadDeSolicitudes; i++)
	{
		pthread_join(hiloReduLocal[i],NULL);
	}

	enviarMensaje(conexionYama,"salio todo piola la reduccion"); // aca iria solo un número, sería 0 para todo bien y a partir de 1 para señales

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
		 //   recibirMensaje(conexion,algo)  --> CONFIRMACION DEL WORKER

		pthread_mutex_lock (&variableTareasReduccionLocal);
		tareasReduccion --;
		pthread_mutex_unlock (&variableTareasReduccionLocal);

		free (reduccion->ind);
		free (reduccion);
	}


void gestionarReduccionGlobal()
	{

		struct timeval t0;
		struct timeval t1;
		gettimeofday (&t0,NULL);
		int cantidadSolicitudes = list_size(pedidosDeReduccionGlobal);
		int posActual = 0;
		t_indicacionReduccionGlobal * indicaciones[cantidadSolicitudes];
		int conexionWorker;
		pthread_t hiloReduGlobal[cantidadSolicitudes] ;
		while(posActual < cantidadSolicitudes)  // todo seguir cambiando nombres
		{
			t_pedidoReduccionGlobal * pedido = malloc (sizeof(t_pedidoReduccionGlobal));
			t_pedidoReduccionGlobal * indicacion = list_get(pedidosDeReduccionGlobal,posActual);
			reduGlobal * reduccion = malloc (sizeof(reduGlobal));
			pedido->ArchivoResultadoReduccionGlobal = indicacion->ArchivoResultadoReduccionGlobal;
			pedido->archivoReduccionPorWorker = indicacion->archivoReduccionPorWorker;
			pedido->workerEncargdo = indicacion->workerEncargdo;
			indicaciones[posActual] = indicacion;

			if ( string_equals_ignore_case( pedido -> workerEncargdo , "1"))
			{
				conexionWorker= conectarCliente(indicacion->ip,indicacion->puerto, WORKER);
			}

			posActual ++;

		}

		posActual = 0;
		while(posActual < cantidadSolicitudes)
		{
			pthread_create(&hiloReduGlobal[posActual],NULL,(void*) mandarDatosReduccionGlobal, ((void*)&indicaciones[posActual],&conexionWorker));
			posActual++;

		}

		int i = 0;
		for(; i < cantidadSolicitudes; i++)
		{
			pthread_join (hiloReduGlobal[i],NULL);
		}

		enviarMensaje(conexionYama, "salio piola la reduGlobal");
		gettimeofday(t1,NULL);

		tiempoReduccionGlobal = (t1.tv_sec - t0.tv_sec) *1000 + (t1.tv_usec - t0.tv_usec) / 1000;
		pthread_mutex_lock(&mutexMetricas);
		tablaMetricas.cantidadTareasTotalesReduccionGlobal = cantidadSolicitudes;
		pthread_mutex_unlock(&mutexMetricas);
	}

void mandarDatosReduccionGlobal(t_indicacionReduccionGlobal * indicacion , int conexionEncargado)
{
	enviarIndicacionReduccionGlobal(conexionEncargado,indicacion);
}


void gestionarAlmacenadoFinal(t_pedidoAlmacenadoFinal * pedido)
{

	t_indicacionAlmacenadoFinal * indicacion = malloc (sizeof(t_indicacionAlmacenadoFinal));
	indicacion->rutaArchivoReduccionGlobal = pedido->archivoReduccionGlobal; //HAY QUE SACARLE IP Y PUERTO A INDICACION
	//indicacion->rutaAlmacenadoFinal = rutaArchivoParaAlmacenar     ---> es la ruta que viene por el main . Mandar al worker???
	int conexionWorker = conectarCliente(pedido->ip,pedido->puerto,WORKER);

	enviarIndicacionAlmacenadoFinal(conexionWorker,indicacion);
	enviarMensaje(conexionYama, "termino todo wacho");

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
		t_pedidoTransformacion * indicacionTransformacion = recibirSolicitudTransformacion(unPaquete);
		char* continuarRecibiendoPedidosTransformacion = "1";// recibirMensaje(unPaquete); // preg facu
		list_add(pedidosDeTransformacion,indicacionTransformacion); //leno lista con pedidos
		if (string_equals_ignore_case(continuarRecibiendoPedidosTransformacion, "0"))
		{
			gestionarTransformacion();
			finDeSolicitudes = true;
		}
		break;// sale para volver a pedir


	case ENVIAR_INDICACION_REDUCCION_LOCAL: //guarda, acá recibe una INDICACIONDEREDULOCAL, no una solicitud

		 ;
		t_indicacionReduccionLocal * indicacionReduLocal = recibirIndicacionReduccionLocal(unPaquete); //ACA TIENE QUE RECIBIR LA INDICACION
		char* continuarRecibiendoPedidosDeReduLocal= "1";//recibirMensaje(unPaquete); // aca que onda? --> preguntar facu
		list_add(pedidosDeReduccionLocal,indicacionReduLocal);

		if(string_equals_ignore_case(continuarRecibiendoPedidosDeReduLocal,"0"))
		{
			gestionarReduccionLocal();
			finDeSolicitudes = true;
		}
		break;


	case ENVIAR_INDICACION_REDUCCION_GLOBAL:
	 ;
		t_indicacionReduccionGlobal * indicacionesParaReduccionGlobal = recibirIndicacionReduccionGlobal(unPaquete);
	    list_add(pedidosDeReduccionGlobal, indicacionesParaReduccionGlobal);

	    char* continuarRecibiendo = "1";//recibirMensaje(unPaquete);
	    if(string_equals_ignore_case(continuarRecibiendo, "0"))
	    {

	    	gestionarReduccionGlobal();
	    	finDeSolicitudes = true;

	    }


		break;


	case ENVIAR_INDICACION_ALMACENADO_FINAL:
		 ;
		 t_pedidoAlmacenadoFinal * indicacionAlmacenamientoFinal = recibirSolicitudAlmacenadoFinal(unPaquete);
		 gestionarAlmacenadoFinal(indicacionAlmacenamientoFinal);
		break;


	default:
		break;
	}

	destruirPaquete(unPaquete);
}

