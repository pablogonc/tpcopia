/*
 * utilidades.h
 *
 *  Created on: 8 may. 2021
 *      Author: utnso
 */

#ifndef UTILIDADES_H_
#define UTILIDADES_H_

typedef enum //codigos de operaciones servidor
{
	//consola
	INICIAR_PATOTA,
	LISTAR_TRIPULANTES,
	EXPULSAR_TRIPULANTE,
	INICIAR_PLANIFICACION,
	PAUSAR_PLANIFICACION,
	OBTENER_BITACORA,
	//ram_tripulante
	INICIAR_TRIPULANTE,
	RECIBIR_UBICACION,
	RECIBIR_TAREAS,
	ENVIAR_PROXIMA_TAREA,
	DUMP_MEMORIA,
	NOHAYTAREAS, //ver
	FAILURE,
	//store
	SABOTAJE,
	FSCK,
	test2,
	GENERAR_OXIGENO,
	CONSUMIR_OXIGENO,
	GENERAR_COMIDA,
	CONSUMIR_COMIDA,
	GENERAR_BASURA,
	DESCARTAR_BASURA
}op_code;




typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

t_paquete* crear_paquete(op_code);
void agregar_a_paquete(t_paquete*, void*, int);
void enviar_paquete(t_paquete*, int );
void eliminar_paquete(t_paquete*);

int recibir_operacion(int);
int largo_paquete(int);
void * recibir(int);
void* recibir_buffer(int* , int );

#endif /* UTILIDADES_H_ */
