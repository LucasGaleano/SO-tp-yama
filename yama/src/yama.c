#include "yama.h"

int main(void) {

	//Levanto el archivo de configuracion
	char* path_config_yama =
			"/home/utnso/workspace/tp-2017-2c-NULL/configuraciones/yama.cfg";

	logYama = log_create("yama.log", "Yama", true, LOG_LEVEL_TRACE);

	configuracion = leerArchivoDeConfiguracionYAMA(path_config_yama);

	log_trace(logYama, "Arranca proceso Yama");

	//Me conecto con el file system
	socketFS = conectarCliente(configuracion->ip, configuracion->puerto, YAMA); //todo VERIFICAR CONEXION CON FS ROMPER

	//Creo estructuras administrativas
	listaDireccionesNodos = list_create();
	idJob = 0;
	tabla_de_estados = list_create();
	tablaPlanificador = Planificador_create();

	//Creo el thread para escuchar conexiones
	pthread_t threadServerYama;

	if (pthread_create(&threadServerYama, NULL, (void*) iniciarServidor,
			configuracion->puerto_yama)) {

		perror("Error el crear el thread servidor.");
		exit(EXIT_FAILURE);
	}

	pthread_join(threadServerYama, NULL);

	destruirConfiguracion(configuracion);
//	list_destroy_and_destroy_elements(tabla_de_estados, (void*) eliminarElemento() ); //TODO PREGUNTAR SI ESTA BIEN LIBERAR DESPUES DE USAR

	return EXIT_SUCCESS;
}

/*------------------------Configuracion proyecto-------------------------*/
t_configuracion * leerArchivoDeConfiguracionYAMA(char* path) {

	t_config * config = config_create(path);

	configuracion = malloc(sizeof(t_configuracion));

	configuracion->ip = strdup(config_get_string_value(config, "FS_IP"));

	configuracion->puerto = strdup(config_get_string_value(config, "FS_PUERTO"));

	configuracion->retardo = config_get_int_value(config, "RETARDO_PLANIFICACION");

	configuracion->algoritmo = strdup(config_get_string_value(config,"ALGORITMO_BALANCEO"));

	configuracion->disponibilidad_base = config_get_int_value(config,"DISPONIBILIDAD_BASE");

	configuracion->puerto_yama = strdup(config_get_string_value(config, "PUERTO_YAMA"));

	printf(
			"Se levanto el proceso YAMA con: YAMA_PUERTO: %s  FS_IP: %s - FS_PUERTO: %s - RETARDO: %d - ALGORITMO: %s - DISPONIBILIDAD BASE: %d \n",
			configuracion->puerto_yama,
			configuracion->ip,
			configuracion->puerto,
			configuracion->retardo,
			configuracion->algoritmo,
			configuracion->disponibilidad_base);

	config_destroy(config);

	return configuracion;
}

/*-------------------------Manejo de conexiones-------------------------*/
void iniciarServidor(char* unPuerto) {
	iniciarServer(unPuerto, (void *) procesarPaquete, logYama);
}

/*-------------------------Procesamiento paquetes-------------------------*/
void procesarPaquete(t_paquete * unPaquete, int * client_socket) {
	switch (unPaquete->codigoOperacion) {
	case HANDSHAKE:
		procesarRecibirHandshake(unPaquete, client_socket);
		break;
	case ENVIAR_MENSAJE:
		procesarRecibirMensaje(unPaquete);
		break;
	case ENVIAR_ARCHIVO:
		procesarRecibirArchivo(unPaquete);
		break;
	case ENVIAR_ERROR:
		procesarRecibirError(unPaquete);
		break;
	case ENVIAR_RUTA_PARA_ARRANCAR_TRANSFORMACION: //RECIBO SOLICITUD DE TRANSFORMACION CON PATH DE ARCHIVO
		procesarEnviarSolicitudTransformacion(unPaquete, client_socket); //ENVIO A FS PATH DE ARCHIVO
		break;
	case ENVIAR_LISTA_NODO_BLOQUES: //RECIBO LISTA DE ARCHIVOS DE FS CON UBICACIONES Y BLOQUES
		procesarEnviarListaNodoBloques(unPaquete); //
		break;
	case ENVIAR_INDICACION_TRANSFORMACION:
		procesarResultadoTranformacion(unPaquete, client_socket);
		break;
	case ENVIAR_INDICACION_REDUCCION_LOCAL:
		procesarResultadoReduccionLocal(unPaquete, client_socket);
		break;
	case TAREA_COMPLETADA:
		procesarTareaCompletada(unPaquete, client_socket);
		break;

	default:
		break;
	}
	destruirPaquete(unPaquete);
}

