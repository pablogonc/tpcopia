#!/bin/bash
tput setaf 2 && echo $' \n\n------- Descargando commons -------\n \n' && tput sgr0 &&
cd .. && git clone https://github.com/sisoputnfrba/so-commons-library.git && cd ./so-commons-library &&
tput setaf 2 && echo instalando commons  && tput sgr0 &&
sudo make install && cd .. &&
tput setaf 2 && echo $' \n\n-------Descargando Nivel GUI -------\n\n' && tput sgr0 &&
git clone https://github.com/sisoputnfrba/so-nivel-gui-library.git && cd ./so-nivel-gui-library &&
tput setaf 2 && echo $' \n\n------- instalando Nivel GUI -------\n\n' &&  tput sgr0 &&
sudo make install && cd .. &&  cd ./tp-2021-1c-Error-404 &&
tput setaf 2 && echo $' \n\n-------Librerias instaladas -------\n\n' && tput sgr0 &&
tput setaf 2 && echo $' \n\n-------preparando utilidadess -------\n\n' && tput sgr0 &&
cd ./utilidades/Debug &&
sudo ./compilarUtilidades.sh &&
cd ../../ &&
tput setaf 2 && echo $' \n\n-------discordiador -------\n\n' && tput sgr0 &&
cd ./Discordiador/Debug &&
sudo ./compilarDiscordiador.sh &&
cd ../../ &&
tput setaf 2 && echo $' \n\n-------ram -------\n\n' && tput sgr0 &&
cd ./Mi-RAMHQ/Debug &&
sudo ./compilarRAM.sh &&
cd ../../ &&
tput setaf 2 && echo $' \n\n-------store -------\n\n' && tput sgr0 &&
cd ./I-Mongo-Store/Debug &&
sudo ./compilarStore.sh &&
cd ../../ &&
tput setaf 2 && echo $' \n\n------- todo listo -------\n\n' && tput sgr0 &&
export LD_LIBRARY_PATH="utilidades/Debug"



