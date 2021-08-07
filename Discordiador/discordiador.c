/*
 * discordiador.c
 *
 *  Created on: 8 may. 2021
 *      Author: utnso
 */

#include "discordiador.h"

//variables
///////
int gradoMultiTarea;
char* algoritmoPlanificacion="RR";
int quantum=3;
int salir = 0;


void escuchar(int socket[]){

	while(1){

		int cod_op = recibir_operacion(socket[STORE]);
		switch(cod_op){

			case SABOTAJE:
				printf("sabotaje\n");
			/**/largo_paquete(socket[STORE]);


				sabotaje_curso.x = /*8;*/*(uint32_t*) recibir(socket[STORE]);
				sabotaje_curso.y = /*7;*/*(uint32_t*) recibir(socket[STORE]);
				sabotaje=1;
				sabotaje_curso.estadoPlanificador= planificando;
				printf("Atendiendo sabotaje\n");
				bloquearTripulantesEM(socket);

				break;
			default:
				log_warning(logger,"OPERACION DESCONOCIDA");
				break;
		}
	}
}

int planificadorOcupado;

void iniciarVariables(){
	//globales
	planificadorOcupado=0;
	sabotaje=0;
	planificando = 0;

	//config

	t_config* discoConfig = config_create("discordiador.config");
	gradoMultiTarea = config_get_int_value(discoConfig,"GRADO_MULTITAREA");
	algoritmoPlanificacion = config_get_string_value(discoConfig,"ALGORITMO");
	quantum = config_get_int_value(discoConfig,"QUANTUM");
	duracionSabotaje = config_get_int_value(discoConfig,"DURACION_SABOTAJE");
	retardoCPU = config_get_int_value(discoConfig,"RETARDO_CICLO_CPU");


	//semaforos
	semLiberarTripulantes = (sem_t*)malloc(sizeof(sem_t));
	sem_init(semLiberarTripulantes,1,0);

	semPlanificacion  = (sem_t*)malloc(sizeof(sem_t));
	sem_init(semPlanificacion,1,0);

	semTripulantes = (sem_t*)malloc(sizeof(sem_t));
	sem_init(semTripulantes,1,0);

	semMain = (sem_t*)malloc(sizeof(sem_t));
	sem_init(semMain,1,0);

	//listas
	//creo la lista de patotas y tripulantes (provisional)
	lista_patotas = list_create();
	lista_tripulantes = list_create();
	Llegada = list_create();
	Listo = list_create();
	BloqueadoEM = list_create();
	BloqueadoES = list_create();
	Finalizado = list_create();
	Trabajando = list_create();
}

int main(void){
	readline("Discordiador >");

	printf("Inciando modulo discordiador \n");

	logger = log_create("Discordiador.log", "Discordiador", 1, LOG_LEVEL_DEBUG);

	int socket[2];
	socket[RAM] = conectar_con_ram("discordiador.config");
	socket[STORE] = conectar_con_store("discordiador.config");

	iniciarVariables();

	pthread_t escstore;
	pthread_create(&escstore,NULL,&escuchar,socket);

	pthread_t hConsola;
	pthread_create(&hConsola,NULL,&consola,socket);


	while(!salir){
		sem_wait(semMain);
		if(sabotaje){
			sem_wait(semLiberarTripulantes);
			//sleep(3);
			printf("Desbloqueado todo\n");
			desbloquearTripulantes();
		}
		if(planificando){
			planificar();
			sem_post(semMain);
		}

	}

	pthread_join(hConsola,NULL);
	terminar_programa(socket);

}

void procesar_mensaje(char *leido,int socket[2]){
	char * token;
	token = strtok(leido," ");
	int op=operacion(token);
	token =strtok(NULL,"\n");
	switch(op)
		{
			case INICIAR_PATOTA:
				iniciarPatota(token,socket[RAM]);
				break;
			case LISTAR_TRIPULANTES:
				listarTripulantes();
				break;
			case EXPULSAR_TRIPULANTE:
				expulsarTripulante(token,socket[RAM]);
				break;
			case INICIAR_PLANIFICACION:
				iniciarPlanificacion();
				break;
			case PAUSAR_PLANIFICACION:
				pausarPlanificacion();
				break;
			case OBTENER_BITACORA:
				obtenerBitacora();
				break;
			case DUMP_MEMORIA:
				dumpMemoria(socket[RAM]);
				break;
			case 456:
				bloquearTripulantesEM(socket);
				break;
			default:
				log_warning(logger,"OPERACION DESCONOCIDA");
				break;
		}

}

