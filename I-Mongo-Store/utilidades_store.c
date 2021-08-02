/*
 * utilidades_store.c
 *
 *  Created on: 2 jul. 2021
 *      Author: utnso
 */

#include "utilidades_store.h"

void crearDirectorios(){

	mkdir(puntoDeMontaje, S_IRWXU);

	mkdir(string_from_format("%s/Files",puntoDeMontaje),S_IRWXU);

	mkdir(string_from_format("%s/Files/Bitacoras",puntoDeMontaje),S_IRWXU);

}

superBloque_t crearSuperBloque(){

	char * buffer;
	superBloque_t aux;

	aux.block_size = BLOQUES_SIZE;
	aux.blocks = BLOQUES; ///////////////preguntar si es hardcodeado

	buffer = malloc(aux.blocks/8);

	aux.bitmap = bitarray_create_with_mode(buffer,(aux.blocks)/8,LSB_FIRST); /////////////////////

	int ceros=0,unos = 0;
	for(int i = 0;i<aux.blocks;i++) {
		bitarray_clean_bit(aux.bitmap, i);
	}

	bitarray_set_bit(aux.bitmap,448);
	bitarray_set_bit(aux.bitmap,1000);
	bitarray_set_bit(aux.bitmap,1023);

	for(int i = 0;i<aux.blocks;i++) {
		if(bitarray_test_bit(aux.bitmap,i)){
			unos++; }else{    ceros++; }
	}

	printf("  unos : %d   ceros: %d\n",unos,ceros);

	FILE* superBloqueFile = fopen(string_from_format("%s/SuperBloque.bin", puntoDeMontaje), "wb");
	fwrite(&aux.block_size,sizeof(uint32_t),1,superBloqueFile);
	fwrite(&aux.blocks,sizeof(uint32_t),1,superBloqueFile);
	fwrite(aux.bitmap->bitarray,aux.blocks/8,1,superBloqueFile);
	fclose(superBloqueFile);


	return aux;
}

superBloque_t leerSuperBloque(){

	superBloque_t aux;
	FILE* superBloqueFile2 = fopen(string_from_format("%s/SuperBloque.bin", puntoDeMontaje), "rb");

	fread(&aux.block_size,sizeof(uint32_t),1,superBloqueFile2);
	fread(&aux.blocks,sizeof(uint32_t),1,superBloqueFile2);
	printf("bloques: %d\n block size: %d\n",aux.blocks,aux.block_size);

//----------------------------------------------------------------
	char *buffer;
	size_t size = aux.blocks/8;
	buffer = malloc(size);

	fread(buffer, size, 1, superBloqueFile2);

	aux.bitmap = bitarray_create_with_mode(buffer, aux.blocks / 8,LSB_FIRST);

	fclose(superBloqueFile2);

	int ceros=0,unos = 0;
	for(int i = 0; i<aux.blocks;i++){
		if(bitarray_test_bit(aux.bitmap,i)){
			unos++;}else{  ceros++;}
	}
	printf("unos : %d   ceros: %d\n",unos,ceros);

	return aux;
}

void update_bitmap_file() {

	FILE* superBloqueFile = fopen(string_from_format("%s/SuperBloque.bin", puntoDeMontaje), "wb");

//	int offset = sizeof(superBloque.blocks)+sizeof(superBloque.block_size);	printf("offset :%d",offset);
//	fseek(superBloqueFile,offset,SEEK_SET);

	fwrite(&superBloque.block_size,sizeof(uint32_t),1,superBloqueFile);
	fwrite(&superBloque.blocks,sizeof(uint32_t),1,superBloqueFile);
	fwrite(superBloque.bitmap->bitarray,superBloque.blocks/8,1,superBloqueFile);

	fclose(superBloqueFile);

}

//-----------------------BLOQUE

void crearBloques(){
	for (int i = 0; i<(superBloque.blocks);i++){
		for(int j= 0;j<(superBloque.block_size);j++){
			block[i][j] = ' ';
		}
	}

	FILE * bloqueFile = fopen(string_from_format("%s/Bloque.bin",puntoDeMontaje),"wb");

	fwrite(block,superBloque.block_size,superBloque.blocks,bloqueFile);

	fclose(bloqueFile);

}

