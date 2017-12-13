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

		return list_find(tabla_de_estados, (void*)esRegistroBuscado);
}

t_elemento_tabla_estado* buscarRegistroPorNodoYBloque(int master, char* nombreNodo, int bloque, int etapa){ //lo mismo sin el job
	bool esRegistroBuscado(t_elemento_tabla_estado* elemento){
			bool esMasterIgual = elemento->master == master;
			bool esBloqueIgual = elemento->bloque == bloque;
			bool esEtapaIgual = elemento->etapa == etapa;
			bool esNombreNodoIgual = string_equals_ignore_case(elemento->nodo, nombreNodo);

			return esMasterIgual && esBloqueIgual && esNombreNodoIgual && esEtapaIgual;
		}

		return list_find(tabla_de_estados, (void*)esRegistroBuscado);
}

void modificarEstadoDeRegistroPorNodoYBloque(int master, char* nombreNodo, int bloque, int etapa, int nuevoEstado){
	t_elemento_tabla_estado* elemento = buscarRegistroPorNodoYBloque(master, nombreNodo, bloque, etapa);
	elemento->estado = nuevoEstado;
}

t_elemento_tabla_estado* TerminoElNodo(char* nombreNodo,int etapa,int estado){

	bool esRegistroBuscado(t_elemento_tabla_estado* elemento){
				bool esEtapaIgual = elemento->etapa == etapa;
				bool esNombreNodoIgual = string_equals_ignore_case(elemento->nodo, nombreNodo);
				bool esEstadoIgual = elemento->estado == estado;

				return  esNombreNodoIgual && esEtapaIgual && esEstadoIgual;
			}

	return list_find(tabla_de_estados, (void*)esRegistroBuscado);

}

bool terminoUnNodoLaTransformacion(char* nomNodo,int etapa, int estado){

	bool esRegistroBuscado(t_elemento_tabla_estado* elemento) {
		bool esEtapaIgual = elemento->etapa == etapa;
		bool esNombreNodoIgual = string_equals_ignore_case(elemento->nodo, nomNodo);
		bool esEstadoIgual = elemento->estado == estado;

		return esNombreNodoIgual && esEtapaIgual && esEstadoIgual;
	}

	int cantElementos = list_count_satisfying(tabla_de_estados, (void*)esRegistroBuscado);

	if(cantElementos == 0){
		return true;
	}
	return false;

}

char* nombreArchivoTemp(char* prefijo){
	char* nombre = string_new();
	string_append(&nombre, prefijo);
	char* hora = temporal_get_string_time();
	string_append(&nombre, hora);
	free(hora);
	return nombre;
}


