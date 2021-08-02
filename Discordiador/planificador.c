/*
 * planificador.c
 *
 *  Created on: 2 ago. 2021
 *      Author: utnso
 */
#include "planificador.h"

void planificador_iniciarVariables(){
	//globales
	planificador_inciado = false;
	bool planificador_saboteado = false;
	//semaforos
	planificador_planificador = (sem_t*)malloc(sizeof(sem_t));
	sem_init(planificador_planificador,1,0);

}

void planificador_iniciarPlanificacion(){
	if(!planificador_inciado){
		sem_post(planificador_planificador);
	}
}

void planificador_detenerPlanificacion(){
	if(planificador_inciado){
		sem_wait(planificador_planificador);
	}
}


void planificador_planificar(){

}

void planificador_correr(){
	while(1){
		if(planificador_saboteado){

		}else{
			planificador_planificar();
		}
	}
}