void procesarRecibirHandshake(t_paquete * unPaquete, int * client_socket) {
	int tipoCliente = recibirHandshake(unPaquete);

	switch (tipoCliente) {
	case MASTER:
		;
		int * a = malloc(sizeof(int));
		memcpy(a, client_socket, sizeof(int));
		//TODO CHECKEAR HANDSHAKE
		break;
	default:
		*client_socket = -1;
		break;
	}
}

void procesarRecibirMensaje(t_paquete * unPaquete) {
//char * mensaje = recibirMensaje(unPaquete); todo Comento para que no tire error
}

void procesarRecibirArchivo(t_paquete * unPaquete) {
//void * archivo = recibirArchivo(unPaquete);  todo Comento para que no tire error
}

void procesarRecibirError(t_paquete * unPaquete) { //supuestamente necesita un socket

	int error = recibirError(unPaquete);

	switch (error) {
	case ERROR_REDUCCION_LOCAL:
		printf("[-]fallo reduccion local"); //todo agregar error a todos los job
		exit(EXIT_FAILURE);

		break;
	case ERROR_REDUCCION_GLOBAL:
		printf("[-]fallo reduccion global");
		exit(EXIT_FAILURE);
		break;

	case ERROR_ALMACENAMIENTO_FINAL:
		printf("[-]fallo almacenamiento final");
		exit(EXIT_FAILURE);
		break;
	default:

		break;
	}

}

void procesarEnviarSolicitudTransformacion(t_paquete * unPaquete, int *client_socket) {
	char * nomArchivo = recibirMensaje(unPaquete);
	log_trace(logYama, "Recibida ruta de Archivo:  %s", nomArchivo);
	enviarRutaParaArrancarTransformacion(socketFS, nomArchivo, client_socket);
	log_trace(logYama, "Enviada ruta para obtener Nodos y Bloques a: %d", socketFS);
}

