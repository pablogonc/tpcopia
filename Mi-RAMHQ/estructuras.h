#ifndef estructuras_H
#define estructuras_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct pcb{
    uint32_t pid;
    uint32_t tareas;
}pcb;

typedef struct tcb{
    uint32_t tid;
    char estado;
    uint32_t posicionX;
    uint32_t posicionY;
    uint32_t proximaInstruccion;
    uint32_t punteroPCB;
}tcb;

typedef struct nodoOcupanciaMemoria{
    int inicio;
    uint32_t codigoUnico;
    int final;
    struct nodoOcupanciaMemoria *sig;
}nodoOcupanciaMemoria;

typedef nodoOcupanciaMemoria* ocupanciaPuntero; //Puntero al tipo de dato nodoOcupanciaMemoria para no utilizar punteros de punteros

typedef struct nodoPCB{
	uint32_t pcbEnMemoria;
    uint32_t id;
    uint32_t codigoUnicoPcb;
    uint32_t codigoUnicoTareas;
    struct nodoPCB *sig;
}nodoPCB;

typedef struct nodoTCB{
	uint32_t tcbEnMemoria;
    uint32_t id;
    uint32_t codigoUnico;
    struct nodoTCB *sig;
}nodoTCB;

#endif
