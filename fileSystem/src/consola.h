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
#include "interfaz.h"

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
void 				modificar								(char *);
void 				mostrarContenidoArchivo					(char *);
void 				crearDirectorio							(char *);
void 				copiarArchivoLocalAlYamafsInterfaz		(char *);
void 				copiarArchivoYamafsALocal				(char *);
void 				crearCopiaBloqueEnNodo					(char *);
void				solicitarHash							(char *);
void 				listarArchivos							(char *);
void 				mostrarInfo								(char *);

/*------------------------------Auxiliares------------------------------*/
char * 				obtenerParametro				(char *, int);
void 				destruirSubstring				(char **);
int 				obtenerIndex					(char *);
bool 				verificarDuplicados				(t_directory *);
char ** 			buscarBloqueABorar				(int, int *, int *, t_config *);
int 				cantArchivosEnDirectorio		(char *);
void 				listarArchivosDirectorios		(char *);
void				modificarArchivo				(char **, char **, int, int, int, char *);
void 				imprimirBloque					(t_config *, int, int);
#endif /* CONSOLA_H_ */
