#!/bin/bash

gcc -I"../../utilidades" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"discordiador.d" -MT"discordiador.o" -o "discordiador.o" "../discordiador.c"

gcc -I"../../utilidades" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"tareas.d" -MT"tareas.o" -o "tareas.o" "../tareas.c"

gcc -I"../../utilidades" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"tripulante.d" -MT"tripulante.o" -o "tripulante.o" "../tripulante.c"

gcc -I"../../utilidades" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"utilidades_discordiador.d" -MT"utilidades_discordiador.o" -o "utilidades_discordiador.o" "../utilidades_discordiador.c"

gcc -L"../../utilidades/Debug" -o "Discordiador"  ./discordiador.o ./tareas.o ./tripulante.o ./utilidades_discordiador.o   -lcommons -lm -lpthread -lreadline -lutilidades