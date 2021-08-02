#include <string.h>
#include <stdio.h>
#include <commons/temporal.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <signal.h>
#include "RAM_utilidades.h"

void reservarMemoria (uint32_t tamanioMemoria){
    memoriaPrincipal = calloc(1, tamanioMemoria);
    TAMANIOMEMORIA = tamanioMemoria;
}

void configurarMemoriaPaginacion(uint32_t tamanioPagina,uint32_t tamanioSwap,string pathSwap,string algoritmoReemplazo){
    
}

void configurarMemoriaSegmentacion(string criterioSeleccion){
    if(strncmp(criterioSeleccion,"FF",sizeof(char)*2) == 0){
    	criterioDeSegmentacion = &buscarLugarFF;
    }else{
    	if(strncmp(criterioSeleccion,"BF",sizeof(char)*2) == 0){
    		criterioDeSegmentacion = &buscarLugarBF;
    	}else{
    		log_warning(logger,"Criterio de seleccion invalido");
    	}
    }
    agregarPatota = &agregarPatotaSegmentacion;
    buscarPatota = &buscarPatotaSegmentacion;
    quitarPatota = &quitarPatotaSegmentacion;
    agregarTripulante = &agregarTripulanteSegmentacion;
    buscarTripulante = &buscarTripulanteSegmentacion;
    quitarTripulante = &quitarTripulanteSegmentacion;
    agregarInstrucciones = &agregarInstruccionesSegmentacion;
    buscarInstruccion = &buscarInstruccionSegmentacion;
    quitarInstrucciones = &quitarInstruccionesSegmentacion;
    cambiarUbicacion = &cambiarUbicacionSegmentacion;
    dumpMemoria = &dumpMemoriaSegmentacion;
}

void initSemaphores(){
	socket1 = (sem_t*)malloc(sizeof(sem_t));
	sem_init(socket1,1,1);
	memoria = (sem_t*)malloc(sizeof(sem_t));
	sem_init(memoria,1,1);
	mapa = (sem_t*)malloc(sizeof(sem_t));
	sem_init(mapa,1,1);
	loguear = (sem_t*)malloc(sizeof(sem_t));
	sem_init(loguear,1,1);
	colaTripulantes = (sem_t*)malloc(sizeof(sem_t));
	sem_init(colaTripulantes,1,0);
	signalThread = (sem_t*)malloc(sizeof(sem_t));
	sem_init(signalThread,1,0);
}

void initGlobales(){
	LISTAOCUPANCIA  = NULL;
	LISTAPATOTAS  = NULL;
	LISTATRIPULANTES = NULL;
	acumuladorTripulantes  = 0;
	acumuladorCodigoUnico  = 0;
	acumuladorPatotas = 0;
}
//////////////////////////////////////////////////////////////

uint32_t nuevoTripulante(uint32_t patotaID, uint32_t posicionX, uint32_t posicionY, uint32_t tid){
	tcb unTCB;
	uint32_t tripulanteID = tid;
	pcb* patotaEnMemoria = buscarPatota(patotaID);

	unTCB.tid = tripulanteID;
	unTCB.estado = 'N';
	unTCB.posicionX = posicionX;
	unTCB.posicionY = posicionY;
	unTCB.proximaInstruccion = 0;
	unTCB.punteroPCB = (void*) patotaEnMemoria - memoriaPrincipal;

    return agregarTripulante(unTCB,tripulanteID);;
}

int avanzarTarea(tcb* tripulante){
    tripulante -> proximaInstruccion += 1;
    return 0;
}

///////////////////////////////////////////////////

int agregarPatotaSegmentacion (pcb patota, uint32_t id) {
	pcb* auxiliar2 = (pcb*)malloc(sizeof(pcb));
	*auxiliar2 = patota;
	int posicion = criterioDeSegmentacion(sizeof(pcb));
	if(posicion >= 0 && buscarEnListaPatotas(id) == NULL){
		memcpy(memoriaPrincipal + posicion,(void*) auxiliar2,sizeof(pcb));
		uint32_t codigoUnico = ++acumuladorCodigoUnico;
		insertarEnListaPCB(posicion,id, codigoUnico);
		insertarEnListaOcupancia(posicion,sizeof(pcb),codigoUnico);
		return 0;
	}
	return 1;
}