void procesarEnviarListaNodoBloques(t_paquete * unPaquete) {
	log_trace(logYama, "Recibida lista de bloques y nodos de File System");
	int idJob = generarJob();

	t_nodos_bloques * nodosBloques = recibirListaNodoBloques(unPaquete); //RECIBO UN STRUCT CON 2 LISTAS ANIDADAS

	t_list* listaNodoBloque = nodosBloques->nodoBloque;
	log_trace(logYama, "Recibido %d nodos-bloques de FilesSystem", listaNodoBloque->elements_count);
	listaDireccionesNodos = list_take(nodosBloques->puertoIP,nodosBloques->puertoIP->elements_count);

	t_list* listaBloquesConNodos = agruparNodosPorBloque(listaNodoBloque); // LISTA DE BLOQUES CON LOS NODOS DONDE ESTA
	t_list* nodosSinRepetidos = extraerNodosSinRepetidos(listaNodoBloque); //SOLO LOS NOMBRE NODOS SIN REPETIDOS

	void agregarATablaPlanificador(char* nombreNodo) {
		planificador_agregarWorker(tablaPlanificador, nombreNodo);
	}

	list_iterate(nodosSinRepetidos, (void*) agregarATablaPlanificador);

	planificador(configuracion->algoritmo, listaBloquesConNodos,
			tablaPlanificador, configuracion->disponibilidad_base);

	t_list* indicacionesDeTransformacionParaMaster = list_create();

	void armarIndicacionDeTransformacionPorRegistro(
			t_registro_Tabla_Planificador* registroTabla) {
		t_indicacionTransformacion* indicacionTransformacion = malloc(
				sizeof(t_indicacionTransformacion)); //TODO LIBERAR

		int idNodo = registroTabla->id;
		char* nombreNodo = obtenerNombreNodoDesdeId(idNodo);

		t_puerto_ip* direccionNodo = buscarIpYPuertoConNombreNodo(nombreNodo,
				listaDireccionesNodos);
		int tamanioArchivo = buscarTamanioArchivoConNombreNodo(nombreNodo,
				listaNodoBloque);

		void armarIndicacionDeTransformacionPorBloque(int numeroBloque) {
			indicacionTransformacion->estado = PROCESANDO;
			indicacionTransformacion->ip = direccionNodo->ip;
			indicacionTransformacion->puerto = direccionNodo->puerto;
			indicacionTransformacion->nodo = nombreNodo; //TODO REVISAR SI HAY QUE RESERVAR MEMORIA
			indicacionTransformacion->bytes = tamanioArchivo;

			int bloqueNodo = mapearBloqueArchivoABloqueNodo(listaNodoBloque,
					nombreNodo, numeroBloque);
			indicacionTransformacion->bloque = bloqueNodo;
			indicacionTransformacion->rutaArchivoTemporal = nombreArchivoTemp(
					prefijoArchivosTemporalesTranformacion);

			list_add(indicacionesDeTransformacionParaMaster,
					indicacionTransformacion);
		}
		list_iterate(registroTabla->listaBloques,
				(void*) armarIndicacionDeTransformacionPorBloque);
	}
	list_iterate(tablaPlanificador,
			(void*) armarIndicacionDeTransformacionPorRegistro);

	void registrarYEnviarAMaster(
			t_indicacionTransformacion* indicacionDeTransformacion) {

		//REGISTRAR EN TABLA DE ESTADO EL JOB
		agregarRegistro(idJob, nodosBloques->masterSolicitante,
				indicacionDeTransformacion->nodo,
				indicacionDeTransformacion->bloque, TRANSFORMACION,
				indicacionDeTransformacion->rutaArchivoTemporal, PROCESANDO);

		//ENVIAR A MASTER LA INDICACION
		enviarIndicacionTransformacion(nodosBloques->masterSolicitante,
				indicacionDeTransformacion);
	}

	list_iterate(indicacionesDeTransformacionParaMaster, (void*) registrarYEnviarAMaster);
}

void procesarResultadoTranformacion(t_paquete * unPaquete, int *client_socket) {
	t_indicacionTransformacion* resultado = recibirIndicacionTransformacion(
			unPaquete);				//todo falta librerar resultado

	//ACTUALIZAR REGISTRO Y CONTINUAR O REPLANIFICAR ALGUN BLOQUE (VER REPLANIFICACION)
	//todo FIJARSE QUE PASA CON LA TABLA DE ESTADO EN EL CASO DE QUE FALLE ALGUNA ETAPA

	if (resultado->estado == FINALIZADO_OK) {

		planificador_sumarWLWorker(tablaPlanificador,
				extraerIddelNodo(resultado->nodo), -1); //le saco carga de trabaja al nodo

		modificarEstadoDeRegistro(-1, *client_socket, resultado->nodo,
				resultado->bloque, TRANSFORMACION, FINALIZADO_OK);

		//MIRAR SI PARA UN MISMO NODO, TERMINARON TODAS LAS TRANSFORMACIONES

		if (buscarRegistro(-1, -1, resultado->nodo, -1, TRANSFORMACION,
				PROCESANDO, NULL) == NULL) {
				//TODO POR QUE UN MISMO NODO ????
			//SI -> MANDAR A HACER TODAS LAS REDUCCIONES LOCALES DE ESE NODO

			//recorrer la tabla registros y enviar paquete reduccion local por cada nodo terminado

			int i = 0;
			int tam = list_size(tabla_de_estados);
			while (tam > i) {

				t_elemento_tabla_estado * reg = list_get(tabla_de_estados, i);

				if (string_equals_ignore_case(reg->nodo, resultado->nodo)) {
					t_indicacionReduccionLocal* indReducLocal = malloc(
							sizeof(t_indicacionReduccionLocal));
					indReducLocal->nodo = string_duplicate(resultado->nodo);
					indReducLocal->ip = string_duplicate(resultado->ip);
					indReducLocal->puerto = string_duplicate(resultado->puerto);
					indReducLocal->archivoTemporalTransformacion =
							string_duplicate(resultado->rutaArchivoTemporal);
					indReducLocal->archivoTemporalReduccionLocal =
							nombreArchivoTemp(
									prefijoArchivosTemporalesReduLocal);

					//ACTUALIZAR TABLA DE ESTADO AVANZANDO LA ETAPA

					agregarRegistro(reg->job, *client_socket,
							indReducLocal->nodo, reg->bloque, REDUCCION_LOCAL,
							indReducLocal->archivoTemporalReduccionLocal,
							PROCESANDO);

					enviarIndicacionReduccionLocal(*client_socket,
							indReducLocal);
					IndicReducLocal_destroy(indReducLocal);

				}
				i++;
			}

		}

	}
}

