#ifndef INTERFAZ_H_
#define INTERFAZ_H_

#include "tablas.h"

t_list * listaTemporal;

/*-------------------------Almacenar archivo-------------------------*/
void				almacenarArchivo				(char *, char *, char *, int);
void * 				dividirBloqueArchivoBinario		(void *, size_t, int *, int *);
void * 				dividirBloqueArchivoTexto		(void *, int *);
char * 				buscarNodoMenosCargado			(void);
int 				buscarBloqueAEscribir			(char *);
char * 				generarBloque					(void *, int);

/*-------------------------Leer archivo-------------------------*/
char* 				leerArchivo						(char *);
t_nodoBloque *		nodoMenosSaturado				(t_list *);
int 				cantidadTareas					(char *);

#endif /* INTERFAZ_H_ */