pcb* buscarPatotaSegmentacion (uint32_t pid){
	nodoPCB* patota = buscarEnListaPatotas(pid);
	if(patota == NULL){
		return NULL;
	}
	return (pcb*) (memoriaPrincipal + (patota -> pcbEnMemoria));
}

int quitarPatotaSegmentacion(uint32_t pid){
	nodoPCB* patota = buscarEnListaPatotas(pid);
	if(patota != NULL){
		quitarDeListaOcupancia(patota -> codigoUnicoPcb);
		quitarDeListaPCB(patota -> codigoUnicoPcb);
		quitarInstruccionesSegmentacion(patota -> codigoUnicoTareas);
		return 0;
	}else{
		return 1;
	}

}

int agregarTripulanteSegmentacion (tcb tripulante, uint32_t id) {
		tcb* auxiliar2 = (tcb*)malloc(sizeof(tcb));
		*auxiliar2 = tripulante;

		int posicion = criterioDeSegmentacion(sizeof(tcb));

		if(posicion >= 0 && buscarEnListaTripulantes(id) == NULL){
			memcpy(memoriaPrincipal + posicion,(void*) auxiliar2,sizeof(tcb));
			uint32_t codigoUnico = ++acumuladorCodigoUnico;
			insertarEnListaTCB(posicion,id, codigoUnico);
			insertarEnListaOcupancia(posicion,sizeof(tcb),codigoUnico);
			return 0;
		}
		return 1;
}

tcb* buscarTripulanteSegmentacion (uint32_t tid){
	nodoTCB* tripulante = buscarEnListaTripulantes(tid);
		if(tripulante == NULL){
			return NULL;
		}
		return (tcb*) (memoriaPrincipal + (tripulante -> tcbEnMemoria));
}

int quitarTripulanteSegmentacion (uint32_t tid) {
	nodoTCB* tripulante = buscarEnListaTripulantes(tid);
	if(tripulante != NULL){
		quitarDeListaOcupancia(tripulante -> codigoUnico);
		quitarDeListaTCB(tid);
		return 0;
	}
	return 1;
}

int agregarInstruccionesSegmentacion (uint32_t pid, uint32_t tamanio, void* instrucciones){
	nodoPCB* nodoPatota = buscarEnListaPatotas(pid);
	pcb* patota = buscarPatotaSegmentacion(pid);
	int posicion = criterioDeSegmentacion(tamanio);

	if(posicion >= 0 && nodoPatota != NULL){
		memcpy(memoriaPrincipal + posicion, instrucciones, tamanio);
		uint32_t codigoUnico = ++acumuladorCodigoUnico;
		patota -> tareas = posicion;
		insertarEnListaOcupancia(posicion, tamanio, codigoUnico);
		nodoPatota -> codigoUnicoTareas = codigoUnico;
		return 0;
	}
	return 1;
}

void* buscarInstruccionSegmentacion (uint32_t tid, int posicion){
	int acumulador;
	void* ip;
	uint32_t posTareas;
	tcb* tcbEnMemoria = buscarTripulanteSegmentacion(tid);

	pcb* pcbEnMemoria = memoriaPrincipal + (tcbEnMemoria -> punteroPCB);

	posTareas = pcbEnMemoria -> tareas;
	ip = memoriaPrincipal + posTareas;
	for(acumulador = 0; acumulador < posicion; acumulador++){
		uint32_t* sizeTarea = (uint32_t*) ip;
		if(*sizeTarea == 0){
			return ip;
		}
		ip += sizeof(uint32_t) + *sizeTarea;
	}
	return ip;

/*
	int acumulador;
	void* ip;
	uint32_t posTareas;
	tcb* tcbEnMemoria = buscarTripulanteSegmentacion(tid);

	pcb* pcbEnMemoria = memoriaPrincipal + (tcbEnMemoria -> punteroPCB);

	posTareas = pcbEnMemoria -> tareas;
	ip = memoriaPrincipal + posTareas;

	----------------
	for(acumulador = 0; acumulador < posicion; acumulador++){
		char* auxiliar = ip;
		uint32_t sizeTarea = (uint32_t) strlen(auxiliar);
		if(*(auxiliar + sizeTarea -1) == EOF){
			return NULL;
		}
		ip += sizeTarea + 1;
	}
	return ip;
	-----------------
*/
}

