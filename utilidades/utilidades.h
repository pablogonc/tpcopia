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
	MENSAJE
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



#endif /* UTILIDADES_H_ */
