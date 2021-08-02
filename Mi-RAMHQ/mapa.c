#include "mapa.h"
#include <stdio.h>

NIVEL* nivel;
int cols, rows;
int err;

int dibujarMapa() {
	nivel_gui_inicializar();
	nivel_gui_get_area_nivel(&cols, &rows);
	nivel = nivel_crear("AmongUs");
	nivel_gui_dibujar(nivel);
	return 0;
}

int agregarPersonajeMapa(char id, int x, int y){
	personaje_crear(nivel, id, x , y);
	err = nivel_gui_dibujar(nivel);

	return 0;
}

int moverPersonajeMapa(char id, int x, int y){
	item_mover(nivel, id, x, y);
	err = nivel_gui_dibujar(nivel);

	return 0;
}

int eliminarPersonajeMapa(char id){
	item_borrar(nivel, id);
	err = nivel_gui_dibujar(nivel);

	return 0;
}

int finalizarMapa(){
	nivel_destruir(nivel);
	nivel_gui_terminar();
	return 0;
}
