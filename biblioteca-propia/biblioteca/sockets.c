#include "sockets.h"

#define MAX_BUFFER_SIZE  200
#define MYPORT 3200		// Puerto al que conectarán los usuarios
#define MAXDATASIZE 100 // máximo número de bytes que se pueden leer de una vez


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

void enviarMensaje(int socket, char * buffer) {
	fgets(buffer, MAX_BUFFER_SIZE, stdin);

	int resultado = send(socket, buffer, strlen(buffer) + 1, 0);

	if (resultado == -1)
		perror("send");
}

void enviarPaquetes(int sockfd) {
	char * buffer = malloc(MAX_BUFFER_SIZE);

	bool enviar = true;

	while (enviar) {
		enviarMensaje(sockfd, buffer);
		if (!strcmp(buffer, "exit\n"))
			enviar = false;
	}
}

int main(void) {

	int sockfd = conectarCliente("127.0.0.1", "3200");

	enviarPaquetes(sockfd);

	//Cerramos los sockets abiertos
	close(sockfd);

	return EXIT_SUCCESS;
}

/*------------------------------Servidor------------------------------*/

#define BACKLOG 10		// Cantidad de conexiones maximas

int crearSocketDeEscucha(const char * puerto) {
	//Creo las estructuras
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // No importa si uso IPv4 o IPv6
	hints.ai_flags = AI_PASSIVE; // Asigna el address del localhost: 127.0.0.1
	hints.ai_socktype = SOCK_STREAM; // Indica que usaremos el protocolo TCP

	getaddrinfo(NULL, puerto, &hints, &serverInfo); // Notar que le pasamos NULL como IP, ya que le indicamos que use localhost en AI_PASSIVE

	//Creo el socket
	int listeningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);

	if (listeningSocket == -1)
		perror("socket");

	//No dejo el puerto ocupado
	int yes = 1;

	int resultado = setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &yes,
			sizeof(yes));

	if (resultado == -1)
		perror("setsockopt");

	//Conecto el puerto con el IP
	if (bind(listeningSocket, serverInfo->ai_addr, serverInfo->ai_addrlen))
		perror("bind");

	//Libero memoria
	freeaddrinfo(serverInfo); // Ya no lo vamos a necesitar

	//Escuchar conexiones de entrada
	if (listen(listeningSocket, BACKLOG)) {
		perror("listen");
		exit(1);
	}

	return listeningSocket;
}

int aceptarConexionDeEntrada(int sockfd, struct sockaddr_in * serverAddress) {
	int newSockfd;
	int sin_size;

	sin_size = sizeof(struct sockaddr_in);

	newSockfd = accept(sockfd, (struct sockaddr *) serverAddress,
			(socklen_t *) &sin_size);

	if (newSockfd == -1) {
		perror("accept");
	}

	return newSockfd;
}

void recibirMensaje(int socket, char * buffer, bool * recibir) {
	int resultado = recv(socket, (void*) buffer, MAX_BUFFER_SIZE, 0);

	if (resultado == -1)
		perror("recv");

	if (resultado == 0 || !strcmp(buffer, "exit\n")) {
		perror("cerro conexión");
		*recibir = false;
	}
}

/*
int main() {
	char * buffer = malloc(MAX_BUFFER_SIZE);

	int listenningSocket = crearSocketDeEscucha("3200");

	struct sockaddr_in addr; // Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t addrlen = sizeof(addr);

	int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr,
			&addrlen);

	printf("Cliente conectado. Esperando mensajes:\n");

	bool recibir = true;

	while (recibir) {
		recibirMensaje(socketCliente, buffer, &recibir);
		if (strcmp(buffer, "exit\n"))
			printf("%s", buffer);
	}

	close(listenningSocket);
	close(socketCliente);

	return 0;
}
*/
