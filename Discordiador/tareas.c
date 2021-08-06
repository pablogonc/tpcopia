/*
 * tareas.c
 *
 *  Created on: 18 jun. 2021
 *      Author: utnso
 */

// tarea param;x;y;tiempo
// pos1=7 pos2=5 tripulante x=0 y=0

#include "tareas.h"

int moversex(uint32_t* pos,Tripulante * tripulante){
	uint32_t pos1 = *pos;

	int pasos = pos1 - tripulante->X; // lo cambie el tipo porque uint32_t no es negativo

	if(tripulante->X !=pos1){

		tripulante->X += (abs(pasos))/pasos;
		t_paquete* p =crear_paquete(RECIBIR_UBICACION);
		agregar_a_paquete(p,&tripulante->X,sizeof(uint32_t));
		agregar_a_paquete(p,&tripulante->Y,sizeof(uint32_t));
		enviar_paquete(p,tripulante->ramSocket);
		eliminar_paquete(p);
		return Tarea_Ejecutada;
	}

	return Tarea_Completada;
}
int moversey(uint32_t* pos,Tripulante * tripulante){
	uint32_t pos1 = *pos;

	int pasos = pos1 - tripulante->Y;  // lo cambie el tipo porque uint32_t no es negativo

	if(tripulante->Y !=pos1){

		tripulante->Y += (abs(pasos))/pasos;
		t_paquete* p =crear_paquete(RECIBIR_UBICACION);
		agregar_a_paquete(p,&tripulante->X,sizeof(uint32_t));
		agregar_a_paquete(p,&tripulante->Y,sizeof(uint32_t));
		enviar_paquete(p,tripulante->ramSocket);
		eliminar_paquete(p);

		return Tarea_Ejecutada;
	}

	return Tarea_Completada;
}


int operacionES(int* magicNumber,Tripulante* tripulante){
	int numero = *magicNumber ;
	int codOp= numero>>26;
	int parametro = (numero<<6)>>6;
	t_paquete* p =crear_paquete(codOp);
	agregar_a_paquete(p,&parametro,sizeof(int));
	enviar_paquete(p,tripulante->storeSocket);
	eliminar_paquete(p);

	//todo ¿bloquear hilo?

	 //recibir_operacion(tripulante->storeSocket);
	 //largo_paquete(tripulante->storeSocket);

	return Tarea_ES;
}


int esperar(int* tiempo,Tripulante * tripulante){

	if(--(*tiempo)){
		return Tarea_Ejecutada;
	}else{
		if(sabotaje == 1){
			return -5;
		}
		return Tarea_Completada;
	}
}

int invocarFSCK(int* sabotaje ,Tripulante * tripulante){
//todo
	t_paquete* paquete;
	paquete = crear_paquete(FSCK);
	enviar_paquete(paquete,tripulante->storeSocket);
	eliminar_paquete(paquete);
	if(recibir_operacion(tripulante->storeSocket) == FSCK){

		largo_paquete(tripulante->storeSocket);
		*sabotaje = -1;
		*(int*)recibir(tripulante->storeSocket);
		return Tarea_Completada;//

	}else{
		//algun error
		return -1; //todo ver
	}
}

void separarEnInstrucciones(char* tarea,t_list* instrucciones,Tripulante* tripulante){
//	list_destroy(instrucciones);/////////////////////////////////////////////////////////////////
//	instrucciones = list_create();
	// int parametroTarea,parametroX,parametroY,parametroTiempo;
	uint32_t parametroX;
	uint32_t parametroY;
	int parametroTiempo;
	int parametroTarea;

	char* token = strtok(tarea," ");
	int codTarea = reconocerTarea(token);
	if(codTarea == -1){ //nombreTarea;1;1;1
		token = strtok(tarea,";");
	    parametroX = atoi(strtok(NULL,";"));
	    parametroY = atoi(strtok(NULL,";"));
	    parametroTiempo = atoi(strtok(NULL,";"));
	}else{
		parametroTarea = atoi(strtok(NULL,";"));
		parametroX = atoi(strtok(NULL,";"));
		parametroY = atoi(strtok(NULL,";"));
		parametroTiempo = atoi(strtok(NULL,";"));
	}

    Instruccion* instruccion = malloc(sizeof(Instruccion));
    instruccion->instrucion=moversex;
    instruccion->parametro=parametroX;
    list_add(instrucciones,instruccion);

    //free(instruccion);
    instruccion = malloc(sizeof(Instruccion));
	instruccion->instrucion=moversey;
	instruccion->parametro=parametroY;
	list_add(instrucciones,instruccion);
	//free(instruccion);

	t_paquete* paquete;

	if(codTarea!=-1){

		int numeromagico=codTarea;
		numeromagico = numeromagico <<26;
		numeromagico = numeromagico | parametroTarea;
		instruccion = malloc(sizeof(Instruccion));
		instruccion->instrucion=operacionES;
		instruccion->parametro=numeromagico;
		list_add(instrucciones,instruccion);

		instruccion = malloc(sizeof(Instruccion));
		instruccion->instrucion=esperar;
		instruccion->parametro=parametroTiempo;
		list_add(instrucciones,instruccion);

	}else{
		instruccion = malloc(sizeof(Instruccion));
		instruccion->instrucion=esperar;
		instruccion->parametro=parametroTiempo;
		list_add(instrucciones,instruccion);
	}
	//free(instruccion);
}

int reconocerTarea(char* tarea){
	char * nombreTarea;
	nombreTarea = tarea;

    if (strcmp(nombreTarea,"GENERAR_OXIGENO")==0)
        return GENERAR_OXIGENO;
    if (strcmp(nombreTarea,"CONSUMIR_OXIGENO")==0)
            return CONSUMIR_OXIGENO;
    if (strcmp(nombreTarea,"GENERAR_COMIDA")==0)
            return GENERAR_COMIDA;
    if (strcmp(nombreTarea,"CONSUMIR_COMIDA")==0)
            return CONSUMIR_COMIDA;
    if (strcmp(nombreTarea,"GENERAR_BASURA")==0)
            return GENERAR_BASURA;
    if (strcmp(nombreTarea,"DESCARTAR_BASURA")==0)
            return DESCARTAR_BASURA;
    return -1;
}
