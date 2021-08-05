/*
 * tripulante.h
 *
 *  Created on: 24 may. 2021
 *      Author: utnso
 */

#ifndef TRIPULANTE_H_
#define TRIPULANTE_H_

#include <commons/log.h>
#include "tripulante.h"
#include <cliente.h>
#include <utilidades.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "discordiador.h"
#include "utilidades_discordiador.h"
#include <stdint.h>
#include "tareas.h"

void crear_tripulante(Tripulante *);

int iniciar_tripulante(Tripulante *);

void trabajando(Tripulante*,t_list*, int, int,char*);

void informar_inicio(Tripulante*,int);

int trabajar(Tripulante*,t_list*);

char * pedirTarea(Tripulante *,int);

void prepararSabotaje(Tripulante * ,t_list* );


void solicitar_proxima_tarea();

void ejecutar_tarea();

void bloqueoES(Tripulante *,char*);
#endif /* TRIPULANTE_H_ */
