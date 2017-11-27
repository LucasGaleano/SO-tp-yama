#include "tablas.h"


t_elemento_tabla_estado * crearRegistro(int job, int master, char * nombreNodo, int bloque, int etapa, char * nombreArchivoTemporal, int estado) {
	int tamanioNombreNodo = string_length(nombreNodo) + 1;
	int tamanioNombreArchivoTemporal = string_length(nombreArchivoTemporal) + 1;

	t_elemento_tabla_estado * registro = malloc(sizeof(t_elemento_tabla_estado));

	registro->job = job;
	registro->master = master;
	registro->bloque = bloque;
	registro->etapa = etapa;
	registro->estado = estado;

	registro->nodo = malloc(tamanioNombreNodo);
	registro->nombreArchivoTemporal = malloc(tamanioNombreArchivoTemporal);

	memcpy(registro->nodo, nombreNodo, tamanioNombreNodo);
	memcpy(registro->nombreArchivoTemporal, nombreArchivoTemporal, tamanioNombreArchivoTemporal);

	return registro;
}

void agregarRegistro(int job, int master, char * nombreNodo, int bloque, int etapa, char * nombreArchivoTemporal, int estado){
	t_elemento_tabla_estado * elemento = crearRegistro(job, master, nombreNodo, bloque, etapa, nombreArchivoTemporal, estado);
	list_add(tabla_de_estados, elemento);
}

void modificarEstadoDeRegistro(int job, int master, char* nombreNodo, int bloque, int etapa, int nuevoEstado){
	t_elemento_tabla_estado* elemento = buscarRegistro(job, master, nombreNodo, bloque, etapa);
	elemento->estado = nuevoEstado;
}

void eliminarRegistro(int job, int master, char* nombreNodo, int bloque, int etapa){

	bool esRegistroBuscado(t_elemento_tabla_estado* elemento){
		bool esJobIgual = elemento->job == job;
		bool esMasterIgual = elemento->master == master;
		bool esBloqueIgual = elemento->bloque == bloque;
		bool esEtapaIgual = elemento->etapa == etapa;
		bool esNombreNodoIgual = string_equals_ignore_case(elemento->nodo, nombreNodo);

		return esJobIgual && esMasterIgual && esBloqueIgual && esNombreNodoIgual && esEtapaIgual;
	}
	void liberarRegistro(t_elemento_tabla_estado* elemento){
		free(elemento->nodo);
		free(elemento->nombreArchivoTemporal);
		free(elemento);
	}

	list_remove_and_destroy_by_condition(tabla_de_estados, (void*) esRegistroBuscado, (void*) liberarRegistro);
}

t_elemento_tabla_estado* buscarRegistro(int job, int master, char* nombreNodo, int bloque, int etapa){
	bool esRegistroBuscado(t_elemento_tabla_estado* elemento){
			bool esJobIgual = elemento->job == job;
			bool esMasterIgual = elemento->master == master;
			bool esBloqueIgual = elemento->bloque == bloque;
			bool esEtapaIgual = elemento->etapa == etapa;
			bool esNombreNodoIgual = string_equals_ignore_case(elemento->nodo, nombreNodo);

			return esJobIgual && esMasterIgual && esBloqueIgual && esNombreNodoIgual && esEtapaIgual;
		}

		return list_find(tabla_de_estados, esRegistroBuscado);
}

int numeroRandom() {
	unsigned short lfsr = 0xACE1u;
	unsigned bit;

	bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
	return lfsr = (lfsr >> 1) | (bit << 15);
}
