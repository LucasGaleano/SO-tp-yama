#ifndef SRC_PROCESAMIENTOPAQUETES_H_
#define SRC_PROCESAMIENTOPAQUETES_H_

#include "estructuras.h"
#include "serializacion.h"

/*-------------------------Paquetes-------------------------*/
void	 			enviarPaquetes					(int, t_paquete *);
int 				recibirTamPaquete				(int, fd_set *);
t_paquete * 		recibirPaquete					(int, fd_set *, int);
t_paquete * 		crearPaquete					(void *);
void 				destruirPaquete					(t_paquete *);
void 				mostrarPaquete					(t_paquete *);

/*-------------------------Enviar paquetes-------------------------*/

void 				enviarMensaje					(int, int, char *);
void 				enviarArchivo					(int, int, char *);

/*-------------------------Recibir paquetes-------------------------*/












//Procesamiento de paquetes
void procesarPaqueteDataNode(t_paquete * unPaquete, int socket);
void procesarPaqueteFileSystem(t_paquete * unPaquete, int socket);
void procesarPaqueteMaster(t_paquete * unPaquete, int socket);
void procesarPaqueteWorker(t_paquete * unPaquete, int socket);
void procesarPaqueteYama(t_paquete * unPaquete, int socket);
void procesarPaquete(t_paquete * unPaquete, int socket);

#endif /* SRC_PROCESAMIENTOPAQUETES_H_ */
