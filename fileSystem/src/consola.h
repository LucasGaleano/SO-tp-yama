#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <commons/string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <dirent.h>

#include "tablas.h"

/*------------------------------Consola------------------------------*/
void 				iniciarConsola							(void);
void 				ejecutarComando							(char *, bool *);

/*------------------------------Comandos------------------------------*/
void 				ejecutarMan								(void);
void 				ejecutarExit							(bool *);
void 				formatearFilesystem						(void);
void 				eliminarArchivo							(char *);
void 				eliminarDirectorio						(char *);
void 				eliminarBloque							(char *);
void 				modificarArchivo						(char *);
void 				mostrarContenidoArchivo					(char *);
void 				crearDirectorio							(char *);
void 				copiarArchivoLocalAlYamafsInterfaz		(char *);
void 				copiarArchivoLocalAlYamafs				(char *);
void 				crearCopiaBloqueEnNodo					(char *);
void				solicitarHash							(char *);
void 				listarArchivos							(char *);
void 				mostrarInfo								(char *);

/*------------------------------Auxiliares------------------------------*/
char * 				obtenerParametro				(char *, int);
void 				destruirSubstring				(char **);
int 				obtenerIndexPadre				(char *);
bool 				verificarDuplicados				(t_directory *);
char ** 			buscarBloqueABorar				(int, int *, int *, t_config *);
int 				cantArchivosEnDirectorio		(char *);

#endif /* CONSOLA_H_ */
