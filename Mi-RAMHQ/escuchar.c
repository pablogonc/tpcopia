#include <commons/log.h>
#include "../utilidades/utilidades.h"
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>
#include "escuchar.h"
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
t_log* logger;

int escuchar(int cliente){
	sem_post(socket1);
	uint32_t* numericalMsg;
	pcb* patota;
	pcb unaPatota;
	tcb* tripulante;
	t_paquete* paquete;
	uint32_t tripulanteID = -1;
	uint32_t* posicionX;
	uint32_t* posicionY;
	uint32_t pSize;
	void* content;

	uint32_t pid, resInit;

	ocupanciaPuntero ocupancia;
	nodoTCB* nodoTCB;
	pcb* unPcb;
	tcb* tcb;
	char* auxiliar;
	ocupanciaPuntero ocAux;


	while(1){


		int cod_op = recibir_operacion(cliente);


		switch(cod_op){
		case INICIAR_PATOTA:
			pSize = largo_paquete(cliente);

			//////////////////////////////////numericalMsg = recibir(cliente);
			pid = ++acumuladorPatotas;
			char* content = recibir(cliente);

			sem_wait(loguear);
			log_info(logger, "Iniciando la patota '%lu'", (long unsigned int) pid);
			sem_post(loguear);

			unaPatota.pid = pid;

			sem_wait(memoria);
			agregarPatota(unaPatota,pid);
			sem_post(memoria);

			sem_wait(memoria);
			agregarInstrucciones(pid, pSize-4, content);
			sem_post(memoria);

			paquete = crear_paquete(INICIAR_PATOTA);
			agregar_a_paquete(paquete,&pid,sizeof(uint32_t));
			enviar_paquete(paquete,cliente);
			eliminar_paquete(paquete);

			if(DEBUG_MODE){
				nodoPCB* nodoPCB = buscarEnListaPatotas(pid);
				printf("PCB EN MEMORIA %d\n", nodoPCB -> pcbEnMemoria);
				printf("PID %d\n", nodoPCB -> id);
				printf("Unique PCB-ID %d\n", nodoPCB -> codigoUnicoPcb);
				printf("Unique TAREAS-ID %d\n", nodoPCB -> codigoUnicoTareas);
				printf("\n");
				ocupancia = buscarEnListaOcupancia(nodoPCB -> codigoUnicoPcb);
				printf("INICIO %d\n", ocupancia -> inicio);
				printf("ID %d\n", ocupancia -> codigoUnico);
				printf("FINAL %d\n", ocupancia -> final);
				printf("\n");
				unPcb = memoriaPrincipal + nodoPCB -> pcbEnMemoria;
				auxiliar = memoriaPrincipal + unPcb->tareas;
				printf("EL TAMAÑO DEL PAQUETE ES: %d\n", (int) pSize);
				printf("LA TAREA ES: %s\n", auxiliar);
				ocupancia = buscarEnListaOcupancia(nodoPCB -> codigoUnicoTareas);
				printf("INICIO %d\n", ocupancia -> inicio);
				printf("ID %d\n", ocupancia -> codigoUnico);
				printf("FINAL %d\n", ocupancia -> final);
				printf("\n\n");
			}
			free(content);
			break;

		case INICIAR_TRIPULANTE:
			largo_paquete(cliente);
			uint32_t* pid =(uint32_t*)recibir(cliente);
			posicionX =(uint32_t*)recibir(cliente);
			posicionY =(uint32_t*)recibir(cliente);
			uint32_t* tid =(uint32_t*)recibir(cliente);
			tripulanteID = *tid;

			TRYINIT:
			sem_wait(memoria);
			resInit = nuevoTripulante(*pid, *posicionX, *posicionY, *tid);
			sem_post(memoria);

			if(resInit){
				sem_wait(colaTripulantes);
				goto TRYINIT;
			}

			sem_wait(loguear);
			log_info(logger, "Iniciando tripulante '%d' perteneciente a la patota %d.",(int) tripulanteID,(int) *pid);
			sem_post(loguear);

			if(!DEBUG_MODE){
				sem_wait(mapa);
				agregarPersonajeMapa((char) ((int) tripulanteID + 65), *posicionX, *posicionY);
				sem_post(mapa);
			}
/*
			paquete = crear_paquete(INICIAR_TRIPULANTE);
			agregar_a_paquete(paquete,&tripulanteID,sizeof(uint32_t));
			enviar_paquete(paquete,cliente);
			eliminar_paquete(paquete);
*/
			if(DEBUG_MODE){
				nodoTCB = buscarEnListaTripulantes(tripulanteID);
				printf("TCB EN MEMORIA %d\n", nodoTCB -> tcbEnMemoria);
				printf("TRIP ID %d\n", nodoTCB -> id);
				printf("Unique ID %d\n", nodoTCB -> codigoUnico);
				printf("\n");
				ocupancia = buscarEnListaOcupancia(nodoTCB -> codigoUnico);
				printf("INICIO %d\n", ocupancia -> inicio);
				printf("ID %d\n", ocupancia -> codigoUnico);
				printf("FINAL %d\n", ocupancia -> final);
				printf("\n");
				tcb = memoriaPrincipal + ocupancia -> inicio;
				printf("TID %d\n", tcb -> tid);
				printf("ESTADO %c\n", tcb -> estado);
				printf("POS-X %d\n", tcb -> posicionX);
				printf("POS-Y %d\n", tcb -> posicionY);
				printf("POS-PCB %d\n", tcb -> punteroPCB);
				printf("\n\n");
			}

			free(pid);
			free(tid);
			break;

		case RECIBIR_UBICACION:

			largo_paquete(cliente);
			posicionX =(uint32_t*)recibir(cliente);
			posicionY =(uint32_t*)recibir(cliente);

			sem_wait(loguear);
			log_info(logger, "Moviendo al tripulante '%lu' al la posicion %lu | %lu", (long unsigned int) tripulanteID, (long unsigned int) (*posicionX), (long unsigned int) (*posicionY));
			sem_post(loguear);

			sem_wait(memoria);
			cambiarUbicacion(tripulanteID, *posicionX, *posicionY);
			sem_post(memoria);

			if(!DEBUG_MODE){
				sem_wait(mapa);
				moverPersonajeMapa((char) ((int) tripulanteID + 65), *posicionX, *posicionY);
				sem_post(mapa);
			}
			if(DEBUG_MODE){
				nodoTCB = buscarEnListaTripulantes(tripulanteID);
				ocupancia = buscarEnListaOcupancia(nodoTCB -> codigoUnico);
				tcb = memoriaPrincipal + ocupancia -> inicio;
				printf("TID %d\n", tcb -> tid);
				printf("ESTADO %c\n", tcb -> estado);
				printf("NUEVA POS-X %d\n", tcb -> posicionX);
				printf("NUEVA POS-Y %d\n", tcb -> posicionY);
				printf("POS-PCB %d\n", tcb -> punteroPCB);
				printf("\n\n");
			}
			free(posicionX);
			free(posicionY);
			break;

		case ENVIAR_PROXIMA_TAREA:
			largo_paquete(cliente);
			sem_wait(memoria);
			tripulante = buscarTripulante(tripulanteID);
			uint32_t proximaInstruccion = tripulante -> proximaInstruccion;

			char* tarea =
			//uint32_t* sizeTarea =
					buscarInstruccion(tripulanteID, proximaInstruccion);

			if(tarea != NULL){
			//if (*sizeTarea > 0 ){
				//char* tarea = (char*) ((void*) sizeTarea + sizeof(uint32_t));
				sem_wait(loguear);
				log_info(logger, "Enviando proxima tarea a '%d': tarea: %s.", (int) tripulanteID,tarea);
				sem_post(loguear);

				paquete = crear_paquete(RECIBIR_TAREAS);
				int sizeTarea = strlen(tarea);
				agregar_a_paquete(paquete,tarea,sizeTarea + 1);
				//agregar_a_paquete(paquete,tarea,(int) *sizeTarea);
				enviar_paquete(paquete,cliente);

				sem_post(memoria);

				eliminar_paquete(paquete);

				sem_wait(memoria);
				avanzarTarea(tripulante);
				sem_post(memoria);
			}else{
				sem_wait(loguear);
				log_info(logger, "El tripulante '%d' no tiene mas tareas.", (int) tripulanteID);
				sem_post(loguear);
				sem_post(memoria);
				paquete = crear_paquete(NOHAYTAREAS);
				enviar_paquete(paquete,cliente);
			}
			//printf("PROXIMA INSTRUCCION = %d\n", (int) tripulante -> proximaInstruccion);
			break;

		case EXPULSAR_TRIPULANTE:
			largo_paquete(cliente);
			numericalMsg=(uint32_t*)recibir(cliente);

			sem_wait(loguear);
			log_info(logger, "Expulsando al tripulante '%lu'", *numericalMsg);
			sem_post(loguear);

			sem_wait(memoria);
			quitarTripulante(*numericalMsg);
			sem_post(memoria);

			if(!DEBUG_MODE){
				sem_wait(mapa);
				eliminarPersonajeMapa((char) ((int) *numericalMsg + 65));
				sem_post(mapa);
			}

			sem_post(colaTripulantes);

			if(DEBUG_MODE){
				nodoTCB = buscarEnListaTripulantes(tripulanteID);
				if(nodoTCB == NULL) printf("NODO TCB ES NULL\n");
				ocupancia = buscarEnListaOcupancia(3);
				if(ocupancia == NULL) printf("OCUPANCIA ES NULL\n\n");
				ocAux = LISTAOCUPANCIA;
				int acc =0;
				while(ocAux != NULL){
					printf("NODO %d -- UID %d\n", acc, ocAux -> codigoUnico);
					printf("INICIO: %d\n", ocAux -> inicio);
					printf("FINAL: %d\n", ocAux -> final);
					acc++;
					ocAux = ocAux -> sig;
				}
				printf("\n\n");
			}
			free(numericalMsg);
			break;
		case DUMP_MEMORIA:
			largo_paquete(cliente);
			sem_wait(loguear);
			log_info(logger,"Realizando un DUMP de la memoria.");
			sem_post(loguear);
			pthread_kill(hiloSignals, SIGUSR1);

			break;
		case -1:
			sem_wait(loguear);
			log_warning(logger,"Un cliente se desconecto. Terminando conexion con el mismo.");
			sem_post(loguear);
			return 0;
			break;

		default:
			sem_wait(loguear);
			log_warning(logger,"Operacion desconocida del cliente '%d': %d\n.", tripulanteID, cod_op);
			sem_post(loguear);
			break;
		}
	}

}
