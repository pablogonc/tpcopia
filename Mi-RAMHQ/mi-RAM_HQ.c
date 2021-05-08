/*
 * mi-RAM_HQ.c
 *
 *  Created on: 8 may. 2021
 *      Author: utnso
 */

#include <commons/log.h>
#include <servidor.h>
#include <utilidades.h>
#include<readline/readline.h>
#include<stdlib.h>

//----
t_log* logger;
int escuchar(int);

int main(){

	logger = log_create("RAM.log", "RAM", 1, LOG_LEVEL_DEBUG);

	log_info(logger, "Iniciando servidor RAM");

	int server= iniciar_servidor("127.0.0.1","4445");

	log_info(logger, "RAM : - listo para recibir al cliente");

	int cliente = esperar_cliente(server);

	log_info(logger, "RAM : - Cliente Conectado");

	escuchar(cliente);
}


int escuchar(int cliente){

	char *msg;
	while(1){
		int cod_op = recibir_operacion(cliente);

		switch(cod_op)
		{
		case MENSAJE:
			msg= recibir_mensaje(cliente);
			log_info(logger, ":- Recibido - '%s'", msg);
			//
			//codigo en que se use el contenido del msg
			//
			free(msg);
			break;
		case -1:
			log_warning(logger,"el cliente se desconecto. Terminando servidor");
			readline("Presione enter para salir>");
			return EXIT_FAILURE;
		default:
			printf("Operacion desconocida.");
			break;
		}
	}
	return EXIT_SUCCESS;
}