void leerBloques(){

	FILE * bloqueFile = fopen(string_from_format("%s/Bloque.bin",puntoDeMontaje),"rb");

		fread(block,superBloque.block_size,superBloque.blocks,bloqueFile);

		fclose(bloqueFile);

		for (int i = 0;i<superBloque.blocks;i++){
			for(int j = 0; j<superBloque.block_size;j++){
				printf("%d %d :: %c\n",i,j,block[i][j]);
			}
		}
}

int buscarBloqueVacio(){
	int i = 0;
	while(bitarray_test_bit(superBloque.bitmap,i)){
		i++;
		if(i>1023){
			return -1;  //////// revisar
		}
	}
	bitarray_set_bit(superBloque.bitmap,i);
	update_bitmap_file();
	return i;

}

int escribirBloque(file_t** file){

	//validar que el ultimo bloque no este lleno, si esta lleno, asignar uno nuevo.
	(*file)->size++;
	if(block[(*file)->blocks[(*file)->block_count-1]][superBloque.block_size -1] != ' '){
		// asignar un nuevo bloque:
		//buscar bloque disponible
		int bloque = buscarBloqueVacio();
		//bloque al file
		(*file)->block_count++;
		int *aux= malloc(sizeof(int)*(*file)->block_count);
		for(int i=0;i<(*file)->block_count;i++){
			aux[i]=(*file)->blocks[i];
		}
		aux[(*file)->block_count-1]= bloque;
		(*file)->blocks = aux;

		//llenar bloque
		block[bloque][0] = (*file)->caracterLlenado;


	} else {
		// agregar un caracter de llenado en el lugar disponible
		int indice =0;
		while(block[(*file)->blocks[(*file)->block_count-1]][indice] != ' '){
			indice++;
		}
		block[(*file)->blocks[(*file)->block_count-1]][indice] = (*file)->caracterLlenado;

	}
	updateIms(file);
	updateBlock();

	return 0;
}

void borrarBloque(file_t** file){

	(*file)->size--;
	int indice=0;
	while(block[(*file)->blocks[(*file)->block_count-1]][indice] == (*file)->caracterLlenado && indice <superBloque.block_size){
		indice++;
	}
	indice--;
	block[(*file)->blocks[(*file)->block_count-1]][indice] = ' ';

	if (indice==0 && (*file)->block_count >1 ){

		printf("Se despeja un gran bloque \n");
		int lastBlock = (*file)->blocks[(*file)->block_count-1];
		(*file)->block_count--;
		int *aux= malloc(sizeof(int)*(*file)->block_count);
		for(int i=0;i<(*file)->block_count;i++){
			aux[i]=(*file)->blocks[i];
		}
		free((*file)->blocks);
		(*file)->blocks = aux;
		bitarray_clean_bit(superBloque.bitmap,lastBlock);
		update_bitmap_file();
		printf("es libre\n");


	}
	updateIms(file);
	updateBlock();

}

void updateIms(file_t** file){
	char * nombre;

	switch((*file)->caracterLlenado){
	case 'O': case 'o':
		nombre="Oxigeno";
		break;
	case 'C': case 'c':
		nombre ="Comida";
		break;
	case 'B': case 'b':
		nombre ="Basura";
		break;
	}


	FILE* archivo = fopen(string_from_format("%s/Files/%s.ims",puntoDeMontaje,nombre),"w");

	fprintf(archivo,"SIZE=%d\n",(*file)->size);
	fprintf(archivo,"BLOCK_COUNT=%d\n",(*file)->block_count);

	//--------------------------------------- WARNING//todo

	fprintf(archivo,"BLOCKS=[");
	for(int i =0 ; i<(*file)->block_count-1;i++){
		fprintf(archivo,"%d,",(*file)->blocks[i]);
	}
	fprintf(archivo,"%d]\n",(*file)->blocks[(*file)->block_count-1]);
	//-------------------------------------------------------------------------------------------
	fprintf(archivo,"CARACTER_LLENADO=%c\n",(*file)->caracterLlenado);
	fprintf(archivo,"MD5_ARCHIVO=");

	fclose(archivo);
	char*ruta = string_from_format("%s/Files/%s.ims",puntoDeMontaje,nombre);

	system(string_from_format("md5sum %s > salida.txt\n",ruta));


	FILE * salida = fopen("salida.txt","r");
	archivo = fopen(string_from_format("%s/Files/%s.ims",puntoDeMontaje,nombre),"a");
	size_t linesize = 32;
	char md5[32] ;

	fgets( md5, 33, salida ); // funciona
	for(int i=0;i<32;i++){
		(*file)->MD5[i]=md5[i];
	}

	fclose(salida);
	remove("salida.txt");
	fprintf(archivo,"%s\n",md5);
	//printf("\n MD5: '%s' \n",file.MD5);                        /////// de donde saca la basuraaaa
	fclose(archivo);

}

