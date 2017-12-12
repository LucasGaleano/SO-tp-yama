#ifndef MAPAS_H_
#define MAPAS_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <string.h>
#include <biblioteca/estructuras.h>

t_puerto_ip* buscarIpYPuertoConNombreNodo		(char* nombreNodo, t_list* listaDireccionesNodos);
int 		 buscarTamanioArchivoConNombreNodo	(char* nombreNodo, t_list* listaBloquesNodos);
int mapearBloqueArchivoABloqueNodo(t_list* listaBloquesNodos, char* nombreNodo, int bloqueArchivo);

#endif /* MAPAS_H_ */
