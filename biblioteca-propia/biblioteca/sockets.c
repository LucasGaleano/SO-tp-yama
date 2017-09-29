#include "sockets.h"

/*------------------------------Clientes------------------------------*/

int conectarCliente(const char * ip, const char * puerto, int cliente) {
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
	}

	freeaddrinfo(serverInfo);

	enviarHandshake(socketfd, cliente);

	return socketfd;
}

/*------------------------------Servidor------------------------------*/

void iniciarServer(const char * puerto, void(*procesarPaquete)(void*, int*)) {
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
					gestionarDatosCliente(n_descriptor, &set_master, (void *) procesarPaquete);
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
	if (listen(server_socket, SOMAXCONN)) {
		perror("listen");
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
		return;
	}

	printf("El socket %d se ha conectado al servidor.\n", client_socket);

	//Añado al conjunto maestro
	FD_SET(client_socket, set_master);

	//Actualizo el máximo
	if (client_socket > *descriptor_mas_alto)
		*descriptor_mas_alto = client_socket;
}

void gestionarDatosCliente(int client_socket, fd_set * set_master, void(*procesarPaquete)(void*, int*)) {

	int tamPaquete = recibirTamPaquete(client_socket, set_master);

	if (tamPaquete > 0) {
		t_paquete * unPaquete = recibirPaquete(client_socket, set_master,
				tamPaquete);

		int socketAux = client_socket;

		procesarPaquete(unPaquete, &client_socket);

		if(client_socket == -1){
			printf("El socket %d no a pasado el handshake "
					"y ha sido desconectado.\n", socketAux);

			//Cierro el socket
			close(socketAux);

			//Elimino el socket del conjunto maestro
			FD_CLR(socketAux, set_master);
		}
	}
}
