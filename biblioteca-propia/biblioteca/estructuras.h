#ifndef BIBLIOTECA_ESTRUCTURAS_H_
#define BIBLIOTECA_ESTRUCTURAS_H_

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/*------------------------------Estructuras------------------------------*/
typedef struct {
	int size;
	void * data;
} t_stream;

typedef struct {
	int emisor;
	int codigoOperacion;
	t_stream * buffer;
}t_paquete;

enum cod_op{
	ENVIAR_MENSAJE=0,
};

enum emisor {
	DATANODE = 900,
	FILESYSTEM = 901,
	MASTER = 902,
	WORKER = 903,
	YAMA = 904,
};


#endif /* BIBLIOTECA_ESTRUCTURAS_H_ */