void procesarResultadoReduccionLocal(t_paquete* unPaquete, int *client_socket) {

	t_indicacionReduccionLocal * indicReduLocal = recibirIndicacionReduccionLocal(unPaquete);

	modificarEstadoDeRegistro(-1, -1, indicReduLocal->nodo, -1, REDUCCION_LOCAL, FINALIZADO_OK);

	t_elemento_tabla_estado* registro = buscarRegistro(-1, -1, NULL, -1, -1, -1, indicReduLocal->archivoTemporalTransformacion);

	if (buscarRegistro(registro->job, -1, indicReduLocal->nodo, -1,REDUCCION_LOCAL, PROCESANDO, NULL) == NULL) {

		enviarTareaCompletada(*client_socket, NO_CONTINUA);

		t_list* listaDeIndicacionesReduccionGlobal = list_create();

		char* nombreArchivoReduccionGlobal = nombreArchivoTemp(prefijoArchivosTemporalesReduGlobal);

		void siCumpleCondicionArmarIndicacionDeReduccionGlobal(t_elemento_tabla_estado* elemento){
			if(elemento->job == registro->job && elemento->etapa == REDUCCION_LOCAL && elemento->estado == FINALIZADO_OK){
				t_indicacionReduccionGlobal* indicacionReduccionGlobal = malloc(sizeof(t_indicacionReduccionGlobal));//TODO LIBERAR MEMORIA
				//todo ver lo del encargado

				int esEncargadoReduccionGlobal(char* nodo){
					char* nodoEncargado = obtenerEncargadoReduccionGlobal();
					return string_equals_ignore_case(nodo, nodoEncargado);
				}

				indicacionReduccionGlobal->encargado = esEncargadoReduccionGlobal(elemento->nodo);
				indicacionReduccionGlobal->archivoDeReduccionGlobal = string_duplicate(nombreArchivoReduccionGlobal);
				indicacionReduccionGlobal->archivoDeReduccionLocal = string_duplicate(elemento->nombreArchivoTemporal);
				indicacionReduccionGlobal->nodo = string_duplicate(elemento->nodo);
				t_puerto_ip* PyIP = buscarIpYPuertoConNombreNodo(elemento->nodo, listaDireccionesNodos);
				indicacionReduccionGlobal->ip = string_duplicate(PyIP->ip);
				indicacionReduccionGlobal->puerto = string_duplicate(PyIP->puerto);
				//todo liberar

				list_add(listaDeIndicacionesReduccionGlobal,indicacionReduccionGlobal);
			}
		}

		list_iterate(tabla_de_estados, (void*)siCumpleCondicionArmarIndicacionDeReduccionGlobal);

		enviarIndicacionReduccionGlobal(*client_socket , listaDeIndicacionesReduccionGlobal);
	}

}

void procesarTareaCompletada( unPaquete, client_socket) {

}

/*-------------------------Funciones auxiliares-------------------------*/

