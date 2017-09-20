#include "sockets.h"

#define BACKLOG 10		// Cantidad de conexiones maximas

/*------------------------------Clientes------------------------------*/

int conectarCliente(const char * ip, const char * puerto) {
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM; // Indica que usaremos el protocolo TCP

	getaddrinfo(ip, puerto, &hints, &serverInfo); // Carga en serverInfo los datos de la conexion

	//Creo el socket
	int socketfd = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);
	if (connect(socketfd, serverInfo->ai_addr, serverInfo->ai_addrlen)) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(serverInfo);
	return socketfd;
}

/*------------------------------Servidor------------------------------*/

void iniciarServer(const char * puerto) {
	fd_set set_master;
	fd_set set_copia;

	//Creo el socket server
	int server_socket = crearSocketServer(puerto);

	//Borro el conjunto de descriptores de fichero
	FD_ZERO(&set_master);

	//Añado fd al conjunto
	FD_SET(server_socket, &set_master);

	//Setteo el socket mas alto
	int descriptor_mas_alto = server_socket;

	printf("Servidor listo para escuchar conexiones\n");

	while (true) {
		set_copia = set_master;

		if (select(descriptor_mas_alto + 1, &set_copia, NULL, NULL, NULL)
				== -1) {
			perror("select");
			break;
		}

		//Exploro conexiones existentes en busca de datos que leer
		int n_descriptor = 0;

		while (descriptor_mas_alto >= n_descriptor) {

			if (FD_ISSET(n_descriptor, &set_copia)) {

				//Si el descriptor es igual al server_socket quiere decir que un nuevo cliente se quiere conectar
				if (n_descriptor == server_socket) {
					gestionarNuevasConexiones(server_socket, &set_master,
							&descriptor_mas_alto);
				} else {
					//Si el decriptor pertenece a un socket cliente ya aceptado
					gestionarDatosCliente(n_descriptor, &set_master);
				}
			}
			n_descriptor++;
		}
	}

}

int crearSocketServer(const char * puerto) {
	//Creo las estructuras
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // No importa si uso IPv4 o IPv6
	hints.ai_flags = AI_PASSIVE; // Asigna el address del localhost: 127.0.0.1
	hints.ai_socktype = SOCK_STREAM; // Indica que usaremos el protocolo TCP

	getaddrinfo(NULL, puerto, &hints, &serverInfo); // Notar que le pasamos NULL como IP, ya que le indicamos que use localhost en AI_PASSIVE

	//Creo el socket
	int server_socket = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);

	if (server_socket == -1)
		perror("socket");

	//No dejo el puerto ocupado
	int yes = 1;

	int resultado = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes,
			sizeof(yes));

	if (resultado == -1)
		perror("setsockopt");

	//Conecto el puerto con el IP
	if (bind(server_socket, serverInfo->ai_addr, serverInfo->ai_addrlen))
		perror("bind");

	//Libero memoria
	freeaddrinfo(serverInfo); // Ya no lo vamos a necesitar

	//Escuchar conexiones de entrada
	if (listen(server_socket, BACKLOG)) {
		perror("listen");
		exit(1);
	}

	return server_socket;
}

void gestionarNuevasConexiones(int server_socket, fd_set * set_master,
		int * descriptor_mas_alto) {
	//Socket del nuevo cliente
	int client_socket;

	//Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	//Acepto la nueva condicion
	if ((client_socket = accept(server_socket, (struct sockaddr *) &addr,
			&addrlen)) == -1) {
		printf("El socket %d ha producido un error"
				"y ha sido desconectado.\n", client_socket);
		perror("accept");
	} else {
		printf("El socket %d se ha conectado al servidor.\n", client_socket);

		//Añado al conjunto maestro
		FD_SET(client_socket, set_master);

		//Actualizo el máximo
		if (client_socket > *descriptor_mas_alto)
			*descriptor_mas_alto = client_socket;
	}
}

void gestionarDatosCliente(int client_socket, fd_set * set_master) {

	int tamPaquete = recibirTamPaquete(client_socket, set_master);

	if (tamPaquete > 0) {
		t_paquete * unPaquete = recibirPaquete(client_socket, set_master,
				tamPaquete);

		mostrarPaquete(unPaquete);
	}
}

/*------------------------------Paquetes------------------------------*/

