#ifndef SRC_PROCESAMIENTOPAQUETES_H_
#define SRC_PROCESAMIENTOPAQUETES_H_

#include "estructuras.h"
#include "serializacion.h"

/*-----------------------------------Paquetes-----------------------------------*/
void	 						enviarPaquetes						(int, t_paquete *);
int 							recibirTamPaquete					(int);
t_paquete * 					recibirPaquete						(int, int);
t_paquete * 					crearPaquete						(void *);
t_paquete *						crearPaqueteError					(int client_socket);
void 							destruirPaquete						(t_paquete *);
void 							mostrarPaquete						(t_paquete *);

/*-----------------------------------Enviar paquetes-----------------------------------*/
void 							enviarHandshake								(int, int);

void 							enviarSolicitudNombre						(int);
void 							enviarNombre								(int, char *, char *, char *);

void 							enviarMensaje								(int, char *);

void 							enviarArchivo								(int, char *);

void 							enviarSolicitudInfoDataNode					(int);
void 							enviarInfoDataNode							(int, char *, int, int);

void 							enviarBloque								(int, char*);
void 							enviarBloqueGenerarCopia					(int, void *, char *, int, char *);
void 							enviarBloqueArchTemp						(int, char*, int);

void 							enviarSolicitudLecturaBloque				(int, int);
void 							enviarSolicitudLecturaBloqueGenerarCopia	(int,int, char *, int, char *);
void 							enviarSolicitudLecturaArchTemp				(int, int, int);

void 							enviarSolicitudEscrituraBloque				(int, int, int, void*);
void 							enviarRespuestaEscrituraBloque				(int, bool, int);

void 							enviarSolicitudTransformacion				(int, t_pedidoTransformacion *);
void 							enviarSolicitudReduccionLocal				(int, t_pedidoReduccionLocal *);
void 							enviarSolicitudReduccionGlobal				(int, t_pedidoReduccionGlobal *);
void 							enviarSolicitudAlmacenadoFinal				(int, t_pedidoAlmacenadoFinal *);

void 							enviarIndicacionTransformacion				(int, t_indicacionTransformacion *);
void 							enviarIndicacionReduccionLocal				(int, t_indicacionReduccionLocal *);
void 							enviarIndicacionReduccionGlobal				(int, t_indicacionReduccionGlobal *);
void 							enviarIndicacionAlmacenadoFinal				(int, t_indicacionAlmacenadoFinal *);

void							enviarError									(int, int);
void							enviarTareaCompletada						(int, int);

void							enviarListaNodoBloques						(int, t_nodos_bloques *);

void 							enviarRutaParaArrancarTransformacion		(int server_socket, char * ruta);

char * 							recibirRutaParaArrancarTransformacion		(t_paquete * unPaquete);

/*-----------------------------------Recibir paquetes-----------------------------------*/
int 							recibirHandshake							(t_paquete *);

t_nodo_nombre *					recibirNombre								(t_paquete *);

char * 							recibirMensaje								(t_paquete *);

char * 							recibirRutaArchivo							(t_paquete *);

void * 							recibirArchivo								(t_paquete *);

t_nodo_info *					recibirInfoDataNode							(t_paquete *);

void * 							recibirBloque								(t_paquete *);
t_respuestaLecturaGenerarCopia* recibirBloqueGenerarCopia					(t_paquete *);
t_respuestaLecturaArchTemp * 	recibirBloqueArchTemp						(t_paquete *);

int 							recibirSolicitudLecturaBloque				(t_paquete *);
t_lecturaGenerarCopia *			recibirSolicitudLecturaBloqueGenerarCopia	(t_paquete *);
t_lecturaArchTemp * 			recibirSolicitudLecturaBloqueArchTemp		(t_paquete *);

t_pedidoEscritura* 				recibirSolicitudEscrituraBloque				(t_paquete *);
t_respuestaEscritura*			recibirRespuestaEscrituraBloque				(t_paquete *);

t_pedidoTransformacion* 		recibirSolicitudTransformacion				(t_paquete *);
t_pedidoReduccionLocal* 		recibirSolicitudReduccionLocal				(t_paquete *);
t_pedidoReduccionGlobal*		recibirSolicitudReduccionGlobal				(t_paquete *);
t_pedidoAlmacenadoFinal* 		recibirSolicitudAlmacenadoFinal				(t_paquete *);

t_indicacionTransformacion* 	recibirIndicacionTransformacion				(t_paquete *);
t_indicacionReduccionLocal*	 	recibirIndicacionReduccionLocal				(t_paquete *);
t_indicacionReduccionGlobal*	recibirIndicacionReduccionGlobal			(t_paquete *);
t_indicacionAlmacenadoFinal* 	recibirIndicacionAlmacenadoFinal			(t_paquete *);

int								recibirError								(t_paquete *);
int								recibirTareaCompletada						(t_paquete *);

t_nodos_bloques *				recibirListaNodoBloques						(t_paquete *);

#endif /* SRC_PROCESAMIENTOPAQUETES_H_ */
