#ifndef SRC_PROCESAMIENTOPAQUETES_H_
#define SRC_PROCESAMIENTOPAQUETES_H_

#include "estructuras.h"
#include "serializacion.h"
#include "estructurasMasterYama.h"

/*-------------------------Paquetes-------------------------*/
void	 					enviarPaquetes							(int, t_paquete *);
int 						recibirTamPaquete						(int);
t_paquete * 				recibirPaquete							(int, int);
t_paquete * 				crearPaquete							(void *);
t_paquete *					crearPaqueteError						(int client_socket);
void 						destruirPaquete							(t_paquete *);
void 						mostrarPaquete							(t_paquete *);

/*-------------------------Enviar paquetes-------------------------*/
void 						enviarHandshake							(int, int);
void 						enviarMensaje							(int, char *);
void 						enviarArchivo							(int, char *);
void 						enviarInfoDataNode						(int, char *, int, int);
void 						enviarBloque							(int, char*);
void 						enviarSolicitudLecturaBloque			(int, int);
void 						enviarSolicitudEscrituraBloque			(int, void*,int);
void 						enviarSolicitudTransformacion			(int, t_pedidoTransformacion *);
void 						enviarSolicitudPedidoReduccionLocal		(int, t_pedidoReduccionLocal *);
void 						enviarSolicitudPedidoReduccionGlobal	(int, t_pedidoReduccionGlobal *);
void 						enviarSolicitudPedidoAlmacenadoFinal	(int, t_pedidoAlmacenadoFinal *);

/*-------------------------Recibir paquetes-------------------------*/
void 						recibirMensaje							(t_paquete *);
void 						recibirArchivo							(t_paquete *);
void 						recibirBloque							(t_paquete *);
int 						recibirSolicitudLecturaBloque			(t_paquete *);
t_pedidoEscritura* 			recibirSolicitudEscrituraBloque			(t_paquete *);
t_pedidoTransformacion* 	recibirSolicitudTransformacion			(t_paquete *);
t_pedidoReduccionLocal* 	recibirSolicitudPedidoReduccionLocal	(t_paquete *);
t_pedidoReduccionGlobal*	recibirSolicitudPedidoReduccionGlobal	(t_paquete *);
t_pedidoAlmacenadoFinal* 	recibirSolicitudPedidoAlmacenadoFinal	(t_paquete *);

#endif /* SRC_PROCESAMIENTOPAQUETES_H_ */
