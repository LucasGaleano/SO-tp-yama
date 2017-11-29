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

	RESPUESTA_INFO_ARCHIVO

};

enum tipoArchivo{
	TEXTO=0,
	BINARIO=1,
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
	char * nodo;
}t_lecturaGenerarCopia;

typedef struct {
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
	char* ip;
	char* puerto;
	int bloque;
	char* rutaArchivoTemporal;
} t_pedidoTransformacion;

typedef struct {
	char* archivoTransformacion;
	char* archivoReduccionLocal;
} t_pedidoReduccionLocal;

typedef struct {
	char* nodo;
	char* ip;
	char* puerto;
	char* archivoReduccionPorWorker;
	char* workerEncargdo;
	char* ArchivoResultadoReduccionGlobal;
} t_pedidoReduccionGlobal;

typedef struct {
	char* ip;
	char* puerto;
	char* archivoReduccionGlobal;
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
	char * ubicacion1;
	char * ubicacion2;
	int tamanioArchivo;
} t_informacion_bloque;

//Verificados


/*------------------------------Estructuras de comunicacion Yama File System------------------------------*/
typedef struct {
	char* rutaArchivo;
} t_solicitudArchivo;

#endif /* BIBLIOTECA_ESTRUCTURAS_H_ */
