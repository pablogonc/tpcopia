/*
 * cliente.h
 *
 *  Created on: 8 may. 2021
 *      Author: utnso
 */

#ifndef CLIENTE_H_
#define CLIENTE_H_

int crear_conexion(char *, char* );
void enviar_mensaje(char*, int );
void liberar_conexion(int );
#endif /* CLIENTE_H_ */
