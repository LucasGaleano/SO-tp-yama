#include "mapas.h"

t_puerto_ip* buscarIpYPuertoConNombreNodo(char* nombreNodo,
		t_list* listaDireccionesNodos) {

	bool esRegistroBuscado(t_puerto_ip* direccionNodo) {
		if (string_equals_ignore_case(direccionNodo->nomNodo, nombreNodo)) {
			return true;
		}
		return false;
	}

	return list_find(listaDireccionesNodos, (void*) esRegistroBuscado);

}

int buscarTamanioArchivoConNombreNodo(char* nombreNodo,t_list* listaBloquesNodos) {

	int i = 0;
	for (; i < listaBloquesNodos->elements_count; i++) {
		t_nodo_bloque* nodoBloque = list_get(listaBloquesNodos, i);
		if (string_equals_ignore_case(nodoBloque->nomNodo, nombreNodo)) {

			return nodoBloque->bloqueNodo;
			break;
		}
	}
}

int mapearBloqueArchivoABloqueNodo(t_list* listaBloquesNodos, char* nombreNodo, int bloqueArchivo) {
	int i = 0;
	for (; i < listaBloquesNodos->elements_count; i++) {
		t_nodo_bloque* nodoBloque = list_get(listaBloquesNodos, i);
		if (string_equals_ignore_case(nodoBloque->nomNodo, nombreNodo)
				&& nodoBloque->bloqueArchivo == bloqueArchivo)
			return nodoBloque->bloqueNodo;
	}
}
