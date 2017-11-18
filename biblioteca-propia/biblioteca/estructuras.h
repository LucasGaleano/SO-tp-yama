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

	ENVIAR_MENSAJE=1,
	ENVIAR_ARCHIVO=2,
	ENVIAR_INFO_DATANODE=3,
	ENVIAR_ERROR=4,
	ENVIAR_BLOQUE=5,

	ENVIAR_SOLICITUD_LECTURA_BLOQUE=6,
	ENVIAR_SOLICITUD_ESCRITURA_BLOQUE=7,

	ENVIAR_SOLICITUD_TRANSFORMACION=8,

	ENVIAR_SOLICITUD_REDUCCION_LOCAL=9,
	ENVIAR_SOLICITUD_REDUCCION_GLOBAL=10,

	ENVIAR_SOLICITUD_ALMACENADO_FINAL=11,

	ENVIAR_INDICACION_TRANSFORMACION=12,

	ENVIAR_INDICACION_REDUCCION_LOCAL=13,
	ENVIAR_INDICACION_REDUCCION_GLOBAL=14,

	ENVIAR_INDICACION_ALMACENADO_FINAL=15,

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

#endif /* BIBLIOTECA_ESTRUCTURAS_H_ */
