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


/*------------------------------Clientes------------------------------*/
int		 			conectarCliente					(const char *, const char *);

/*------------------------------Servidor------------------------------*/
void	 			iniciarServer					(const char *);
int 				crearSocketServer				(const char *);
void 				gestionarDatosCliente			(int, fd_set *);
void 				gestionarNuevasConexiones		(int, fd_set *, int *);

/*------------------------------Paquetes------------------------------*/
void	 			enviarPaquetes					(int, t_paquete *);
int 				recibirTamPaquete				(int, fd_set *);
t_paquete * 		recibirPaquete					(int, fd_set *, int);
t_paquete * 		crearPaquete					(void *);
void 				destruirPaquete					(t_paquete *);
void 				mostrarPaquete					(t_paquete *);

#endif /* BIBLIOTECA_SOCKETS_H_ */