void updateBlock(){ //teoricamente es valido todo
	FILE * bloqueFile = fopen(string_from_format("%s/Bloque.bin",puntoDeMontaje),"wb");

	fwrite(block,superBloque.block_size,superBloque.blocks,bloqueFile);

	fclose(bloqueFile);
}

//-----------------------FILE

void obtenerFile(file_t** file,char* nombre){

	if(!access(string_from_format("%s/Files/%s.ims", puntoDeMontaje,nombre),F_OK)==0){
		crearArchivo(file,nombre);
	}else{
		leerArchivo(file,nombre);
	}


//	FILE* archivo = fopen(string_from_format("%s/Files/%s.ims",puntoDeMontaje,nombre),"a");
//	fclose(archivo);



}

void crearArchivo(file_t** aux,char* nombre){ //nombre = Oxigeno   sin .ims
	(*aux) = malloc(sizeof(file_t));
	(*aux)->blocks = malloc(sizeof(int));
	(*aux)->size = 0;
	(*aux)->block_count = 1;
	(*aux)->blocks[0] = buscarBloqueVacio(superBloque);

	FILE* archivo = fopen(string_from_format("%s/Files/%s.ims",puntoDeMontaje,nombre),"w");

	switch(nombre[0]){
	case 'O': case 'o':
		(*aux)->caracterLlenado ='O';
		break;
	case 'C': case 'c':
		(*aux)->caracterLlenado ='C';
		break;
	case 'B': case 'b':
		(*aux)->caracterLlenado ='B';
		break;
	default:
		printf("nombre del file incorrecto");
		break;
	}

	fprintf(archivo,"SIZE=%d\n",(*aux)->size);
	fprintf(archivo,"BLOCK_COUNT=%d\n",(*aux)->block_count);
	fprintf(archivo,"BLOCKS=[%d]\n",(*aux)->blocks[0]);
	fprintf(archivo,"CARACTER_LLENADO=%c\n",(*aux)->caracterLlenado);
	fprintf(archivo,"MD5_ARCHIVO=");

	//fclose(archivo);

	char*ruta = string_from_format("%s/Files/%s.ims",puntoDeMontaje,nombre);

	system(string_from_format("md5sum %s > salida.txt\n",ruta));


	FILE * salida = fopen("salida.txt","r");

	size_t linesize = 32;

	fgets( (*aux)->MD5, 33, salida ); // funciona

	fclose(salida);
	remove("salida.txt");
	fprintf(archivo,"%s\n",(*aux)->MD5);
	printf("\n MD5: '%s' \n",(*aux)->MD5);                        /////// de donde saca la basuraaaa
	fclose(archivo);

}
/*
void leerArchivo(file_t** aux,char* nombre){

	(*aux) = malloc(sizeof(file_t));

	FILE* archivo = fopen(string_from_format("%s/Files/%s.ims",puntoDeMontaje,nombre),"r");

	fscanf(archivo,"SIZE=%d\n",&(*aux)->size);
	fscanf(archivo,"BLOCK_COUNT=%d\n",&(*aux)->block_count);

	char* arrayNumbers = malloc(sizeof(int)* (*aux)->block_count + (*aux)->block_count);//puede romper mas adelante de esto memorycorruption por ser muy chico el malloc -agrandar malloc para reparar

	fscanf(archivo,"BLOCKS=%s\n",arrayNumbers);

	getArrayV2(&((*aux)->blocks) ,arrayNumbers,(*aux)->block_count);

	fscanf(archivo,"CARACTER_LLENADO=%c\n",&(*aux)->caracterLlenado);
	fscanf(archivo,"MD5_ARCHIVO=%s",(*aux)->MD5);

	fclose(archivo);

	printf("\nSIZE=%d\n",(*aux)->size);
	printf("BLOCK_COUNT=%d\n",(*aux)->block_count);
	printf("BLOCKS=[");
	for(int i=0;i<(*aux)->block_count;i++){
			printf("%d",(*aux)->blocks[i]);
			if( i !=(*aux)->block_count-1)
				printf(",");
	}
	printf("]\n");
	printf("CARACTER_LLENADO=%c\n",(*aux)->caracterLlenado);
	printf("MD5_ARCHIVO=%s\n",(*aux)->MD5);
	printf("BLOCKS=[%d]\n",(*aux)->blocks[(*aux)->block_count - 1]);


	//free(arrayNumbers);//todo  por si acaso

}*/

