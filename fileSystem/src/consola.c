#include "consola.h"

void iniciarConsola() {
	char * linea;

	bool ejecutar = true;

	while (ejecutar) {
		linea = readline(">");

		if (linea) {
			add_history(linea);
		} else {
			free(linea);
			break;
		}

		if (string_equals_ignore_case(linea, "exit")) {
			printf("Se cerro la consola \n");
			ejecutar = false;
			free(linea);
			break;
		}

		printf("%s\n", linea);

		free(linea);
	}

	clear_history();
}
