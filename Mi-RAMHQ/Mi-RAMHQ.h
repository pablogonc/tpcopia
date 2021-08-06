/*
 * Mi-RAMHQ.h
 *
 *  Created on: 19 jun. 2021
 *      Author: utnso
 */
#ifndef MIRAMHQH
#define MIRAMHQH

#include <stdint.h>

typedef struct {
    uint32_t pid;
    uint32_t tareas;
}pcb;

typedef struct {
    uint32_t tid;
    char estado;
    uint32_t posicionX;
    uint32_t posicionY;
    uint32_t proximaInstruccion;
    uint32_t punteroPCB;
}tcb;

typedef struct {
    int pid;
    t_list* tareas;
}Tareas;

t_log* logger;

#endif /* MIRAMHQH */