void leerArchivo(file_t** aux,char* nombre){

	(*aux) = malloc(sizeof(file_t));

	char * path = string_from_format("%s/Files/%s.ims",puntoDeMontaje,nombre);

	(*aux)->size = atoi(charFromFile(path,"SIZE"));

	(*aux)->block_count = atoi(charFromFile(path,"BLOCK_COUNT"));

	char* arrayNumbers = charFromFile(path,"BLOCKS");

	getArrayV2(&((*aux)->blocks) ,arrayNumbers,(*aux)->block_count);
	free(arrayNumbers);
	(*aux)->caracterLlenado = charFromFile(path,"CARACTER_LLENADO")[0];

	strcpy((*aux)->MD5,charFromFile(path,"MD5_ARCHIVO"));
/*
	printf("\nSIZE=%d\n",(*aux)->size);
	printf("BLOCK_COUNT=%d\n",(*aux)->block_count);
	printf("BLOCKS=[");
	for(int i=0;i<(*aux)->block_count;i++){
			printf("%d",(*aux)->blocks[i]);
			if( i !=(*aux)->block_count-1)
				printf(",");
	}

	printf("]\n");
	printf("CARACTER_LLENADO=%c\n",(*aux)->caracterLlenado);
	printf("MD5_ARCHIVO=%s\n",(*aux)->MD5);
	printf("BLOCKS=[%d]\n",(*aux)->blocks[(*aux)->block_count - 1]);
*/

	//free(arrayNumbers);//todo  por si acaso

}

void insertarEnBloque(char* nombre,int cantidad){
	file_t* aux;
	obtenerFile(&aux,nombre);
	for(int i=0;i<cantidad;i++){
		escribirBloque(&aux); //ver que hacer con estado
		//sleep(1);
	}
	free(aux->blocks);
	free(aux);

}

void eliminarDeBloque(char* nombre,int cantidad){
	file_t* aux;
	obtenerFile(&aux,nombre);
	int flag=0;
	for(int i=0;i<cantidad;i++){
		if (aux->size >0){
			borrarBloque(&aux); //ver que hacer con estado
		}else{
			flag++;
		}

		//sleep(1);
	}
	if(flag){
		log_warning(logger,"Usted intento eliminar %d elementos mas de lo que habia, usted se tiene que arrepentir de lo que hizo",flag);
	}
	free(aux->blocks);
	free(aux);
}


void getArrayV2(int** array,char* texto,int tamanio){

	char * algo = strtok(texto,"[");


	(*array) =malloc(sizeof(int)*tamanio);
	char* aux;

	(*array)[0]=atoi(strtok(algo,","));

	int offset=1;
	while( (aux=strtok(NULL,","))!=NULL  ){
		(*array)[offset]=atoi(aux);
		offset++;
	}

}

