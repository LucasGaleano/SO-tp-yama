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
};

#endif /* BIBLIOTECA_ESTRUCTURAS_H_ */