int quitarInstruccionesSegmentacion (uint32_t codigoUnicoTareas){
	if(buscarEnListaOcupancia(codigoUnicoTareas) != NULL){
		quitarDeListaOcupancia(codigoUnicoTareas);
		return 0;
	}
	return 1;
}

int cambiarUbicacionSegmentacion (uint32_t tid, uint32_t x, uint32_t y) {
	tcb* tripulante = buscarTripulanteSegmentacion(tid);
	if(tripulante != NULL){
		tripulante -> posicionX = x;
		tripulante -> posicionY = y;
		return 0;
	}
	return 1;
}

//TODO
int dumpMemoriaSegmentacion(){
	char* time = temporal_get_string_time("%H:%M:%S:%MS");
	char* date = temporal_get_string_time("%d/%m/%y %H:%M:%S");
	char* dumpName = (char*)malloc(sizeof(char)*22);
	memcpy(dumpName,"Dump_",sizeof(char)*5);
	memcpy(dumpName+5, time,sizeof(char)*12);
	memcpy(dumpName+17,".dmp",sizeof(char)*5);
	if(DEBUG_MODE){
		printf("Realizando dump de la memoria en %s \n", dumpName);
	}
	FILE* f = fopen(dumpName,"w+");

	fprintf(f,"Dump: ");
	fprintf(f,"%s",date);
	fprintf(f,"\n");

	nodoPCB* patotas = LISTAPATOTAS;
	nodoOcupanciaMemoria* ocupancia;
	uint32_t pid;
	while(patotas != NULL){
		pid = patotas->id;
		ocupancia = buscarEnListaOcupancia(patotas->codigoUnicoPcb);
		fprintf(f,"Proceso: %-3d  ", (int) pid);
		fprintf(f,"Segmento: 1  ");
		fprintf(f,"Inicio: 0x%-7x  ", (int) ocupancia->inicio);
		fprintf(f,"Tamaño: %4db", (int) (ocupancia->final - ocupancia->inicio));
		fprintf(f,"\n");
		ocupancia = buscarEnListaOcupancia(patotas->codigoUnicoTareas);
		fprintf(f,"Proceso: %-3d  ", (int) pid);
		fprintf(f,"Segmento: 2  ");
		fprintf(f,"Inicio: 0x%-7x  ", (int) ocupancia->inicio);
		fprintf(f,"Tamaño: %4db", (int) (ocupancia->final - ocupancia->inicio));
		fprintf(f,"\n");
		patotas = patotas ->sig;
	}
	fclose(f);
	return 0;
}

////////////////////////////////////////////////////////////

int buscarLugarFF(int tamanio){
    if(LISTAOCUPANCIA == NULL) {
        return 0;
    }
    ocupanciaPuntero auxiliar = LISTAOCUPANCIA;
    int posActual = 0;
    while(auxiliar != NULL) {
        if(auxiliar->inicio - posActual >= tamanio) {
            return posActual;
        }
        posActual = auxiliar -> final + 1;
        auxiliar = auxiliar -> sig;
    }
    if(TAMANIOMEMORIA - posActual >= tamanio) {
        return posActual;
    }

    return -1;
}

