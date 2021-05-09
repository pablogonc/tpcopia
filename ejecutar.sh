#!/bin/bash

terminator -e 'LD_LIBRARY_PATH="/home/utnso/TPSO/utilidades/Debug" ./I-Mongo-Store/Debug/I-Mongo-Store '  &

terminator -e 'LD_LIBRARY_PATH="/home/utnso/TPSO/utilidades/Debug" ./Mi-RAMHQ/Debug/Mi-RAMHQ ' &

terminator -e 'LD_LIBRARY_PATH="/home/utnso/TPSO/utilidades/Debug" ./Discordiador/Debug/Discordiador' &