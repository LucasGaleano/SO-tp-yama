#ifndef BIBLIOTECA_SOCKETS_H_
#define BIBLIOTECA_SOCKETS_H_

#include "estructuras.h"
#include "paquetes.h"

/*------------------------------Clientes------------------------------*/
int		 			conectarCliente					(const char *, const char *, int);
void 				gestionarSolicitudes			(int, void(*)(void*, int*));

/*------------------------------Servidor------------------------------*/
void	 			iniciarServer					(const char *,void(*)(void*, int*));
int 				crearSocketServer				(const char *);
void 				gestionarNuevasConexiones		(int, fd_set *, int *);
void 				gestionarDatosCliente			(int, fd_set *, void(*)(void*, int*));


#endif /* BIBLIOTECA_SOCKETS_H_ */