int buscarLugarBF(int tamanio){
    if(LISTAOCUPANCIA == NULL) {
        return 0;
    }
    ocupanciaPuntero auxiliar = LISTAOCUPANCIA;
    int posActual = 0;
    int bestPos = 0;
    int bestSize = TAMANIOMEMORIA;

    while(auxiliar != NULL) {
        if(auxiliar->inicio - posActual >= tamanio && auxiliar->inicio - posActual < bestSize) {
            bestPos = posActual;
            bestSize = auxiliar->inicio - posActual;
        }
        posActual = auxiliar -> final + 1;
        auxiliar = auxiliar -> sig;
    }

    if(TAMANIOMEMORIA - posActual >= tamanio && TAMANIOMEMORIA - posActual < bestSize) {
        return posActual;
    }

    if(bestSize != TAMANIOMEMORIA) {
        return bestPos;
    }

    return -1;
}

int insertarEnListaOcupancia (int inicio, int tamanio, uint32_t codigoUnico){
	if(buscarEnListaOcupancia(codigoUnico) != NULL){
		return 1;
	}

    nodoOcupanciaMemoria* nodo = malloc(sizeof(nodoOcupanciaMemoria));
    nodo -> inicio = inicio;
    nodo -> final = inicio + tamanio - 1;
    nodo -> codigoUnico = codigoUnico;
    nodo -> sig = NULL;

    if(LISTAOCUPANCIA == NULL) {
    	LISTAOCUPANCIA = nodo;
        return 0;
    }

    if(LISTAOCUPANCIA -> inicio > inicio){
    	nodo->sig = LISTAOCUPANCIA;
    	LISTAOCUPANCIA = nodo;
    }else{
    	ocupanciaPuntero reco = LISTAOCUPANCIA;
    	ocupanciaPuntero atras = LISTAOCUPANCIA;
    	while(inicio >= reco -> inicio && reco -> sig != NULL){
    		atras = reco;
    		reco = reco -> sig;
    	}
    	if(reco -> sig == NULL){
    		reco->sig = nodo;
    	}else{
    		nodo -> sig = reco;
    		atras -> sig = nodo;
    	}
    }
    return 0;
}

int insertarEnListaPCB (uint32_t pcbEnMemoria, uint32_t id, uint32_t codigoUnico){
	if(buscarEnListaPatotas(id) != NULL){
		return 1;
	}

    nodoPCB* nodo = malloc(sizeof(nodoPCB));
    nodo -> pcbEnMemoria = pcbEnMemoria;
    nodo -> id = id;
    nodo -> codigoUnicoPcb = codigoUnico;
    nodo -> sig = NULL;

    if(LISTAPATOTAS == NULL) {
    	LISTAPATOTAS = nodo;
        return 0;
    }


    if(LISTAPATOTAS -> id > id){
    	nodo->sig = LISTAPATOTAS;
    	LISTAPATOTAS = nodo;
    }else{
    	nodoPCB* reco = LISTAPATOTAS;
    	nodoPCB* atras = LISTAPATOTAS;
    	while(id >= reco -> id && reco -> sig != NULL){
    		atras = reco;
    		reco = reco -> sig;
    	}
    	if(reco -> sig == NULL){
    		reco->sig = nodo;
    	}else{
    		nodo -> sig = reco;
    		atras -> sig = nodo;
    	}
    }
    return 0;
}

int insertarEnListaTCB (uint32_t tcbEnMemoria, uint32_t id, uint32_t codigoUnico){
	if(buscarEnListaTripulantes(id) != NULL){
		return 1;
	}
    nodoTCB* nodo = malloc(sizeof(nodoTCB));
    nodo -> tcbEnMemoria = tcbEnMemoria;
    nodo -> id = id;
    nodo -> codigoUnico = codigoUnico;
    nodo -> sig = NULL;

    if(LISTATRIPULANTES == NULL) {
    	LISTATRIPULANTES = nodo;
        return 0;
    }


    if(LISTATRIPULANTES -> id > id){
    	nodo->sig = LISTATRIPULANTES;
    	LISTATRIPULANTES = nodo;
    }else{
    	nodoTCB* reco = LISTATRIPULANTES;
    	nodoTCB* atras = LISTATRIPULANTES;
    	while(id >= reco -> id && reco -> sig != NULL){
    		atras = reco;
    		reco = reco -> sig;
    	}
    	if(reco -> sig == NULL){
    		reco->sig = nodo;
    	}else{
    		nodo -> sig = reco;
    		atras -> sig = nodo;
    	}
    }
    return 0;
}

