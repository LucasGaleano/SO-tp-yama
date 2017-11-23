#ifndef INTERFAZ_H_
#define INTERFAZ_H_

#include <stdlib.h>
#include <stdio.h>

#include "tablas.h"

/*-------------------------Almacenar archivo-------------------------*/
void				almacenarArchivo				(char *, char *, char *, int);
void * 				dividirBloqueArchivoBinario		(void *, int *);
void * 				dividirBloqueArchivoTexto		(void *, int *);
char * 				buscarNodoMenosCargado			(void);
int 				buscarBloqueAEscribir			(char *);

#endif /* INTERFAZ_H_ */