void enviarPaquetes(int socketfd, t_paquete * unPaquete) {

	int desplazamiento = 0;

	int tamPaquete = sizeof(int);
	int tamEmisor = sizeof(int);
	int tamCodOp = sizeof(int);
	int tamSize = sizeof(int);
	int tamData = unPaquete->buffer->size;

	int tamTotal = tamEmisor + tamCodOp + tamSize + tamData;

	void * buffer = malloc(tamPaquete + tamTotal);

	//Tamaño del paquete
	memcpy(buffer + desplazamiento, &tamTotal, tamPaquete);
	desplazamiento += tamPaquete;

	//Emisor
	memcpy(buffer + desplazamiento, &unPaquete->emisor, tamEmisor);
	desplazamiento += tamEmisor;

	//Codigo de operacion
	memcpy(buffer + desplazamiento, &unPaquete->codigoOperacion, tamCodOp);
	desplazamiento += tamCodOp;

	//Buffer -- size
	memcpy(buffer + desplazamiento, &unPaquete->buffer->size, tamSize);
	desplazamiento += tamSize;

	//Buffer -- data
	memcpy(buffer + desplazamiento, unPaquete->buffer->data, tamData);

	//Envio el paquete y compruebo errores
	int resultado = send(socketfd, buffer, tamPaquete + tamTotal, MSG_NOSIGNAL);

	if (resultado == -1)
		perror("send");

	//Libero memoria
	destruirPaquete(unPaquete);
}

int recibirTamPaquete(int client_socket, fd_set * set_master) {
	int tamBuffer;

	//Creo el buffer
	void * buffer = malloc(sizeof(int));

	//Recibo el buffer
	int recvd = recv(client_socket, buffer, sizeof(int), MSG_WAITALL);

	//Verifico error o conexión cerrada por el cliente
	if (recvd <= 0) {
		tamBuffer = recvd;
		if (recvd == -1) {
			perror("recv");
		}
		printf("El socket %d ha producido un error "
				"y ha sido desconectado.\n", client_socket);

		//Cierro el socket
		close(client_socket);

		//Elimino el socket del conjunto maestro
		FD_CLR(client_socket, set_master);

	} else {

		//Copio el buffer en una variable asi despues lo libero
		memcpy(&tamBuffer, buffer, sizeof(int));

		//Libero memoria
		free(buffer);
	}

	return tamBuffer;
}

t_paquete * recibirPaquete(int client_socket, fd_set * set_master,
		int tamPaquete) {
	//Reservo memoria para el buffer
	void * buffer = malloc(tamPaquete);
	//memset(buffer, '\0', tamPaquete); // Lo limpio al buffer para que no tenga basura

	//Recibo el buffer
	int recvd = recv(client_socket, buffer, tamPaquete, MSG_WAITALL);

	//Verifico error o conexión cerrada por el cliente
	if (recvd <= 0) {
		if (recvd == -1) {
			perror("recv");
		}
		printf("El socket %d ha producido un error "
				"y ha sido desconectado.\n", client_socket);

		//Cierro el socket
		close(client_socket);

		//Elimino el socket del conjunto maestro
		FD_CLR(client_socket, set_master);
	}

	//Armo el paquete a partir del buffer
	t_paquete * unPaquete = crearPaquete(buffer);

	return unPaquete;
}

t_paquete * crearPaquete(void * buffer) {
	t_paquete * unPaquete = malloc(sizeof(t_paquete));

	int desplazamiento = 0;

	int tamEmisor = sizeof(int);
	int tamCodOp = sizeof(int);
	int tamSize = sizeof(int);

	//Emisor
	memcpy(&unPaquete->emisor, buffer + desplazamiento, tamEmisor);
	desplazamiento += tamEmisor;

	//Codigo de operacion
	memcpy(&unPaquete->codigoOperacion, buffer + desplazamiento, tamCodOp);
	desplazamiento += tamCodOp;

	//Buffer -- size
	unPaquete->buffer = malloc(sizeof(t_stream));
	memcpy(&unPaquete->buffer->size, buffer + desplazamiento, tamSize);
	desplazamiento += tamSize;

	//Buffer -- data
	int tamData = unPaquete->buffer->size;
	unPaquete->buffer->data = malloc(tamData);
	memcpy(unPaquete->buffer->data, buffer + desplazamiento, tamData);

	return unPaquete;
}

void destruirPaquete(t_paquete * unPaquete) {
	free(unPaquete->buffer->data);
	free(unPaquete->buffer);
	free(unPaquete);
}

void mostrarPaquete(t_paquete * unPaquete) {
	printf("Muestro el paquete: \n");
	printf("Emisor: %d \n", unPaquete->emisor);
	printf("Codigo de operacion: %d \n", unPaquete->codigoOperacion);
	printf("Tamanio del buffer: %d \n", unPaquete->buffer->size);
	printf("Buffer: %s \n", (char*) unPaquete->buffer->data);
}
