/*
 * i-mongo-store.h
 *
 *  Created on: 27 jun. 2021
 *      Author: utnso
 */

#ifndef I_MONGO_STORE_H_
#define I_MONGO_STORE_H_


#include <commons/log.h>
#include <servidor.h>
#include <utilidades.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <commons/config.h>


#include <sys/stat.h> // para el mkdir
#include <unistd.h>  // para el F_OK en access (validar si existe el FS)
#include <commons/bitarray.h> //bitmap
#include <semaphore.h>

#define MAXCLIENTES	30
#define BLOQUES 1024
#define BLOQUES_SIZE 64

	sem_t* ocupado;
	t_log* logger;
//-------------------Config Store ------------_//

	char * puntoDeMontaje;
	char* puerto;
	int tiempoSincronizacion;
	char ** posicionesSabotaje;

//---------------------------------------------//

//----------------Estructuras --------------//

	typedef struct {
		uint32_t block_size;
		uint32_t blocks;
		t_bitarray *bitmap;
	} superBloque_t;



	typedef struct{
		uint32_t size;
		uint32_t block_count;
		int *blocks;
		char caracterLlenado;
		char MD5[32]; // ej.: BD1014D173BA92CC014850A7087E254E  hay q ver como se calcula.
	}file_t;

/*	typedef enum{
		Oxigeno,
		Basura,
		Comida
	}files_names;*/

	superBloque_t superBloque;

	char block[BLOQUES][BLOQUES_SIZE];




//----------------------------------------//

#endif /* I_MONGO_STORE_H_ */

/*
 * 3. de donde saco el block_size y la cantidad
 * 4. discordiador, pausar, los tripulantes siguen trabajando pero por 0,0001s mientras el planificador esta activo.
 * 5. bitmap y bloques, cantidad de bloques hardcodeado.
 * 6. los bloques pueden quedar a la mitad?
 * 7. preguntar cual usar en el bitarray_create si el LSB_FIRST o el otro.
 * 8. tareas y sabotajes: cuando el tripulante modifica un archivo, manda un mensaje al store y el store lo modifica, o lo
 * modifica el tripulante sobre una copia o como.
 *
 */


	//fseek apunta a lo que vas a modificar inmediatamente. 1234567  offset=5  1234asdasdasd




