#!/bin/bash

terminator -e 'LD_LIBRARY_PATH="utilidades/Debug" ./Discordiador/Debug/Discordiador' &

terminator -e 'LD_LIBRARY_PATH="utilidades/Debug" ./I-Mongo-Store/Debug/I-Mongo-Store '  &

terminator -e 'LD_LIBRARY_PATH="utilidades/Debug" ./Mi-RAMHQ/Debug/Mi-RAMHQ ' &
