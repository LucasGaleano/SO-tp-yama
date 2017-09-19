#ifndef BIBLIOTECA_SOCKETS_H_
#define BIBLIOTECA_SOCKETS_H_

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

/*------------------------------Clientes------------------------------*/

int 			conectarCliente				(const char *, const char *);
void 			enviarMensaje				(int, char *);
void 			enviarPaquetes				(int);

/*------------------------------Servidor------------------------------*/

int 			crearSocketServer			(const char *);
void 			gestionarDatosCliente		(int, void *, fd_set *);
void 			gestionarNuevasConexiones	(int, fd_set *, int *);
void 			iniciarServer				(const char *);

#endif /* BIBLIOTECA_SOCKETS_H_ */
