#ifndef RAM_utilidades_H
#define RAM_utilidades_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "globales.c"

#define string char*

uint32_t nuevoTripulante(uint32_t, uint32_t, uint32_t, uint32_t);
int avanzarTarea(tcb*);
int expulsarTripulante(uint32_t);

void reservarMemoria (uint32_t);
void configurarMemoriaPaginacion(uint32_t,uint32_t,string,string);
void configurarMemoriaSegmentacion(string);
void initGlobales();
void initSemaphores();

int agregarPatotaPaginacion (pcb);
pcb* buscarPatotaPaginacion (uint32_t);
int quitarPatotaPaginacion (uint32_t);
int agregarTripulantePaginacion (tcb);
tcb* buscarTripulantePaginacion (uint32_t);
int quitarTripulantePaginacion(uint32_t);
int agregarInstruccionesPaginacion ();
int buscarInstruccionesPaginacion ();
int quitarInstruccionesPaginacion ();

int agregarPatotaSegmentacion (pcb, uint32_t);
pcb* buscarPatotaSegmentacion (uint32_t);
int quitarPatotaSegmentacion(uint32_t);
int agregarTripulanteSegmentacion (tcb,uint32_t);
tcb* buscarTripulanteSegmentacion (uint32_t);
int quitarTripulanteSegmentacion (uint32_t);
int agregarInstruccionesSegmentacion (uint32_t, uint32_t, void*);
void* buscarInstruccionSegmentacion (uint32_t, int);
int quitarInstruccionesSegmentacion ();
int cambiarUbicacionSegmentacion (uint32_t, uint32_t, uint32_t);
int dumpMemoriaSegmentacion();

int buscarLugarFF(int);
int buscarLugarBF(int);
int insertarEnListaOcupancia (int, int, uint32_t);
int insertarEnListaPCB (uint32_t, uint32_t, uint32_t);
int insertarEnListaTCB (uint32_t, uint32_t, uint32_t);
ocupanciaPuntero buscarEnListaOcupancia(uint32_t);
nodoPCB* buscarEnListaPatotas(uint32_t);
nodoTCB* buscarEnListaTripulantes(uint32_t);
void quitarDeListaOcupancia (uint32_t);
void quitarDeListaPCB (uint32_t);
void quitarDeListaTCB (uint32_t);

int compactarMemoria(void);
int actualizarEstructuras(uint32_t, uint32_t);
int actualizarTripulantes(uint32_t, uint32_t, uint32_t);

void sigHandler(int);
void* sigHandlerMain(void*);

#endif
