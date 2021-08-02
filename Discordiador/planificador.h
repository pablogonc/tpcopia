/*
 * planificador.h
 *
 *  Created on: 2 ago. 2021
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>
//Globales
bool planificador_inciado;
bool planificador_saboteado;
//semaforos
sem_t* planificador_planificador;
sem_t* planificador_ejecucion;

#endif /* PLANIFICADOR_H_ */
