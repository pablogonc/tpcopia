/*
 * tripulante.c
 *
 *  Created on: 24 may. 2021
 *      Author: utnso
 */

#include "tripulante.h"


void crear_tripulante(Tripulante * tripulante){

	pthread_t hiloTripulante;
	pthread_create(&hiloTripulante,NULL,&iniciar_tripulante,tripulante);

}

int iniciar_tripulante(Tripulante * tripulante){

	char * tarea;
	int ram_socket;
	int store_socket;
	tripulante->instrucciones = list_create();

	char*exit = malloc(sizeof(char)*5);
	exit= "EXIT";
	while(1){
		sem_wait(semTripulantes);
		sem_post(semTripulantes);
		sem_wait(tripulante->sem);

		if(strcmp((tripulante->estado),exit)==0){
			liberar_conexion(ram_socket);
			liberar_conexion(store_socket);
			return 1;
		}



		switch(getEstado(tripulante->tid)){
			case LLEGADA:

				ram_socket = conectar_con("mi-RAM_HQ.config");
				tripulante->ramSocket=ram_socket;
				store_socket = conectar_con("store.config");
				tripulante->storeSocket=store_socket;
				informar_inicio(tripulante,ram_socket);
				tarea = pedirTarea(tripulante,ram_socket);
				if( tarea != NULL ){ //habian tareas para asignar
					separarEnInstrucciones(tarea,tripulante->instrucciones,tripulante);
				}
				//pthread_mutex_lock(&mutexTripulantes);
				if(planificando){
					sem_post(semPlanificacion);
				}

				//pthread_mutex_unlock(&mutexMain);

				break;
			case TRABAJANDO:
				trabajando(tripulante, tripulante->instrucciones, ram_socket, store_socket, tarea);

				break;
			case BLOQUEADOEM:
				//TODO
				if(tripulante->designado==1){
					atenderSabotaje(tripulante, tripulante->instrucciones);
				}
				break;
			case BLOQUEADOES:
				//TODO
				bloqueoES(tripulante);

				tarea = pedirTarea(tripulante,ram_socket);
				if( tarea != NULL ){ //habian tareas para asignar
					separarEnInstrucciones(tarea,tripulante->instrucciones,tripulante);
				}
				//pthread_mutex_lock(&mutexTripulantes);
				if(planificando){
					sem_post(semPlanificacion);
				}
				break;
			case EXIT:
				printf("error 404, file not found\n");

				liberar_conexion(ram_socket);
				liberar_conexion(store_socket);
				return 1;
				break;
		}


	}
}
void bloqueoES(Tripulante * tripulante){


	int op = recibir_operacion(tripulante->storeSocket);
	largo_paquete(tripulante->storeSocket);


	if ( 0==( *(int*) recibir(tripulante->storeSocket)) ){ //Ok
		log_info(logger,"Tripulante %d: Operacion E/S (%s) Finalizada con exito",tripulante->tid,nombreOperacionES(op));
	}else{// :/
		log_warning(logger,"Tripulante %d: Operacion E/S No se pudo completar :/",tripulante->tid);
	}



}

void prepararSabotaje(Tripulante * tripulante,t_list* instrucciones){
	t_list* aux = list_create();

	Instruccion* instruccion = malloc(sizeof(Instruccion));
	instruccion->instrucion=moversex;
	instruccion->parametro=&sabotaje_curso.x;
	list_add(aux,instruccion);
	instruccion = malloc(sizeof(Instruccion));
	instruccion->instrucion=moversey;
	instruccion->parametro=&sabotaje_curso.y;
	list_add(aux,instruccion);

	instruccion = malloc(sizeof(Instruccion));
	instruccion->instrucion=invocarFSCK;
	instruccion->parametro=&sabotaje;
	list_add(aux,instruccion);


	while(0<list_size(instrucciones)){
		list_add(aux,list_remove(instrucciones,0));
	}

	while(0<list_size(aux)){
		list_add(instrucciones,list_remove(aux,0));
	}
	//todo
	list_destroy(aux);
}