int getKeyLine(FILE* file,char* key,int* lineSize){

	int startKey = ftell(file);
	char c;


	while(c != '='){
		c = getc(file);
	}
	int endKey = ftell(file);
	fseek(file,startKey,SEEK_SET);
	char* actKey = malloc(endKey-startKey);
	fgets(actKey,endKey-startKey,file);

	if(strcasecmp(key,actKey)==0){
		while(c != '\n'){
			c = getc(file);
		}
		*lineSize= ftell(file)-endKey;
		fseek(file,endKey,SEEK_SET);
		return 1;
	}else{

		while(c != '\n'){
			c = getc(file);
			if(feof(file)){
				*lineSize=-1;
				return -1;
			}
		}

		return getKeyLine(file,key,lineSize);
	}




}

char* charFromFile(char* path,char* key){
	FILE* file = fopen(path,"r");
	int keySize;
	if(getKeyLine(file,key,&keySize) ==-1){
		return NULL;
	}
	char* aux = malloc(keySize);

	fgets(aux,keySize,file);

	fclose(file);
	return aux;
}

// nota: los sabotajes estan sin testear.---------------------------------
//-----------------------SABOTAJES SUPERBLOQUE---------------------------
// SABOTAJE 1: VERIFICAR CANTIDAD DE BLOQUES = LO QUE DIGA EL SUPERBLOQUE. // SE SOLUCIONA REESCRIBIENDO EL SUPERBLOQUE.BLOCKS

int cantidadDeBloques(){
	return sizeof(block)/sizeof(block[0]);			//deberia funcionar
}

int sabotaje1(){
	superBloque = leerSuperBloque();
	int cantidadDeBloquesReales = cantidadDeBloques();
	if(superBloque.blocks != cantidadDeBloquesReales){
		superBloque.blocks = cantidadDeBloquesReales;
		update_bitmap_file(); 		//estaba pensando en una funcion updateSuperBloque(); pero al parecer el update de bitmap escribe todo xd

		//superBloque = leerSuperBloque();

		log_warning(logger,"Sabotaje resuelto con exito\n");

		return 0;
	} else {
		log_info(logger,"No estaba Saboteado\n");
		return 1;
	}
}

// SABOTAJE 2: VER QUE LOS BLOQUES LIBRES Y NO DEL BITMAP SEAN LOS QUE DIGA SER // SE SOLUCIONA REESCRIBIENDO EL SUPERBLOQUE.BITMAP

int validarBloques(){								// no se esta usando por ahora
	int contadorDiscordancias=0;
	leerBloques(superBloque);
	for(int i = 0; i<superBloque.blocks;i++){
		contadorDiscordancias += validarBloque(i);
	}
}

int validarBloque(int indice){						// no se esta usando por ahora
	int flag = 0;
	if(bitarray_test_bit(superBloque.bitmap,indice)){
		if(block[indice][0]==' '){
			bitarray_clean_bit(superBloque.bitmap,indice);
			flag ++;
		}
	}else{
		if(block[indice][0]!=' '){
			bitarray_set_bit(superBloque.bitmap,indice);
			flag ++;
		}
	}
	return flag;
}

void verificarBitmap(char*nombre){


	if(access(string_from_format("%s/Files/%s.ims", puntoDeMontaje,nombre),F_OK)==0){
		file_t* aux;
		leerArchivo(&aux,nombre);


		for(int i = 0;i<aux->block_count;i++){
			bitarray_set_bit(superBloque.bitmap,aux->blocks[i]);
		}
		free(aux->blocks);
		free(aux);
	}
}

int sabotaje2(){

	superBloque = leerSuperBloque();

	char* buffer = malloc(superBloque.blocks/8);
	bitarray_destroy(superBloque.bitmap);
	superBloque.bitmap = bitarray_create_with_mode(buffer,(superBloque.blocks)/8,LSB_FIRST);

	verificarBitmap("Oxigeno");

	verificarBitmap("Comida");

	verificarBitmap("Basura");

	update_bitmap_file();			//puede o no ir el free del buffer

	return 0;
}




