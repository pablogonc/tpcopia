/*
 * discordiador.h
 *
 *  Created on: 17 jun. 2021
 *      Author: utnso
 */

#ifndef DISCORDIADOR_H_
#define DISCORDIADOR_H_

#include <commons/collections/list.h>
#include <time.h>
#include <commons/log.h>
#include <readline/readline.h>
#include <cliente.h>
#include <utilidades.h>
#include <stdint.h>
#include "utilidades_discordiador.h"
#include <math.h>
#include <commons/config.h>

enum socket {RAM,STORE};
t_log* logger;
char* algoritmoPlanificacion;
//estados
typedef enum //estados del tripu
{
	LLEGADA,
	LISTO,
	TRABAJANDO,
	BLOQUEADOEM,
	BLOQUEADOES,
	EXIT
}estado;

typedef struct {
	int estadoPlanificador;
	uint32_t x;
	uint32_t y;
} Sabotaje;


//config
int retardoCPU;
int duracionSabotaje;
//
int sabotaje;
int planificando;
Sabotaje sabotaje_curso;
//semaforos
sem_t* semLiberarTripulantes;
sem_t* semPlanificacion;
sem_t* semTripulantes;
sem_t* semMain;
pthread_mutex_t send;
//funciones del discordiador
void terminar_programa(int [2]);
void consola(int[2]);
void procesar_mensaje(char *,int[2]);
void bloquearTripulantesEM(int[]);
void desbloquearTripulantes(void);
void escuchar(int[]);
//mensajes discordiador
void iniciarPatota(char*,int);
void iniciarPlanificacion();
void listarTripulantes();
void pausarPlanificacion();
void obtenerBitacora();
void dumpMemoria(int);

int buscarTripulante(t_list * ,uint32_t );
void expulsar(uint32_t,int);
void expulsarTripulante(char*,int);
void mover_tripulante(t_list *,t_list *,uint32_t ,char *);
int getEstado(uint32_t);
Tripulante* getTripulante(t_list*,uint32_t);
void planificar();

uint32_t distancia(Tripulante*,Sabotaje);
#endif /* DISCORDIADOR_H_ */