void consola(int socket[2]){
	menu();
	char* leido;
	leido = readline("Discordiador >");

	while(strcmp(leido,"exit") !=0){

		procesar_mensaje(leido,socket);
		free(leido);
		leido = readline("Discordiador >");
	}

	sem_post(semMain);
	sem_post(semPlanificacion);
	sem_post(semLiberarTripulantes);
	salir = 1;
}

void iniciarPatota(char * instruccion,int socketRam){
//obtengo cantidad de tripulantes
	int tripulantes = atoi(strtok(instruccion," "));

//obtengo las tareas
	char * tareas = strtok(NULL," ");

//obtener todas las instrucciones
	size_t len = 0;
	FILE* f = fopen(tareas,"r");
	char* linea;

// opcode, length paquete, length tarea, tarea, ...
	t_paquete* paquete;
	paquete = crear_paquete(INICIAR_PATOTA);

	void * aux;
	int acumulador = 0;
	//uint32_t *tamanio = malloc(sizeof(uint32_t));

	while(!feof(f)){ //
		getline(&linea,&len,f); //esto me mete todo el texto junto no lo manda por linea
		aux = realloc(aux,strlen(linea)+1+acumulador);

		/*
		aux = realloc(aux,strlen(linea)+sizeof(uint32_t)+1+acumulador);
		*tamanio = strlen(linea)+1;
		memcpy(aux+acumulador,tamanio,sizeof(uint32_t));
		acumulador += sizeof(uint32_t);
		*/

		memcpy(aux+acumulador,linea,strlen(linea)+1);
		acumulador += strlen(linea)+1;
	}
	/*
	aux = realloc(aux,sizeof(uint32_t)+acumulador);
	*tamanio = 0;
	memcpy(aux+acumulador,tamanio,sizeof(uint32_t));
	acumulador += sizeof(uint32_t);
	 */

	agregar_a_paquete(paquete,aux,acumulador);

	enviar_paquete(paquete,socketRam);
	eliminar_paquete(paquete);

	recibir_operacion(socketRam);
	largo_paquete(socketRam);

	uint32_t pid = *(uint32_t*)recibir(socketRam);
	list_add(lista_patotas,pid);

//creo a cada tripulante

	char* token;
	for(int i=0;i<tripulantes;i++){

//agrego tripulantes  a listas

		Tripulante *nTripulante= malloc(sizeof(Tripulante));

		nTripulante->pid=pid;
		nTripulante->tid = (uint32_t)list_size(lista_tripulantes);
		nTripulante->estado = "LLEGADA";

		nTripulante->sem = (sem_t*)malloc(sizeof(sem_t));
		sem_init(nTripulante->sem,1,1);

		nTripulante->designado = 0;//todo
//agrego tripulantes  a listas

		list_add(lista_tripulantes,nTripulante);
		list_add(Llegada,nTripulante);
		token= strtok(NULL,"|");


		 	if(strcmp(algoritmoPlanificacion,"RR")==0){
				nTripulante->quantum = 3;
			}else if(strcmp(algoritmoPlanificacion,"FIFO")==0){
				nTripulante->quantum = -1;
			}


		if(token!=NULL && strcmp(token," ")!=0){
			nTripulante->X = (uint32_t)atoi(token);
			nTripulante->Y = (uint32_t)atoi(strtok(NULL," "));

		}else{
			nTripulante->X = 0;
			nTripulante->Y = 0;
		}
		crear_tripulante(nTripulante);
	}
}

void iniciarPlanificacion(){
	if (planificando==0){
		planificando=1;
		sem_post(semMain);
		sem_post(semPlanificacion);
		sem_post(semTripulantes);

	}

}

