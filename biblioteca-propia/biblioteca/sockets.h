#ifndef BIBLIOTECA_SOCKETS_H_
#define BIBLIOTECA_SOCKETS_H_

#include "estructuras.h"
#include "procesamientoPaquetes.h"

/*------------------------------Clientes------------------------------*/
int		 			conectarCliente					(const char *, const char *);

/*------------------------------Servidor------------------------------*/
void	 			iniciarServer					(const char *);
int 				crearSocketServer				(const char *);
void 				gestionarDatosCliente			(int, fd_set *);
void 				gestionarNuevasConexiones		(int, fd_set *, int *);

#endif /* BIBLIOTECA_SOCKETS_H_ */