ocupanciaPuntero buscarEnListaOcupancia(uint32_t id) {
    ocupanciaPuntero auxiliar = LISTAOCUPANCIA;
    while(auxiliar != NULL) {
        if(auxiliar -> codigoUnico == id) {
            return auxiliar;
        }
        auxiliar = auxiliar -> sig;
    }
    return NULL;
}

nodoPCB* buscarEnListaPatotas(uint32_t pid) {
    nodoPCB* auxiliar = LISTAPATOTAS;
    while(auxiliar != NULL) {
        if(auxiliar -> id == pid) {
            return auxiliar;
        }
        auxiliar = auxiliar -> sig;
    }
    return NULL;
}

nodoTCB* buscarEnListaTripulantes(uint32_t tid) {
    nodoTCB* auxiliar = LISTATRIPULANTES;
    while(auxiliar != NULL) {
        if(auxiliar -> id == tid) {
            return auxiliar;
        }
        auxiliar = auxiliar -> sig;
    }
    return NULL;
}

void quitarDeListaOcupancia (uint32_t id){
    if(LISTAOCUPANCIA == NULL) {
        return;
    }

    if(LISTAOCUPANCIA -> codigoUnico == id){
    	ocupanciaPuntero auxiliar1 = LISTAOCUPANCIA;
    	LISTAOCUPANCIA = LISTAOCUPANCIA -> sig;
    	free(auxiliar1);
    }else{
    	ocupanciaPuntero reco = LISTAOCUPANCIA;
    	ocupanciaPuntero atras = LISTAOCUPANCIA;
    	while(id != reco -> codigoUnico && reco -> sig != NULL){
    		atras = reco;
    		reco = reco -> sig;
    	}
    	if(id == reco -> codigoUnico){
    	    atras -> sig = reco -> sig;
    	    free(reco);
    	    return;
    	}
    	if(reco -> sig == NULL){
    		return;
    	}
    }
}

void quitarDeListaPCB (uint32_t id){
    if(LISTAPATOTAS == NULL) {
        return;
    }

    if(LISTAPATOTAS -> id == id){
    	nodoPCB* auxiliar1 = LISTAPATOTAS;
    	LISTAPATOTAS = LISTAPATOTAS -> sig;
    	free(auxiliar1);
    }else{
    	nodoPCB* reco = LISTAPATOTAS;
    	nodoPCB* atras = LISTAPATOTAS;
    	while(id != reco -> id && reco -> sig != NULL){
    		atras = reco;
    		reco = reco -> sig;
    	}
    	if(id == reco -> id){
    	    atras -> sig = reco -> sig;
    	    free(reco);
    	    return;
    	}
    	if(reco -> sig == NULL){
    	    return;
    	}
    }
}

void quitarDeListaTCB (uint32_t id){
    if(LISTATRIPULANTES == NULL) {
        return;
    }

    if(LISTATRIPULANTES -> id == id){
    	nodoTCB* auxiliar1 = LISTATRIPULANTES;
    	LISTATRIPULANTES = LISTATRIPULANTES -> sig;
    	free(auxiliar1);
    }else{
    	nodoTCB* reco = LISTATRIPULANTES;
    	nodoTCB* atras = LISTATRIPULANTES;
    	while(id != reco -> id && reco -> sig != NULL){
    		atras = reco;
    		reco = reco -> sig;
    	}
    	if(id == reco -> id){
    	    atras -> sig = reco -> sig;
    	    free(reco);
    	    return;
    	}
    	if(reco -> sig == NULL){
    	    return;
    	}
    }
}

