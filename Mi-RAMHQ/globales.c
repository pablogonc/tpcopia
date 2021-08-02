#include <commons/log.h>
#include <semaphore.h>
#include <stdlib.h>
#include "estructuras.h"

int DEBUG_MODE;

t_log* logger;
sem_t* socket1;
sem_t* memoria;
sem_t* mapa;
sem_t* loguear;
sem_t* colaTripulantes;
sem_t* signalThread;

pthread_t hiloSignals;

void* memoriaPrincipal;
uint32_t TAMANIOMEMORIA;

ocupanciaPuntero LISTAOCUPANCIA;
nodoPCB* LISTAPATOTAS;
nodoTCB* LISTATRIPULANTES;

uint32_t acumuladorPatotas;
uint32_t acumuladorTripulantes;
uint32_t acumuladorCodigoUnico;

int (*agregarPatota) (pcb,uint32_t);
pcb* (*buscarPatota) (uint32_t);
int (*quitarPatota) (uint32_t);
int (*agregarTripulante) (tcb, uint32_t);
tcb* (*buscarTripulante) (uint32_t);
int (*quitarTripulante) (uint32_t);
int (*agregarInstrucciones) (uint32_t, uint32_t, void*);
void* (*buscarInstruccion) (uint32_t, int);
int (*quitarInstrucciones) (uint32_t);
int (*cambiarUbicacion) (uint32_t, uint32_t, uint32_t);
int (*criterioDeSegmentacion)(int);
int (*dumpMemoria)(void);
