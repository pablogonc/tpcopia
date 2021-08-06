/*
 * utilidades_store.h
 *
 *  Created on: 2 jul. 2021
 *      Author: utnso
 */

#ifndef UTILIDADES_STORE_H_
#define UTILIDADES_STORE_H_
#include "i-mongo-store.h"

	void crearDirectorios();

//superBloque
	superBloque_t crearSuperBloque();
	superBloque_t leerSuperBloque();
	void update_bitmap_file();

//Bloques
	void crearBloques();
	void leerBloques();
	int validarBloques();
	int validarBloque(int);
	int buscarBloqueVacio();

	int escribirBloque(file_t**);
	void borrarBloque(file_t**);
	void updateIms(file_t**);
	void updateBlock();

//FILE
	void obtenerFile(file_t**,char*);
	void crearArchivo(file_t**,char*);
	void leerArchivo(file_t**,char*);

	void insertarEnBloque(char* ,int);
	void eliminarDeBloque(char* ,int);

	void getArrayV2(int**,char* texto,int tamanio);
	int getKeyLine(FILE* ,char* ,int*);
	char* charFromFile(char* ,char* );
//
	void sincronizar();

	int tamanioPosicionesSabotaje();
	int * posicionSabotaje();
#endif /* UTILIDADES_STORE_H_ */

	/* orden de cosas:
	 * 1. MD5 // dejar con basura e ignorarla
	 * 2. obtener el file ya iniciado / leer uno que ya exista y poder salvar los valores
	 * 3. escribir en los files / ver tareas de E/S hardcodeadas
	 * 4. ver comunicacion con discordiador
	 * 5. sabotajes
	 */