/*
void imprimirLista(ocupanciaPuntero cabeza){
    while(cabeza != NULL){ //Mientras cabeza no sea NULL
        printf("%4d",cabeza->valor); //Imprimimos el valor del nodo
        cabeza = cabeza->sig; //Pasamos al siguiente nodo
    }
}
*/

void borrarLista(ocupanciaPuntero *cabeza){
    ocupanciaPuntero actual; //Puntero auxiliar para eliminar correctamente la lista
  
    while(*cabeza != NULL){ //Mientras cabeza no sea NULL
        actual = *cabeza; //Actual toma el valor de cabeza
        *cabeza = (*cabeza)->sig; //Cabeza avanza 1 posicion en la lista
        free(actual); //Se libera la memoria de la posicion de Actual (el primer nodo), y cabeza queda apuntando al que ahora es el primero
    }
}
////////////////////////////////////////////////////////

int compactarMemoria(){
	ocupanciaPuntero segmento = LISTAOCUPANCIA;
	int acumulador = 0;
	int inicio;
	int fin;
	while(segmento != NULL){
		if(segmento->inicio > acumulador){
			inicio = segmento->inicio;
			fin = segmento->final;
			memcpy(memoriaPrincipal + acumulador, memoriaPrincipal + inicio, fin + 1);
			segmento->inicio = acumulador;
			segmento->final = acumulador + fin - inicio;
			actualizarEstructuras(segmento->codigoUnico, acumulador);
		}
		acumulador += (segmento -> final) + 1;
		segmento = segmento -> sig;
	}
	return 0;
}

int actualizarEstructuras(uint32_t uid, uint32_t nuevoInicio){
	nodoPCB* nodopcb = LISTAPATOTAS;
	nodoTCB* nodotcb = LISTATRIPULANTES;
	pcb* pcbMemoria;
	uint32_t anterior;

	while(nodopcb != NULL){
		if(nodopcb -> codigoUnicoPcb == uid){
			anterior = nodopcb -> pcbEnMemoria;
			nodopcb -> pcbEnMemoria = nuevoInicio;
			actualizarTripulantes(nodopcb -> id, nuevoInicio, anterior);
			return 0;
		}
		if(nodopcb -> codigoUnicoTareas == uid){
			pcbMemoria = memoriaPrincipal + (nodopcb -> pcbEnMemoria);
			pcbMemoria -> tareas = nuevoInicio;
			return 0;
		}
		nodopcb = nodopcb -> sig;
	}
	while(nodotcb != NULL){
		if(nodotcb -> codigoUnico == uid){
			nodotcb -> tcbEnMemoria = nuevoInicio;
			return 0;
		}
		nodotcb = nodotcb -> sig;
	}

	return 1;
}

int actualizarTripulantes(uint32_t pid, uint32_t nuevoInicio, uint32_t anterior){
	nodoTCB* nodotcb = LISTATRIPULANTES;
	tcb* tcbMemoria;
	while(nodotcb != NULL){
		tcbMemoria = memoriaPrincipal + (nodotcb -> tcbEnMemoria);
			if(tcbMemoria -> punteroPCB == anterior){
				tcbMemoria -> punteroPCB = nuevoInicio;
			}
			nodotcb = nodotcb -> sig;
		}
	return 0;
}
////////////////////////////////////////////////////////

void sigHandler(int n){
	switch (n){
	case SIGUSR1:
		sem_wait(loguear);
		log_info(logger,"Realizando un Dump de la memoria.");
		sem_post(loguear);
		sem_wait(memoria);
		dumpMemoria();
		sem_post(memoria);
		break;
	case SIGUSR2:
		sem_wait(loguear);
		log_info(logger, "Compactando segmentos en memoria.");
		sem_post(loguear);
		sem_wait(memoria);
		compactarMemoria();
		sem_post(memoria);
		break;
	}
}

void* sigHandlerMain(void* arg){
	signal(SIGUSR1,&sigHandler);
	signal(SIGUSR2,&sigHandler);
	sem_wait(signalThread);
	return 0;
}
