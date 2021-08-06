#include "utilidades_discordiador.h"
#include <commons/config.h>
#include <utilidades.h>
#include <stdio.h>
#include <string.h>
#include <cliente.h>
#include "tareas.h"
void menu(){
	printf("====================== mandar ====================== \n");
	printf("INICIAR_PATOTA 5 /home/utnso/tareas/tareasPatota5.txt 1|1 3|4  \n");
	printf("LISTAR_TRIPULANTES\n");
	printf("EXPULSAR_TRIPULANTE 5\n");
	printf("INICIAR_PLANIFICACION\n");
	printf("PAUSAR_PLANIFICACION\n");
	printf("OBTENER_BITACORA 1\n");
	printf("DUMP_MEMORIA\n");
	printf("exit\n");
}

int operacion(char * leido){
	if(leido == NULL){
		return -1;
	}
	if (strcmp(leido,"INICIAR_PATOTA")==0)
		return INICIAR_PATOTA;
	if (strcmp(leido,"LISTAR_TRIPULANTES")==0 || strcmp(leido,"1")==0)
		return LISTAR_TRIPULANTES;
	if (strcmp(leido,"INICIAR_PLANIFICACION")==0 || strcmp(leido,"2")==0)
		return INICIAR_PLANIFICACION;
	if (strcmp(leido,"PAUSAR_PLANIFICACION")==0 || strcmp(leido,"3")==0)
		return PAUSAR_PLANIFICACION;
	if (strcmp(leido,"OBTENER_BITACORA")==0)
		return OBTENER_BITACORA;
	if (strcmp(leido,"EXPULSAR_TRIPULANTE")==0)
		return EXPULSAR_TRIPULANTE;
	if (strcmp(leido,"DUMP_MEMORIA")==0)
		return DUMP_MEMORIA;
	if (strcmp(leido,"0")==0)
		return 456;

	return -1;
}

int conectar_con_ram(char* config_ruta){
	t_config* config  = config_create(config_ruta);

	char* ip = config_get_string_value(config,"IP_MI_RAM_HQ");

	char* puerto = config_get_string_value(config,"PUERTO_MI_RAM_HQ");

	return crear_conexion(ip, puerto);
}

int conectar_con_store(char* config_ruta){
	t_config* config  = config_create(config_ruta);

	char* ip = config_get_string_value(config,"IP_I_MONGO_STORE");

	char* puerto = config_get_string_value(config,"PUERTO_I_MONGO_STORE");

	return crear_conexion(ip, puerto);
}

void mostrar(t_list* lista,char* status){
	for(int i=0; i< list_size(lista);i++){
		Tripulante* t= list_get(lista,i);
		printf("Tripulante: %.3d       Patota: %.3d      X: %.2d		Y:%.2d	Quantum:%d   	Status: %s\n",t->tid,t->pid,t->X,t->Y,t->quantum,status);
	}
}
char * nombreOperacionES(int tarea){
	if (tarea == GENERAR_OXIGENO)
		return "GENERAR_OXIGENO";
	if (tarea == CONSUMIR_OXIGENO)
		return "CONSUMIR_OXIGENO";
	if (tarea == GENERAR_COMIDA)
		return "GENERAR_COMIDA";
	if (tarea == CONSUMIR_COMIDA)
		return "CONSUMIR_COMIDA";
	if (tarea == GENERAR_BASURA)
		return "GENERAR_BASURA";
	if (tarea == DESCARTAR_BASURA)
		return "DESCARTAR_BASURA";
}