void atenderSabotaje(Tripulante * tripulante,t_list* instrucciones){
	//todo
	printf("entro\n");
	int (*instruccion)(int*,Tripulante*);
	Instruccion* inst = list_get(instrucciones,0);
	instruccion = inst->instrucion;
	int retorno = instruccion((inst->parametro),tripulante);
	if(retorno==0 || retorno == -5){
		list_remove(instrucciones,0);

	}
	if(retorno ==-5){
		printf("Tripulante que arreglo todo: %d\n",tripulante->tid);
		sem_post(semLiberarTripulantes);
	}else{
		sem_post(tripulante->sem);
	}



	//sabotaje = retorno; //instruccion va devolver algo cuando termino de atender el sabotaje

}


void trabajando(Tripulante * tripulante,t_list* instrucciones, int ram_socket, int store_socket, char* tarea){
	int laburo=0;
	if(list_size(instrucciones)==0){
	tarea = pedirTarea(tripulante,ram_socket);

		if( tarea != NULL ){ //habian tareas para asignar
			separarEnInstrucciones(tarea,instrucciones,tripulante);
		}

		if(planificando){
			sem_post(semPlanificacion);
		}

	}else{
		laburo = trabajar(tripulante,instrucciones, ram_socket, store_socket, tarea);

		if(laburo == -3){
			tripulante->estado ="BLOQUEADOES";
			//pthread_mutex_lock(&mutexTripulantes);
			if(planificando){
				sem_post(semPlanificacion);
			}
			//sem_wait(tripulante->sem);
		}else{

			if(laburo == 1){
				sleep(1); //esperar 1 segundos entre paso
				tripulante->quantum += -1;
				if(tripulante->quantum == 0){
					//pthread_mutex_lock(&mutexTripulantes);
					if(planificando){
						sem_post(semPlanificacion);
					}
				}else{
					sem_post(tripulante->sem);
				}
			}

		}


	}


}

void informar_inicio(Tripulante * tripulante, int ram_socket){

	t_paquete* p;
	p=crear_paquete(INICIAR_TRIPULANTE);//INICIAR_TRIPULANTE

	//opcode largoPaquete, largoPid, pid, largoX , x , largoY , y, largoTID, tid

	agregar_a_paquete(p,&tripulante->pid,sizeof(uint32_t));
	agregar_a_paquete(p,&tripulante->X,sizeof(uint32_t));
	agregar_a_paquete(p,&tripulante->Y,sizeof(uint32_t));
	agregar_a_paquete(p,&tripulante->tid,sizeof(uint32_t));
	pthread_mutex_lock(&send);
	enviar_paquete(p,ram_socket);
	pthread_mutex_unlock(&send);
	eliminar_paquete(p);

// esperar el ok de la ram
/*
	recibir_operacion(ram_socket);
	largo_paquete(ram_socket);
	recibir(ram_socket); // esto va dentro del if para comprobar que este to-do ok pero por ahora para que no tenga basura
*/
	/*
	if( tripulante->tid !=(*(int*)recibir(ram_socket)))

	}else{

	}*/
}


//TODO
int trabajar(Tripulante * tripulante,t_list* instrucciones, int ram_socket, int store_socket, char* tarea){

	int (*instruccion)(int*,Tripulante*);
	Instruccion* inst = list_get(instrucciones,0);
	instruccion = inst->instrucion;

	int retorno = instruccion(&(inst->parametro),tripulante);
	if(retorno==0){
		list_remove(instrucciones,0);
		//trabajando(tripulante, instrucciones, ram_socket, store_socket, tarea);
		return 0;
	}
	if(retorno == -3){
		return -3;
	}
	return 1;
}

char * pedirTarea(Tripulante * tripulante,int socketRam){
	t_paquete* p;
	p=crear_paquete(ENVIAR_PROXIMA_TAREA);
	//agregar_a_paquete(p,&tripulante->pid,sizeof(uint32_t));
	//agregar_a_paquete(p,&tripulante->tid,sizeof(uint32_t));
	enviar_paquete(p,socketRam);
	eliminar_paquete(p);

	int op = recibir_operacion(socketRam); // recibir_operacion(socketRam);

	switch(op){
		case RECIBIR_TAREAS:

//			confirmarPreparacion(tid);
//			return NULL;//(char*) recibir(socketRam);

			largo_paquete(socketRam);

			tripulante->estado = "LISTO";

			return (char*) recibir(socketRam);
			break;

		case NOHAYTAREAS:
			largo_paquete(socketRam);
			expulsar(tripulante->tid,socketRam);
			return NULL;
			break;
		default: //CODIGO NO ESPERADO
			//ERROR
			break;
	}
	largo_paquete(socketRam);
}

