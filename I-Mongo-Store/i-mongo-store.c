/*
 * i-mongo-store.c
 *
 *  Created on: 8 may. 2021
 *      Author: utnso
 */

#include "i-mongo-store.h"
#include "utilidades_store.h"
#include <signal.h>
#include <unistd.h>

int escuchar(int);
int salir=0;
int socket_disc;
char*IP;

sem_t* semSignal;



void sabotaje(){
	sem_wait(ocupado);


	log_warning(logger,"mando aviso SABOTAJE");
	t_paquete* p = crear_paquete(SABOTAJE);
	int * coords = posicionSabotaje();
	int x =coords[0];
	int y =coords[1];
	printf("%d , %d",x,y);
	agregar_a_paquete(p,&x,sizeof(int));
	agregar_a_paquete(p,&y,sizeof(int));
	enviar_paquete(p,socket_disc);
	eliminar_paquete(p);
	free(coords);
}

void signals(){
	printf("\nPID: %lu\n",getpid());
	signal(SIGUSR1,&sabotaje);
	sem_wait(semSignal);
}

int main(void){

	semSignal = (sem_t*)malloc(sizeof(sem_t));
	sem_init(semSignal,1,0);
	pthread_t hilosignal;
	pthread_create(&hilosignal,NULL,&signals,NULL);
	//
	printf("Iniciando I Mongo Store \n");

	logger = log_create("MongoStore.log", "MongoStore", 1, LOG_LEVEL_DEBUG);

	ocupado = (sem_t*)malloc(sizeof(sem_t));
	sem_init(ocupado,1,1);
//-------------------------------- archivo de config store --------------------------//

	t_config * config = config_create("store.config");

	puntoDeMontaje = config_get_string_value(config,"PUNTO_MONTAJE");
	puerto = config_get_string_value(config,"PUERTO");
	tiempoSincronizacion = config_get_int_value(config,"TIEMPO_SINCRONIZACION");

	posicionesSabotaje =  config_get_array_value(config, "POSICIONES_SABOTAJE");
	IP = config_get_string_value(config, "IP");
	cantidadPosicionesSabotaje = tamanioPosicionesSabotaje();
	acumuladorSabotajes = 0;

	posicionSabotaje();

//--------------------------------- Estructuras ------------------------------------//

	if(access(string_from_format("%s/SuperBloque.bin", puntoDeMontaje),F_OK)==0){

		printf("el archivo existe, recuperando filesystem... \n");

		superBloque = leerSuperBloque();//----- Lectura del superBloque

	}else{

		printf("el archivo no existe\n");

		log_info(logger, "iniciando filesystem... \n");

		crearDirectorios();

		superBloque = crearSuperBloque(); ////// crea el superBloque

		log_info(logger, "superBloque creado \n");

		log_info(logger,"creado bloque...\n");

		crearBloques(superBloque);// ------------------- crea al bloque --------------------

		log_info(logger,"bloque.bin creado \n");
	}


	//-------------------Lectura/Generar copia de Bloques -------------//


	leerBloques();

	log_info(logger, "filesystem iniciado \n");

	//crearArchivo("Oxigeno");
	//escribirBloque(obtenerFile("Oxigeno"));
	/*file_t *aux;
	printf("insertando bloques \n");
	insertarEnBloque("Oxigeno",200);
	insertarEnBloque("Oxigeno",200);
	printf("entro 2\n");

	leerArchivo(&aux,"Oxigeno");

	printf("entro a update\n");
	updateIms(&fileOxigeno);
	printf("entro a leer 2\n");
	leerArchivo(&aux,"Oxigeno");
	readline(" -ya sali--");
	readline(" -ya sali--");*/
//---------------------------------------------------------------------------------//

	/*insertarEnBloque("Oxigeno",100);
	insertarEnBloque("Basura",100);
	insertarEnBloque("Comida",100);
	printf("caracter de llenado = %c\n",fileOxigeno->caracterLlenado);
	update();
	printf("caracter de llenado = %c\n",fileOxigeno->caracterLlenado);
	sabotaje1();
	printf("caracter de llenado = %c\n",fileOxigeno->caracterLlenado);
	sabotaje2();

	if(access(string_from_format("%s/Files/Oxigeno.ims", puntoDeMontaje),F_OK)==0){
		printf("caracter de llenado = %c\n",fileOxigeno->caracterLlenado);
			readline(" ddddd");
		 sabotaje3("Oxigeno");
		 sabotaje4("Oxigeno");
		 sabotaje5("Oxigeno");
	}

	if(access(string_from_format("%s/Files/Basura.ims", puntoDeMontaje),F_OK)==0){
		sabotaje3("Basura");
		sabotaje4("Basura");
		sabotaje5("Basura");
	}

	if(access(string_from_format("%s/Files/Comida.ims", puntoDeMontaje),F_OK)==0){
		 sabotaje3("Comida");
		 sabotaje4("Comida");
		 sabotaje5("Comida");
	}

	readline(" ");*/

	pthread_t hiloSincronizador;
	pthread_create(&hiloSincronizador,NULL,&sincronizar,NULL);

	log_info(logger, "Iniciando servidor I Mongo Store \n");

	int server= iniciar_servidor(IP,puerto);

	log_info(logger, "I Mongo Store: - listo para recibir al cliente");


//-------------------------------------------------------------------------------------------------------------



	printf("\nsdsdPID: %lu\n", getpid());

	pthread_t hilos[MAXCLIENTES];
	int clientes[MAXCLIENTES];
	int id=0;

	while(salir<MAXCLIENTES){
		if(id<MAXCLIENTES){
			clientes[id] = esperar_cliente(server);
			if(id==0){
				socket_disc=clientes[id];
			}
			log_info(logger, "I Mongo Store: - Cliente Conectado");
			pthread_create(&hilos[id],NULL,&escuchar,clientes[id]);
			id++;
		}

	}
	readline("Presione enter para salir>");

}


