/*
 * utilidades_discordiador.h
 *
 *  Created on: 28 may. 2021
 *      Author: utnso
 */

#ifndef UTILIDADES_DISCORDIADOR_H_
#define UTILIDADES_DISCORDIADOR_H_

#include <pthread.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <stdint.h>
#include <semaphore.h>
void menu();
int operacion(char *);
int conectar_con(char*);
void mostrar(t_list*,char*);
char * nombreOperacionES(int);
t_list * lista_patotas;
t_list * lista_tripulantes;

/*
typedef struct{
	int tid;
	int pid;
	char* estado;
}Tripulante;*/

typedef struct {
    uint32_t tid;
    char* estado;
    uint32_t X;
    uint32_t Y;
    uint32_t proximaInstruccion;
    uint32_t pid;
    int tiempo;
    int quantum;
    int ramSocket;
    int storeSocket;
    int designado; //tiene que atender el saboaje
    sem_t* sem;
    t_list* instrucciones;
}Tripulante;

/*typedef struct {
	Tripulante* tripulante;
};*/

//colas
t_list * Llegada;
t_list * Listo;
t_list * Trabajando;
t_list * BloqueadoEM;
t_list * BloqueadoES;
t_list * Finalizado;

#endif /* UTILIDADES_DISCORDIADOR_H_ */
