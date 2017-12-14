#ifndef BIBLIOTECA_SOCKETS_H_
#define BIBLIOTECA_SOCKETS_H_

#include "estructuras.h"
#include "paquetes.h"
#include <commons/log.h>

/*------------------------------Clientes------------------------------*/
int		 			conectarCliente					(const char *, const char *, int);
void 				gestionarSolicitudes			(int, void(*)(void*, int*), t_log *);

/*------------------------------Servidor------------------------------*/
void	 			iniciarServer					(const char *,void(*)(void*, int*), t_log *);
int 				crearSocketServer				(const char *);
void 				gestionarNuevasConexiones		(int, fd_set *, int *, t_log *);
void 				gestionarDatosCliente			(int, fd_set *, void(*)(void*, int*), t_log *);


#endif /* BIBLIOTECA_SOCKETS_H_ */
