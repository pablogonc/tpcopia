/*
 * mi-RAM_HQ.c
 *
 *  Created on: 8 may. 2021
 *      Author: utnso
 */
#include "mi-RAM_HQ.h"
#include "tcp_server.h"
#include <commons/log.h>
#include <commons/config.h>
#include "../utilidades/utilidades.h"
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
//----

int main(){
	//Debug mode = 1 para printear cosas por pantalla
	//Debug_mode = 0 para mostrar el mapa por pantalla (no printear nada)
	DEBUG_MODE=1;

	//Para abrir el archivo de configuracion con la biblioteca de commons/config.h
	t_config* config;
	//Elementos del mi-RAM_HQ.config
	long tamanioMemoria;
	string esquemaMemoria;
	long tamanioPagina;
	long tamanioSwap;
	string pathSwap;
	string algoritmoReemplazo;
	string criterioSeleccion;
	uint32_t puertoServidor;
	char* IP;

	logger = log_create("../RAM.log", "RAM", DEBUG_MODE, LOG_LEVEL_DEBUG);

	//Obtenemos los valores del config
	config = config_create("mi-RAM_HQ.config");
	tamanioMemoria = config_get_long_value(config, "TAMANIO_MEMORIA");
	esquemaMemoria = config_get_string_value(config, "ESQUEMA_MEMORIA");
	tamanioPagina = config_get_long_value(config, "TAMANIO_PAGINA");
	tamanioSwap = config_get_long_value(config, "TAMANIO_SWAP");
	pathSwap = config_get_string_value(config, "PATH_SWAP");
	algoritmoReemplazo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	criterioSeleccion = config_get_string_value (config, "CRITERIO_SELECCION");
	puertoServidor = config_get_long_value(config, "PUERTO");
	IP = config_get_string_value(config, "IP");
	
	//Reserva la memoria
	reservarMemoria (tamanioMemoria);

	if(strncmp(esquemaMemoria,"PAGINACION",sizeof(char)*10) == 0){
		configurarMemoriaPaginacion(tamanioPagina, tamanioSwap, pathSwap, algoritmoReemplazo);

	}else{
		if(strncmp(esquemaMemoria,"SEGMENTACION",sizeof(char)*12) == 0){
					configurarMemoriaSegmentacion(criterioSeleccion);
				}else{
					log_info(logger, "Esquema de memoria invalido");
				}
	}

	
	// Dibuja el mapa
	if(!DEBUG_MODE){
		dibujarMapa();
	}


	//Inicia las variables globales
	initGlobales();

	// Inicia los semaforos
	initSemaphores();

	//Inicia el servidor
	log_info(logger, "Iniciando servidor RAM");
	char* puertoString = (char*)calloc(11, sizeof(char));
	sprintf(puertoString, "%lu", (long unsigned int) puertoServidor);

	pthread_create(&hiloSignals,NULL,sigHandlerMain,NULL);

	iniciar_servidor(IP, puertoString);

	sem_post(signalThread);

	free(puertoString);

	finalizarMapa();

	config_destroy(config);

	return 0;
}