void planificar(){
	sem_wait(semPlanificacion);
	sem_wait(semTripulantes);
	Tripulante* tripulante;

	for(int i =0;i<list_size(Trabajando);i++){
			tripulante = list_get(Trabajando,i);
			if(strcmp(tripulante->estado,"BLOQUEADOES")==0){
				log_info(logger, "Tripulante movido  a BLOQUEADOES");
				mover_tripulante(Trabajando,BloqueadoES,tripulante->tid,"BLOQUEADOES");
				i--;
			}
		}

	//es
		for(int i =0;i<list_size(BloqueadoES);i++){
				tripulante = list_get(BloqueadoES,i);

				if(strcmp(tripulante->estado,"LISTO")==0){
					//log_info(logger, "Tripulante movido  a listo");
					mover_tripulante(BloqueadoES,Listo,tripulante->tid,"LISTO");
					//sem_wait(tripulante->sem);
					i--;
				}
			}

		//


	for(int i = 0; i<list_size(Llegada); i++){
		tripulante = list_get(Llegada,i);
		if(strcmp(tripulante->estado,"LISTO")==0){

			mover_tripulante(Llegada,Listo,tripulante->tid,"LISTO");
			//sem_wait(tripulante->sem);
			i--;
		}
	}

	//pasamos los tripulantes que dejaron de trabajar y estan ready a listo

		for(int i =0;i<list_size(Trabajando);i++){
				tripulante = list_get(Trabajando,i);
				if(strcmp(tripulante->estado,"LISTO")==0){
					mover_tripulante(Trabajando,Listo,tripulante->tid,"LISTO");
					//sem_wait(tripulante->sem);
					i--;
				}
		if(strcmp(algoritmoPlanificacion,"RR") == 0 && tripulante->quantum == 0){
			mover_tripulante(Trabajando,Listo,tripulante->tid,"LISTO");
			//sem_wait(tripulante->sem);
			i--;
		}
	}


	while(list_size(Trabajando)<gradoMultiTarea && list_size(Listo) > 0 ){

		tripulante= list_remove(Listo,0);

		if(strcmp(algoritmoPlanificacion,"RR")==0){
			tripulante->quantum = quantum;
		}else if(strcmp(algoritmoPlanificacion,"FIFO")==0){
			tripulante->quantum = -1;
		}
		tripulante->estado="TRABAJANDO";
		list_add(Trabajando,tripulante);
		sem_post(tripulante->sem);
	}


	sem_post(semTripulantes);

}

void listarTripulantes(){
	time_t t;
	struct tm *tm;
	char fechayhora[100];

	t=time(NULL);
	tm=localtime(&t);
	strftime(fechayhora, 100, "%d/%m/%Y %H:%M:%S", tm);

	printf("--------------------------------------------------------------------\n");
	printf("Estado de la nave: %s\n",fechayhora);

	mostrar(Llegada,"Llegada");
	mostrar(Listo,"Listo");
	mostrar(Trabajando,"Trabajando");
	mostrar(Finalizado,"Terminado");
	mostrar(BloqueadoEM,"BloqueadoEM");
	mostrar(BloqueadoES,"BloqueadoES");
	printf("--------------------------------------------------------------------\n");
}

void pausarPlanificacion(){
	if(planificando){

		planificando = 0;
		sem_post(semPlanificacion);
		sem_wait(semTripulantes);
	}

}

void obtenerBitacora(){}

void dumpMemoria(int socketRam){//todo
	t_paquete* paquete;
	paquete = crear_paquete(DUMP_MEMORIA);
	enviar_paquete(paquete,socketRam);
	eliminar_paquete(paquete);
}

int buscarTripulante(t_list * lista,uint32_t tidTarget){
	Tripulante* tripulante;
	for(int i=0; i<list_size(lista); i++){
		tripulante = list_get(lista,i);
		if(tripulante->tid==tidTarget){
			return i;
		}
	}
	return -1;
}


void terminar_programa(int socket[2]){
	liberar_conexion(socket[RAM]);
	liberar_conexion(socket[STORE]);
	log_destroy(logger);
}

void expulsar(uint32_t tid,int socketRam){
	switch(getEstado(tid)){
		case LLEGADA:
			mover_tripulante(Llegada,Finalizado,tid,"EXIT");
			break;
		case LISTO:
			mover_tripulante(Listo,Finalizado,tid,"EXIT");
			break;
		case TRABAJANDO:
			mover_tripulante(Trabajando,Finalizado,tid,"EXIT");
			break;
		case BLOQUEADOEM://todo
			mover_tripulante(BloqueadoEM,Finalizado,tid,"EXIT");
			break;
		case BLOQUEADOES://todo
			mover_tripulante(BloqueadoES,Finalizado,tid,"EXIT");
			break;
		default:
			log_warning(logger,"el tripulante ya esta muerto \n");
			break;
	}
	//todo avisar a ram

	t_paquete* p = crear_paquete(EXPULSAR_TRIPULANTE);
	agregar_a_paquete(p, &tid,sizeof(uint32_t));
	enviar_paquete(p,socketRam);
	eliminar_paquete(p);
	//TODO avisar ram REWRWER
}

void mover_tripulante(t_list * origen,t_list * destino,uint32_t tid,char * estado){
	int indice = buscarTripulante(origen,tid);
	Tripulante* tripulante = list_remove(origen,indice);
	tripulante->estado=estado;
	list_add(destino,tripulante);
}

