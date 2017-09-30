#ifndef SRC_PROCESAMIENTOPAQUETES_H_
#define SRC_PROCESAMIENTOPAQUETES_H_

#include "estructuras.h"
#include "serializacion.h"

/*-------------------------Paquetes-------------------------*/
void	 			enviarPaquetes					(int, t_paquete *);
int 				recibirTamPaquete				(int, fd_set *);
t_paquete * 		recibirPaquete					(int, fd_set *, int);
t_paquete * 		crearPaquete					(void *);
t_paquete *			crearPaqueteError				(int client_socket);
void 				destruirPaquete					(t_paquete *);
void 				mostrarPaquete					(t_paquete *);

/*-------------------------Enviar paquetes-------------------------*/
void 				enviarHandshake					(int, int);
void 				enviarMensaje					(int, char *);
void 				enviarArchivo					(int, char *);
void 				enviarInfoDataNode				(int, char *, int, int);

/*-------------------------Recibir paquetes-------------------------*/
void 				recibirMensaje					(t_paquete *);
void 				recibirArchivo					(t_paquete *);

#endif /* SRC_PROCESAMIENTOPAQUETES_H_ */
