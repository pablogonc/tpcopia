/*
 * tareas.h
 *
 *  Created on: 18 jun. 2021
 *      Author: utnso
 */

#ifndef TAREAS_H_
#define TAREAS_H_

#include "utilidades_discordiador.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <commons/collections/list.h>

int moverse(uint32_t,uint32_t,Tripulante *);

int moversex(uint32_t*,Tripulante *);

int moversey(uint32_t*,Tripulante *);

int esperar(int*,Tripulante *);

int invocarFSCK(int*,Tripulante * tripulante);

void separarEnInstrucciones(char*,t_list*,Tripulante*);
int operacionES(int* ,Tripulante* );
int reconocerTarea(char* );



typedef struct {
    int (*instrucion)(int*,Tripulante*);
    int parametro;
}Instruccion;

typedef enum
{
	//consola
	Tarea_Ejecutada,
	Tarea_Completada,
	Tarea_ES
}estadosTarea;

#endif /* TAREAS_H_ */