int escuchar(int cliente){
	char *msg;
	void * buffer;
	int size;
	int param;
	int estado=0;
	file_t aux;
	t_paquete* p;
	while(1){


		int cod_op = recibir_operacion(cliente);
		printf("COD_OP(16-21): %d\n",cod_op);
		switch(cod_op)
		{
		case GENERAR_OXIGENO:
			sem_wait(ocupado);
			largo_paquete(cliente);
			param = *(int*)recibir(cliente);
			printf("parametro: %d\n",param);

			//todo controlar errores que devuelva int
			insertarEnBloque("Oxigeno",param);

			/*p = crear_paquete(GENERAR_OXIGENO);
			agregar_a_paquete(p,&estado,sizeof(int));
			enviar_paquete(p,cliente);
			eliminar_paquete(p);*/
			sem_post(ocupado);
			break;
		case CONSUMIR_OXIGENO:
			sem_wait(ocupado);
			largo_paquete(cliente);
			param = *(int*)recibir(cliente);

			printf("parametro: %d\n",param);

			eliminarDeBloque("Oxigeno",param);

			sem_post(ocupado);
				break;

		case GENERAR_COMIDA:
			sem_wait(ocupado);

				largo_paquete(cliente);
				param = *(int*)recibir(cliente);
				printf("parametro: %d\n",param);

				//todo controlar errores que devuelva int
				insertarEnBloque("Comida",param);

				sem_post(ocupado);
				break;

		case CONSUMIR_COMIDA:
			printf("estoy ocupado\n");
			sem_wait(ocupado);
			printf("voy a recibir cosas\n");

			largo_paquete(cliente);
			param = *(int*)recibir(cliente);
			printf("parametro: %d\n",param);

			eliminarDeBloque("Comida",param);

			sem_post(ocupado);
			break;

		case GENERAR_BASURA:
			sem_wait(ocupado);
			largo_paquete(cliente);
			param = *(int*)recibir(cliente);
			printf("parametro: %d\n",param);

			//todo controlar errores que devuelva int
			insertarEnBloque("Basura",param);

			sem_post(ocupado);
			break;

		case DESCARTAR_BASURA:
			sem_wait(ocupado);
			largo_paquete(cliente);
			param = *(int*)recibir(cliente);
			printf("parametro: %d\n",param);

			eliminarDeBloque("Basura",param);

			sem_post(ocupado);
			break;
		case FSCK :
			estado=0;
			estado+= sabotaje1();
			estado+= sabotaje2();

			if(access(string_from_format("%s/Files/Oxigeno.ims", puntoDeMontaje),F_OK)==0){
				estado+= sabotaje3("Oxigeno");
				estado+= sabotaje4("Oxigeno");
				estado+= sabotaje5("Oxigeno");
			}

			if(access(string_from_format("%s/Files/Basura.ims", puntoDeMontaje),F_OK)==0){
				estado+=sabotaje3("Basura");
				estado+=sabotaje4("Basura");
				estado+=sabotaje5("Basura");
			}

			if(access(string_from_format("%s/Files/Comida.ims", puntoDeMontaje),F_OK)==0){
				estado+= sabotaje3("Comida");
				estado+= sabotaje4("Comida");
				estado+= sabotaje5("Comida");
			}

			p= crear_paquete(FSCK);
			agregar_a_paquete(p,&estado,sizeof(int));
			enviar_paquete(p,cliente);
			eliminar_paquete(p);
			sem_post(ocupado);
			break;
		case -1:
			log_warning(logger,"el cliente se desconecto");
			salir++;
			return EXIT_FAILURE;
		default:
			sem_wait(ocupado);
			printf("Operacion desconocida.");
			sem_post(ocupado);
			break;
		}

	}
	return EXIT_SUCCESS;
}

