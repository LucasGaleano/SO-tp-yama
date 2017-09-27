#include "fileSystem.h"

int main() {

	//iniciarServer("3200", (void *) procesarPaquete);

	iniciarConsola();

	return EXIT_SUCCESS;
}

/*-------------------------Procesamiento paquetes-------------------------*/
void recibirHandshake(t_paquete * unPaquete, int * client_socket){
	int tipoCliente;
	memcpy(&tipoCliente, unPaquete->buffer->data, sizeof(int));

	switch (tipoCliente) {
		case DATANODE:
			break;
		case YAMA:
			break;
		default:
			*client_socket = -1;
			break;
	}
}

void procesarPaquete(t_paquete * unPaquete, int * client_socket){
	switch (unPaquete->codigoOperacion) {
		case HANDSHAKE:
			recibirHandshake(unPaquete, client_socket);
			break;
		case ENVIAR_MENSAJE:
			recibirMensaje(unPaquete);
			break;
		case ENVIAR_ARCHIVO:
			recibirArchivo(unPaquete);
			break;
		default:
			break;
	}
	destruirPaquete(unPaquete);
}
