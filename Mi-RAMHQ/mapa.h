#ifndef mapa_H
#define mapa_H

#include <nivel-gui/nivel-gui.h>
#include <nivel-gui/tad_nivel.h>

int dibujarMapa(void);
int agregarPersonajeMapa(char,int,int);
int moverPersonajeMapa(char,int,int);
int eliminarPersonajeMapa(char);
int finalizarMapa(void);

#endif