//------------ SABOTAJE DE LOS FILES ------------------
// SABOTAJE 3: VALIDAR EL TAMAÑO DE LOS FILES COMPARANDO CON LO QUE TENGAN LOS BLOQUES //SOLUCION: ASUMIR CORRECTO LO DE BLOQUES

int sabotaje3(char * nombre){

	// conseguir el size del archivo.
	// conseguir los bloques que tenga el archivo
	// contar la cantidad de cosas que tengan esos bloques.
	// validar contra el size del archivo.

	file_t* aux;
	leerArchivo(&aux,nombre);

	int contador = 0;

	for(int indiceI = 0;indiceI < aux->block_count;indiceI++){
		for(int indiceJ = 0; indiceJ < superBloque.block_size; indiceJ++){
			if(block[aux->blocks[indiceI]][indiceJ]!= ' '){
				contador++;
			}
		} /////aux->blocks = [10,11,20,30]
	}

	if(contador != aux->size){
		log_warning(logger,"Archivo size saboteado, reparando...\n");
		aux->size = contador;
		updateIms(&aux);

		free(aux);
		log_warning(logger,"Sabotaje 3 resuelto con exito\n");
		return 0;

	}else{
		log_warning(logger,"Archivo size no estaba saboteado. contador: %d, size: %d\n",contador,aux->size);
		free(aux);
		return 1;
	}
}

/* SABOTAJE 4 Block_count y blocks: Al encontrar una diferencia entre estos dos se debe restaurar la consistencia.
 * Reparación: Actualizar el valor de Block_count en base a lo que está en la lista de Blocks.
 */

int contarcomas(char* string){

	int comas=0;
	if(strtok(string,",")!=NULL){
		while(strtok(NULL,",")!=NULL){
			comas++;
		}
	}
	return comas;
}

int sabotaje4(char*nombre){
	file_t* aux;
	leerArchivo(&aux,nombre);

	// ver cuantas cosas hay en el int*array
	// comparar con el aux.cantidad
	char * path = string_from_format("%s/Files/%s.ims",puntoDeMontaje,nombre);

	char* bloques = charFromFile(path,"BLOCKS");

	int cantBloques = contarcomas(bloques) + 1;

	if(cantBloques != aux->block_count){

		log_warning(logger,"Archivo block_count saboteado, reparando...\n");
		aux->block_count = cantBloques;
		updateIms(&aux);

		free(aux);
		log_warning(logger,"Sabotaje 4 resuelto con exito\n");
		return 0;
	}else{
		log_warning(logger,"Archivo block_count no estaba saboteado. cantBloques: %d, block_count: %d\n",cantBloques,aux->block_count);
		free(aux);
		return 1;
	}

}

/* SABOTAJE 5: Blocks: Se debe validar que los números de bloques en la lista sean válidos dentro del FS.
 * Reparación: Restaurar archivo.
 */

int sabotaje5(char*nombre){
	file_t* aux;
	leerArchivo(&aux,nombre);


	// verificar archivo que este mal
	int restaurar = 0;
	int size = aux->size;
	int bloque = 0;

	for(int i = 0;i<aux->block_count;i++){
		bloque = aux->blocks[i];
		if(bloque<0){
			log_warning(logger,"bloque negativo\n");
			restaurar = 1;
		}
		if(bloque > superBloque.blocks){
			log_warning(logger,"bloque fuera de rango\n");
			restaurar = 1;
		}
		if(bloque != (aux->blocks[aux->block_count-1]) && block[bloque][superBloque.block_size-1]==' '){
			log_warning(logger, "ultimo bloque mal posicionado, bloque = %d, iteracion = %d \n",bloque,i);
			restaurar = 1;
		}
	}
	// llamar a eliminar caracter tantas veces como diga el size del archivo  eliminarBloque()
	// volver a llenar

	if(restaurar){
		eliminarDeBloque(nombre,size);
		insertarEnBloque(nombre,size);
		log_warning(logger,"sabotaje 5 resuelto con exito\n");
		free(aux->blocks);
		free(aux);
		return 0;
	}else{
		log_warning(logger,"no habia nada saboteado\n");
		free(aux->blocks);
		free(aux);
		return 1;
	}
}