int getEstado(uint32_t tid){
	int indice = buscarTripulante(lista_tripulantes,tid);

	Tripulante* tripulante = list_get(lista_tripulantes,indice);

	if (strcmp(tripulante->estado,"LLEGADA") ==0)
			return LLEGADA;
	if (strcmp(tripulante->estado,"LISTO") ==0)
			return LISTO;
	if (strcmp(tripulante->estado,"TRABAJANDO") ==0)
			return TRABAJANDO;
	if (strcmp(tripulante->estado,"BLOQUEADOEM") ==0)
			return BLOQUEADOEM;
	if (strcmp(tripulante->estado,"BLOQUEADOES") ==0)
				return BLOQUEADOES;
	if (strcmp(tripulante->estado,"EXIT") ==0)
			return EXIT;
	return -1;

}

Tripulante* getTripulante(t_list* lista,uint32_t tid){
	int indice = buscarTripulante( lista,tid);
	return list_get(lista,indice);
}

void expulsarTripulante(char * token,int socketRam){ //castea el char del tid a uint32_t y llama a la funcion expulsar
	uint32_t tid = (uint32_t)atoi(token); //editado antes lo ponia en un int directamente
	expulsar((int) tid,socketRam);

}


void bloquearTripulantesEM(int socket[]){
//todo


	pausarPlanificacion();


	sem_post(semMain);


	Tripulante* tripulante;
	bool orderById(Tripulante* a, Tripulante* b){
		return (a->tid) < (b->tid);
	}

	t_list* aux = list_sorted(Trabajando,&orderById);

	for(int i = 0; i<list_size(aux); i++){
		tripulante = list_get(aux,i);
		if(tripulante->quantum!=0){
			sem_wait(tripulante->sem);
		}

		mover_tripulante(Trabajando,BloqueadoEM,tripulante->tid,"BLOQUEADOEM");

		//list_remove(aux,i);
		//i--;
	}
	list_destroy(aux);

	aux = list_sorted(BloqueadoES,&orderById);
	for(int i=0;i<list_size(aux);i++){
		tripulante = list_get(aux,i);

		mover_tripulante(BloqueadoES,BloqueadoEM,tripulante->tid,"BLOQUEADOEM");
	}
	list_destroy(aux);
	aux = list_sorted(Listo,&orderById);

	for(int i = 0; i<list_size(aux); i++){
		tripulante = list_get(aux,i);
		mover_tripulante(Listo,BloqueadoEM,tripulante->tid,"BLOQUEADOEM");
		list_remove(aux,i);
		i--;
	}

	//todo

		Tripulante* cercano;
		for(int i=0; i<list_size(BloqueadoEM);i++){
			tripulante = list_get(BloqueadoEM,i);
			if(i==0){
				cercano=tripulante;
			}else if (distancia(tripulante,sabotaje_curso) < distancia(cercano,sabotaje_curso)){
				cercano=tripulante;
			}
		}


		prepararSabotaje(cercano, cercano->instrucciones);
		cercano->designado=1;
		sem_post(semTripulantes);
		sem_post(cercano->sem);


}
void desbloquearTripulantes(){
	//todo
	Tripulante* tripulante;
	for(int i = 0; i<list_size(BloqueadoEM); i++){
		tripulante = list_get(BloqueadoEM,i);
		if(list_size(Trabajando)<gradoMultiTarea && list_size(BloqueadoEM) > 0 ){

			mover_tripulante(BloqueadoEM,Trabajando,tripulante->tid,"TRABAJANDO");
			sem_post(tripulante->sem);
			i--;
		}else{

			mover_tripulante(BloqueadoEM,Listo,tripulante->tid,"LISTO");
			i--;
		}
	}
	if(sabotaje_curso.estadoPlanificador){
		iniciarPlanificacion();
	}
	/* contemplaba si se estaba planificando antes de todo pero creo que mejor la dejo en pausa por defecto
	if(planificando==1){
		sem_post(semPlanificacion);
	}else{
		sem_wait(semTripulantes);
		sem_wait(semMain);
	}
	 */
	sabotaje=0;
}

uint32_t distancia(Tripulante* tripulante,Sabotaje sabotaje){ //todo sin testear
	int distanciax =  tripulante->X - sabotaje.x;
	int distanciay =  tripulante->Y - sabotaje.y;
	return sqrt( (pow(distanciax,2)+ pow(distanciay,2)) );
}


