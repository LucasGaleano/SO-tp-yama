/*
 ============================================================================
 Name        : worker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Proceso workers
 ============================================================================
 */

#include "worker.h"

#define PATHCONFIG "../configuraciones/nodo.cfg"
#define TAM_MAX 100
#define TAM_BLOQUE 1048576 //1024 * 1024, un mega
#define PUERTO_REDUCCION_GLOBAL 9999

int main(void)
{

	t_config *conf;

	//LEER ARCHIVO DE CONFIGURACION ---------------------------------------------------------

	conf = config_create(PATHCONFIG);       // path relativo al archivo nodo.cfg
	//int NOMBRE_NODO = config_get_string_value(conf,"NOMBRE_NODO");
	int PUERTO_WORKER = config_get_int_value(conf, "PUERTO_WORKER");
	char* RUTA_DATABIN = config_get_string_value(conf, "RUTA_DATABIN");

	iniciarServer(PUERTO_WORKER, (void *) procesarPaquete);

	config_destroy(conf);
	return EXIT_SUCCESS;
}

void procesarPaquete(t_paquete * unPaquete, int * client_socket)
{
	int pid = 0;
	t_indicacionTransformacion *auxTransf;
	t_indicacionReduccionLocal *auxRedL;
	t_indicacionReduccionGlobal *auxRedG;
	switch (unPaquete->codigoOperacion)
	{
	case HANDSHAKE:
		recibirHandshake(unPaquete, client_socket);
		break;
	case ENVIAR_INDICACION_TRANSFORMACION:
		auxTransf = recibirIndicacionTransformacion(unPaquete);
		pid = fork();
		if (pid == 0)
		{
			transformacion(auxTransf->bloque, auxTransf->rutaArchivoTemporal);
			_exit(EXIT_SUCCESS);
		}
		break;
	case ENVIAR_INDICACION_REDUCCION_LOCAL:
		auxRedL = recibirIndicacionReduccionLocal(unPaquete);
		/*if(pid == 0)
		 {
		 reduccion();
		 _exit(EXIT_SUCCESS);
		 }*/
		//hablar con grupo sobre cambio a estructura
		break;
	case ENVIAR_INDICACION_REDUCCION_GLOBAL:
		auxRedG = recibirIndicacionReduccionGlobal(unPaquete);
		pid = fork();
		if (pid == 0)
		{
			if (auxRedG->encargado)
				iniciarEncargado();
			else
				iniciarEsclavo(auxRedG->puerto, auxRedG->ip, auxRedG->archivoDeReduccionLocal);
			_exit(EXIT_SUCCESS);
		}
		break;
		/*case ENVIAR_INDICACION_ALMACENADO_FINAL:
		 break;*/
	default:
		break;
	}
	destruirPaquete(unPaquete);
}

void recibirHandshake(t_paquete * unPaquete, int * client_socket)
{
	int tipoCliente;
	memcpy(&tipoCliente, unPaquete->buffer->data, sizeof(int));

	switch (tipoCliente)
	{
	case WORKER:
	case MASTER:
		break;
	default:
		*client_socket = -1;
		break;
	}
}

//Funcion de ordenamiento universal para las tres etapas.

char** ordenar(char** palabras, int cant_palabras)
{
	int i, j;
	char canje[TAM_MAX] = { '\0' };
	for (i = 0; i < (cant_palabras - 1); i++)
	{
		for (j = 0; j < (cant_palabras - i - 1); j++)
		{
			if (strcmp(palabras[j], palabras[j + 1]) > 0)
			{
				strcpy(canje, palabras[j]);
				strcpy(palabras[j], palabras[j + 1]);
				strcpy(palabras[j + 1], canje);
			}
		}
	}
	return palabras;
}

void transformacion(unsigned int bloque, char* ruta) {

	//aplicar tranformacion aca

	FILE *a_ordenar = fopen("/home/utnso/Escritorio/Nuevo.txt", "r");
	int j;
	if (a_ordenar == NULL)
		_exit(EXIT_FAILURE);
	FILE *salida = fopen(ruta, "w");
	if (salida == NULL)
	{
		fclose(a_ordenar);
		_exit(EXIT_FAILURE);
	}
	char** palabras = NULL;
	char leido[TAM_MAX] = { '\0' };
	int i = 0;

	while (fgets(leido, TAM_MAX, a_ordenar) != NULL)
	{
		palabras = (char**) realloc(palabras, (i + 1) * sizeof(char*));
		palabras[i] = (char*) calloc(TAM_MAX, sizeof(char));
		strcpy(palabras[i], leido);
		if (palabras[i][strlen(palabras[i]) - 1] != '\n')
			palabras[i][strlen(palabras[i])] = '\n';

		i++;
	}
	palabras = ordenar(palabras, i);
	for (j = 0; j < i; j++)
	{
		fputs(palabras[j], salida);
		free(palabras[j]);
	}
	fclose(a_ordenar);
	fclose(salida);
	free(palabras);
}

inline int sonTodosVerdaderos(int *fdt, int cant)
{
	int i;
	for (i = 0; i < cant; i++)
	{
		if (fdt[i] == 0)
			return 0;
	}
	return 1;
}

FILE* aparear(FILE* archi[], int cant)
{
	char** palabras = NULL;
	int i = 0, j = 0;
	int* fdt = calloc(cant, sizeof(int)); //Para saber si ya llego a EOF. 0 para no, 1 para si
	char leido[TAM_MAX];
	char* ret;
	FILE* devolucion = NULL;
	while (!sonTodosVerdaderos(fdt, cant))
	{
		if (fdt[i % cant] == 1)
		{
			i++;
			continue;
		}
		ret = fgets(leido, TAM_MAX, archi[i % cant]);
		if (ret == NULL)
		{
			fdt[i % cant] = 1;
			i++;
			continue;
		}
		palabras = (char**) realloc(palabras, (j + 1) * sizeof(char*));
		palabras[j] = (char*) calloc(TAM_MAX, sizeof(char));
		strcpy(palabras[j], leido);
		if (palabras[j][strlen(palabras[j]) - 1] != '\n')
			palabras[j][strlen(palabras[j])] = '\n';

		j++;
		palabras = ordenar(palabras, j);
		i++;
	}
	free(fdt);
	devolucion = fopen("/home/utnso/Escritorio/apareado.txt", "w");
	for (i = 0; i < j; i++)
	{
		fputs(palabras[i], devolucion);
		free(palabras[i]);
	}
	free(palabras);
	return devolucion;
}

void reduccion(char* rutas[], int cant)
{
	FILE* loc[cant];
	FILE* apareado = NULL;
	int i;
	for (i = 0; i < cant; i++)
	{
		loc[i] = fopen(rutas[i], "r");
		if (loc[i] == NULL)
			_exit(EXIT_FAILURE);
	}

	apareado = aparear(loc, cant);

	//Por aca va la reduccion

	for (i = 0; i < cant; i++)
	{
		fclose(loc[i]);
		remove(loc[i]);
	}
	fclose(apareado);

}

void recibirDatos(t_paquete * unPaquete)
{
	void* paq = recibirArchivo(unPaquete);
}

void iniciarEncargado()
{
	iniciarServer(PUERTO_REDUCCION_GLOBAL, (void*) recibirDatos);
}

void iniciarEsclavo(char* puerto, char* ip, char* rutaLocal)
{
	int socket = conectarCliente(ip, PUERTO_REDUCCION_GLOBAL, WORKER);
	enviarArchivo(socket, rutaLocal);
}
