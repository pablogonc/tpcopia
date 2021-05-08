/*
 * servidor.h
 *
 *  Created on: 8 may. 2021
 *      Author: utnso
 */

#ifndef SERVIDOR_H_
#define SERVIDOR_H_


int iniciar_servidor(char * ,char *);

int esperar_cliente(int);

int recibir_operacion(int);

char* recibir_mensaje(int );

#endif /* SERVIDOR_H_ */