void destruirIndicacionDeTransformacion(
		t_indicacionTransformacion* indTransform) {
	free(indTransform->nodo);
	free(indTransform->ip);
	free(indTransform->puerto);
	free(indTransform->rutaArchivoTemporal);

	free(indTransform);
}

//TODO funcion destruir el t_indicacionTranformacion

long generarJob() {
	idJob += 1;
	return idJob;
}

t_list* agruparNodosPorBloque(t_list* listaDeNodoBloque) {
	t_list* bloquesSinRepetidos = list_create(); //TODO LIBERAR LISTA

	bool existeNodoEnLaLista(int numeroBloque) {
		bool booleano = false;
		int x = 0;
		for (; x < bloquesSinRepetidos->elements_count; x++) {
			if (list_get(bloquesSinRepetidos, x), numeroBloque) {
				booleano = true;
				break;
			}
		}
		return booleano;
	}

	void tomarBloquesSinRepetidos(t_nodo_bloque* t_nodo_bloque) {
		if (!existeNodoEnLaLista(t_nodo_bloque->bloqueArchivo)) {
			list_add(bloquesSinRepetidos, &t_nodo_bloque->bloqueArchivo);
		}
	}

	list_iterate(listaDeNodoBloque, (void*) tomarBloquesSinRepetidos);

	t_list* listaBloquesConListaDeNodos = list_create(); //TODO LIBERAR MEMORIA

	void obtenerNodosDeBloque(int numeroBloque) {
		t_nodos_por_bloque* bloqueConListaNodos = malloc(sizeof(t_nodos_por_bloque)); //TODO LIBERAR MEMORIA
		bloqueConListaNodos->nodosEnLosQueEsta = list_create();
		int y = 0;
		for (; y < listaDeNodoBloque->elements_count; y++) {
			t_nodo_bloque* nodoBloque = list_get(listaDeNodoBloque, y);
			if (numeroBloque == nodoBloque->bloqueArchivo) {
				list_add(bloqueConListaNodos->nodosEnLosQueEsta,
						nodoBloque->nomNodo);
			}
		}
		list_add(listaBloquesConListaDeNodos, bloqueConListaNodos);
	}

	list_iterate(bloquesSinRepetidos, (void*) obtenerNodosDeBloque);

	return listaBloquesConListaDeNodos;
}

t_list* extraerNodosSinRepetidos(t_list* listaDeNodoBloque) {

	t_list* nodosSinRepetidos = list_create(); //TODO LIBERAR LISTA

	bool existeNodoEnLaLista(char* nombreNodo) {
		bool booleano = false;
		int x = 0;
		for (; x < nodosSinRepetidos->elements_count; x++) {
			if (string_equals_ignore_case(list_get(nodosSinRepetidos, x),
					nombreNodo)) {
				booleano = true;
				break;
			}
		}
		return booleano;
	}

	void tomarNodosSinRepetidos(t_nodo_bloque* t_nodo_bloque) {
		if (!existeNodoEnLaLista(t_nodo_bloque->nomNodo)) {
			list_add(nodosSinRepetidos, t_nodo_bloque->nomNodo);
		}
	}

	list_iterate(listaDeNodoBloque, (void*) tomarNodosSinRepetidos);

	return nodosSinRepetidos;
}

void destruirConfiguracion(t_configuracion * configuracion) {
	free(configuracion->algoritmo);
	free(configuracion->ip);
	free(configuracion->puerto);
	free(configuracion->puerto_yama);
	free(configuracion);
}

char* obtenerNombreNodoDesdeId(int idNodo) {
	char* prefijoNodo = string_new();
	char* numeroNodo = string_itoa(idNodo);
	string_append(&prefijoNodo, numeroNodo);
	free(numeroNodo);
	return prefijoNodo;
}

void IndicReducLocal_destroy(t_indicacionReduccionLocal* indReducLocal) {

	free(indReducLocal->archivoTemporalReduccionLocal);
	free(indReducLocal->archivoTemporalTransformacion);
	free(indReducLocal->ip);
	free(indReducLocal->nodo);
	free(indReducLocal->puerto);
	free(indReducLocal);
}

