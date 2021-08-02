#ifndef tcp_server_H
#define tcp_server_H

#include "escuchar.h"

int iniciar_servidor(char*,char*);
void* connection_handler(void*);

#endif
