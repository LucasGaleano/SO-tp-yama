#ifndef BIBLIOTECA_ESTRUCTURAS_H_
#define BIBLIOTECA_ESTRUCTURAS_H_

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/*------------------------------Estructuras------------------------------*/
//#define TAM_BLOQUE 1048576
#define TAM_BLOQUE 1024

typedef struct {
	size_t size;
	void * data;
} t_stream;

typedef struct {
	int codigoOperacion;
	t_stream * buffer;
}t_paquete;

enum emisor {
	DATANODE = 900,
	FILESYSTEM = 901,
	MASTER = 902,
	WORKER = 903,
	YAMA = 904,
};






enum cod_op{
	HANDSHAKE=0,

	ENVIAR_MENSAJE,
	ENVIAR_RUTA_ARCHIVO,
	ENVIAR_ARCHIVO,
	ENVIAR_INFO_DATANODE,
	ENVIAR_ERROR,

	ENVIAR_BLOQUE,
	ENVIAR_BLOQUE_GENERAR_COPIA,
	ENVIAR_BLOQUE_ARCHIVO_TEMPORAL,

	ENVIAR_SOLICITUD_LECTURA_BLOQUE,
	ENVIAR_SOLICITUD_LECTURA_BLOQUE_GENERAR_COPIA,
	ENVIAR_SOLICITUD_LECTURA_ARCHIVO_TEMPORAL,

	ENVIAR_SOLICITUD_ESCRITURA_BLOQUE,

	ENVIAR_RESPUESTA_ESCRITURA_BLOQUE,

	ENVIAR_SOLICITUD_TRANSFORMACION,

	ENVIAR_SOLICITUD_REDUCCION_LOCAL,
	ENVIAR_SOLICITUD_REDUCCION_GLOBAL,

	ENVIAR_SOLICITUD_ALMACENADO_FINAL,

	ENVIAR_INDICACION_TRANSFORMACION,

	ENVIAR_INDICACION_REDUCCION_LOCAL,
	ENVIAR_INDICACION_REDUCCION_GLOBAL,

	ENVIAR_INDICACION_ALMACENADO_FINAL,


	ENVIAR_RUTA_PARA_ARRANCAR_TRANSFORMACION,

	CONTINUA_MENSAJES,

	TAREA_COMPLETADA,

	RESPUESTA_INFO_ARCHIVO,

	INDICACION_TERMINO_TAREA,
};

enum continuaMensaje{
	CONTINUA=0,
	NO_CONTINUA=1,

};

enum tipoArchivo{
	TEXTO=0,
	BINARIO=1,
};

enum error{
	ERROR_TRANSFORMACION =-1,
	ERROR_REDUCCION_LOCAL = -2,
	ERROR_REDUCCION_GLOBAL = -3,
	ERROR_ALMACENAMIENTO_FINAL = -4

};

/*------------------------------Estructuras de comunicacion FS DataNode------------------------------*/
typedef struct {
	void* data;
	int numBloque;
}t_pedidoEscritura;

typedef struct {
	bool exito;
	int numBloque;
}t_respuestaEscritura;

typedef struct {
	int numBloque;
	int orden;
}t_lecturaArchTemp;

typedef struct {
	void * data;
	int orden;
}t_respuestaLecturaArchTemp;

typedef struct {
	int bloque;
	char * rutaArchivo;
	char * nodoBuscado;
	char * nodoAEscribir;
}t_lecturaGenerarCopia;

typedef struct {
	int bloque;
	void * data;
	char * rutaArchivo;
	char * nodo;
}t_respuestaLecturaGenerarCopia;

typedef struct {
	char * nombre;
	int total;
	int libre;
} t_nodo_info;

/*------------------------------Estructuras de comunicacion Yama Master------------------------------*/
typedef struct {
	char* rutaScriptTransformacion;
	int   numBloque;
	char* rutaArchivoTemporal;
	int   cantBytes;
} t_pedidoTransformacion;

typedef struct {
	char* archivoTransformacion;
	char* archivoReduccionLocal;
	char* rutaScript;
} t_pedidoReduccionLocal;

typedef struct {
	char* ip;
	char* puerto;
	char* archivoReduccionPorWorker;
	int workerEncargado;
	char* ArchivoResultadoReduccionGlobal;
	int cantWorkerInvolucradros;
} t_pedidoReduccionGlobal;

typedef struct {
	char* archivoReduccionGlobal;
	char* rutaAlmacenadoFinal;
} t_pedidoAlmacenadoFinal;

typedef struct {
	char* nodo;
	char* ip;
	char* puerto;
	int bloque;
	int bytes;
	char* rutaArchivoTemporal;
} t_indicacionTransformacion;

typedef struct {
	char* nodo;
	char* ip;
	char* puerto;
	char* archivoTemporalTransformacion; // Existe
	char* archivoTemporalReduccionLocal; // Nuevo
} t_indicacionReduccionLocal;

typedef struct {
	char* nodo;
	char* ip;
	char* puerto;
	char* archivoDeReduccionLocal;
	char* archivoDeReduccionGlobal;
	char* encargado;
} t_indicacionReduccionGlobal;

typedef struct {
	char* nodo;
	char* ip;
	char* puerto;
	char* rutaArchivoReduccionGlobal;
} t_indicacionAlmacenadoFinal;


typedef struct{
	char* ip;
	char* nodo;
	char* puerto;
	int numBloque;
} t_bloque_ubicacion;

typedef struct{
	t_bloque_ubicacion* copia0;
	t_bloque_ubicacion* copia1;
	int tamanioOcupado;
} t_bloque;

//Verificados


/*------------------------------Estructuras de comunicacion Yama File System------------------------------*/
typedef struct {
	char* rutaArchivo;
} t_solicitudArchivo;

#endif /* BIBLIOTECA_ESTRUCTURAS_H_ */
