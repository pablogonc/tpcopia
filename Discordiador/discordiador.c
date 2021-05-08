/*
 * discordiador.c
 *
 *  Created on: 8 may. 2021
 *      Author: utnso
 */

#include<commons/config.h>
#include <commons/log.h>
#include<readline/readline.h>
#include <cliente.h>
#include <utilidades.h>

void terminar_programa(int ,int ,t_log* , t_config* );

int main(void){
	printf("Inciando modulo discordiador \n");

	readline("Presione enter para conectar >");



	t_log* logger;
	t_config* config;

	logger = log_create("Discordiador.log", "Discordiador", 1, LOG_LEVEL_DEBUG);

	int conexionRam;
	char* ipRam;
	char* puertoRam;

	config = config_create("ram.config");

	ipRam = config_get_string_value(config,"IP");
	puertoRam = config_get_string_value(config,"PUERTO");



	conexionRam = crear_conexion(ipRam, puertoRam);


	config = config_create("store.config");

	char * ipStore = config_get_string_value(config,"IP");
	char * puertoStore = config_get_string_value(config,"PUERTO");

	int conexionStore = crear_conexion(ipStore, puertoStore);

	char* leido;
	leido = readline("enviar al servidor >");
	int destinatario;
	while(strcmp(leido,"exit") !=0){
		destinatario= analizar(leido,conexionRam,conexionStore);
		if (destinatario==-1){
			log_warning(logger,"Destinatario invalido");
		}else{
			enviar_mensaje(leido,destinatario);
		}
		free(leido);
		leido = readline("enviar al servidor >");
	}


	terminar_programa(conexionRam,conexionStore, logger, config);

}

void terminar_programa(int conexionRam,int conexionStore,t_log* logger, t_config* config){
	liberar_conexion(conexionRam);
	liberar_conexion(conexionStore);
	log_destroy(logger);
	config_destroy(config);
}

int analizar(char * leido,int ram,int store){

	char palabra[strlen(leido)+1];

	strcpy(palabra,leido);

	strtok(palabra," ");

	if (strcmp(palabra,"ram")==0)
		return ram;
	if (strcmp(palabra,"store")==0)
		return store;
	return -1;
}
