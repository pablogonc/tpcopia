/*
 * test.c
 *
 *  Created on: 24 may. 2021
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <stdint.h>
#include <signal.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <commons/bitarray.h>

#include <sys/mman.h>
#include <fcntl.h>

#include <sys/types.h>
#define SIZEEE "SIZE123"


#include <openssl/md5.h>


int contarcomas(char* string){

	int comas=0;
	if(strtok(string,",")!=NULL){
		while(strtok(NULL,",")!=NULL){
			comas++;
		}
	}
	return comas;
}

int main(){
	int var =0;
	if(var){
		printf("hola");
	}
}


/*

typedef struct {
	uint32_t block_size;
	uint32_t blocks;
	int* bitmap;
} superBloque;

typedef struct {
	uint32_t tid;
	uint32_t blocks;
	int* bitmap;
} tripulante;



void showlist(t_list * l){
	tripulante * t;
	printf("lista. \n");
	for(int i=0;i<list_size(l);i++){
		t= list_get(l,i);
		printf("tid: %d\n",(int) t -> tid );
	}
}







int* getArray(char* texto,int* tamanio);
int* getArrayV2(char* texto,int tamanio);


int* getArrayV2(char* texto,int tamanio){

	char * algo = strtok(texto,"[");

	algo = strtok(algo,"]");

	int *ptr =malloc(sizeof(int)*tamanio);
	char* aux;

	ptr[0]=atoi(strtok(algo,","));

	int offset=1;
	while( (aux=strtok(NULL,","))!=NULL  ){
		ptr[offset]=atoi(aux);
		offset++;
	}

	return ptr;
}

int* getArray(char* texto,int* tamanio){
	char * auxstring = malloc(strlen(texto));
	strcpy(auxstring,texto);
	int numeros = contarcomas(auxstring)+1;


	char * algo = strtok(texto,"[");

	algo = strtok(algo,"]");

	int *ptr =malloc(sizeof(int)*numeros);
	char* aux;

	ptr[0]=atoi(strtok(algo,","));

	int offset=1;
	while( (aux=strtok(NULL,","))!=NULL  ){
		ptr[offset]=atoi(aux);
		offset++;
	}
	*tamanio= offset;
	return ptr;
}

int contarcomas(char* string){

	int comas=0;
	if(strtok(string,",")!=NULL){
		while(strtok(NULL,",")!=NULL){
			comas++;
		}
	}
	return comas;
}

typedef struct{
		uint32_t size;
		uint32_t block_count;
		int *blocks;
		char caracterLlenado;
		char MD5[32]; // ej.: BD1014D173BA92CC014850A7087E254E  hay q ver como se calcula.
	}file_t;

int main (){
	file_t* aux;
	aux->blocks = malloc(sizeof(int));
	char* arraychar ;

	FILE* archivo = fopen("Oxigeno.ims","r");

	fscanf(archivo,"SIZE=%d\n",&aux.size);
	fscanf(archivo,"BLOCK_COUNT=%d\n",&aux.block_count);
	fscanf(archivo,"BLOCKS=%s\n",arraychar);
	fscanf(archivo,"CARACTER_LLENADO=%c\n",&aux.caracterLlenado);
	fscanf(archivo,"MD5_ARCHIVO=%s",aux.MD5);
	fclose(archivo);

	printf("block count:::: %d\n",aux.block_count);
	printf("hola:::: [");
	aux.blocks = getArrayV2((*aux)->blocks,arraychar,aux.block_count);


	for(int i=0;i<aux.block_count;i++){
		printf("%d",aux.blocks[i]);
		if( i !=aux.block_count-1)
			printf(",");
	}
	printf("]");


	/*t_list * lista_patotas = list_create();

	tripulante* a = malloc(sizeof(tripulante));
	a->tid=17;
	list_add(lista_patotas,a);
	a= malloc(sizeof(tripulante));
	a->tid=15;
	list_add(lista_patotas,a);
	a= malloc(sizeof(tripulante));
	a->tid=1;
	list_add(lista_patotas,a);
	a= malloc(sizeof(tripulante));
	a->tid=41;
	list_add(lista_patotas,a);
	a= malloc(sizeof(tripulante));
	a->tid=4;
	list_add(lista_patotas,a);

	showlist(lista_patotas);

	bool orderById(tripulante* a, tripulante* b){
		return (a->tid) < (b->tid);
	}

	t_list *aux= list_sorted(lista_patotas,&orderById);
	showlist(aux);
	list_destroy(lista_patotas);

}













/*

int main () {
   char str1[10], str2[10], str3[10];
   int numero;
   FILE * fp;
   char * size = "SIZE";
   fp = fopen ("file.txt", "r+");

   char * aux = string_from_format("%s=%d\n",size);

   char * block = "BLOCK_COUNT";
   char * block2= "BLOCKS";
   fscanf(fp,string_from_format("%s=%%d\n",size),&numero);
   printf("%d\n",numero);

   fscanf(fp,string_from_format("%s=%%d\n",block),&numero);
   printf("%d\n",numero);

   char hola[20000];
   fseek(fp,strlen(block2)+1,SEEK_CUR); // +2 para tirar el [

/*
   while(fscanf(fp,"%d,",&numero)!=0){
	   printf("%d",numero);
   }
   fscanf(fp,"%d",&numero);
   printf("%d",numero);


   fpos_t pos;
   fgetpos(fp,&pos);

   fscanf(fp,"%s\n",hola);
   printf("%s",hola);

   int contador = 0;

   if(strtok(hola,",")!=NULL){
	   contador++;
   }

   fsetpos(fp,&pos);

   fclose(fp);

   return(0);
}

*/



/*

int main(void){

	int*test;

	int test2 = 4;

	printf("%d",sizeof(test));
	test = malloc(sizeof(int));
	test[0] = 2;
	test[1] = 2;
	printf("%d",test[1]);
	free(test);
	printf("%d",test[1100]);
	test[112] = 2;
	printf("%d",test[112]);

return 0;
/*
    FILE *fpipe;
    char command = "ls";
    char c = 0;

    if (0 == (fpipe = popen(command, "r")))
    {
        perror("popen() failed.");
        exit(EXIT_FAILURE);
    }
    printf("hola");
    while (fread(&c, sizeof c, 1, fpipe))
    {
        printf("%c", c);
    }

    pclose(fpipe);

    return EXIT_SUCCESS;

	/*
	MD5_CTX md5;
    unsigned char out[MD5_DIGEST_LENGTH];
    char buf[512];

	char * contenido = "hola";
	char * nombre = "hola.txt";

	FILE * archivo = fopen(nombre,"w");
	fwrite(contenido,strlen(contenido),1,archivo);
	fclose(archivo);

	MD5_Init(&md5);

    ssize_t bytes=read(STDIN_FILENO, buf, 512);

    while(bytes > 0)
        {
            MD5_Update(&md5, buf, bytes);
            bytes=read(STDIN_FILENO, buf, 512);
        }

    MD5_Final(out, &md5);

    for(int n=0; n<MD5_DIGEST_LENGTH; n++)
        printf("%02x", out[n]);
    printf("\n");
//(8888888888888888888888888888888888888888888888888888

	char md5[32];
	char * nombre = "hola.txt";  //usa system que no me gusta mucho que digamos, supuestamente si llega a romper la llamada a system, FFFFFF
	FILE * archivo = fopen(nombre,"w");
	char * contenido = "hola";

	fwrite(contenido,strlen(contenido),1,archivo);

	fclose(archivo);


	system(string_from_format("md5sum %s > salida.txt",nombre));

	FILE * salida = fopen("salida.txt","r");
	fread(md5,32,1,salida);
	fclose(salida);
	remove("salida.txt");
	printf("%s",md5);
	88888888888888888888888
/*
	char matriz [20][22];
	for (int i = 0;i<20;i++){
		for(int j = 0; j<22;j++){
			matriz[i][j]= 'o';
		}
		4d186321c1a7f0f354b297e8914ab240
		4d186321c1a7f0f354b297e8914ab240

	}4d186321c1a7f0f354b297e8914ab240
51db41a195b9a4b26f9e1f16d72e7b20

	matriz[5][5]= 'a';
	matriz[3][2]= 'b';
	matriz[1][1]= 'c';

	FILE * archivo = fopen("archivo.bin","w");

	fwrite(matriz,22,20,archivo);

	fclose(archivo);

	char matriz2 [20][22];
	FILE * archivo2 = fopen("archivo.bin","r");

	fread(matriz2,22,20,archivo);

	for (int i = 0;i<20;i++){
		for(int j = 0; j<22;j++){
			printf("%d %d :: %c\n",i,j,matriz2[i][j]);
		}
	}
	for (int i = 0;i<20;i++){
		if(matriz2[i][0]!=' '){
			return -1;
		}
		return 0;
	}

/*
	char* puntoDeMontaje = "/home/utnso/FSpmt";
	printf("hola4");
	//mkdir((string_from_format("%s/BLoque/asad/123412asda", puntoDeMontaje)), S_IRWXU);
	printf("hola3");
	int i = 10;
	FILE* bitmap_f = fopen(string_from_format("%s/Bitmap%d.txt", puntoDeMontaje,i), "w+");
	superBloque bloque;
	bloque.blocks = NULL;
	bloque.block_size = 10;
	bloque.bitmap = &magiaNegra;
	fwrite(&bloque,sizeof(bloque),1,bitmap_f); //1
	fwrite(&bloque,sizeof(bloque),1,bitmap_f); //2
	fwrite(&bloque,sizeof(bloque),1,bitmap_f); //3
	fwrite(&bloque,sizeof(bloque),1,bitmap_f); //4
	fwrite(&bloque,sizeof(bloque),1,bitmap_f); //5
	int o = 5;
	fwrite(&o,sizeof(o),1,bitmap_f);
	//i++;
	printf("hola2");
	fclose(bitmap_f);
	FILE* bitmap_f2 = fopen(string_from_format("%s/Bitmap%d.txt", puntoDeMontaje,i), "r+");
	superBloque lectura;
	fread(&lectura,sizeof(superBloque),1,bitmap_f2);
	for(int i = 0;i<10;i++){
	printf("\n%d",(lectura.bitmap)[i]);
	}
	fclose(bitmap_f2);
	printf("\nhola");
	char* puntoDeMontaje = "/home/utnso/FSpmt";
	char * filename = "Bitmap10";
	char * direccion = "/home/utnso/FSpmt";
	if(access(string_from_format("%s/Bitmap0.txt", puntoDeMontaje),F_OK)==0){
		printf("el archivo existe\n");
	}else{
		printf("el archivo no existe\n");
	}
		char* puntoDeMontaje = "/home/utnso/FSpmt";
	FILE * bloqueFile = fopen(string_from_format("%s/Blocks.bin", puntoDeMontaje), "wb");
	uint32_t block_size = 64;
	uint32_t blocks = 1024;
	int tamanio = block_size * blocks;
	fwrite("",tamanio,1,bloqueFile);
///////////
	mode_t mode = S_IRWXO | S_IRWXG | S_IRWXU;
	int archivo = open("hola.txt",O_CREAT | O_RDWR,mode);
    struct stat statbuf;
	int n =  fstat(archivo,&statbuf);
    char *ptr = mmap(NULL,statbuf.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,archivo,0);
    for(int i = 0; i<statbuf.st_size;i++){
    	ptr[i]='a';
    	printf("%c",ptr[i]);
    }
    readline("");
    printf("\n");
    char *copy = malloc(statbuf.st_size);
    memcpy(copy,ptr,statbuf.st_size);
    for(int i = 0; i<statbuf.st_size;i++){
    	copy[i]='b';
    	printf("%c",copy[i]);
    }
    printf("\n");
    int len = statbuf.st_size;
    memcpy(ptr,copy,statbuf.st_size);
    readline("");
    if (msync(ptr, len, MS_SYNC ) < 0 ) {
         perror("msync failed with error:");
         return -1;
     }
     else (void) printf("%s","msync completed successfully.");
/////

	mode_t mode = S_IRWXO | S_IRWXG | S_IRWXU;
	int archivo = open("hola.txt",O_CREAT | O_RDWR,mode);

	const char *text = "cccccccccccccccccccc";
	size_t textsize = strlen(text) + 1;

//	lseek(archivo, textsize-1, SEEK_SET);

	write(archivo, text, textsize);

	char *map = mmap(0, textsize, PROT_READ | PROT_WRITE, MAP_SHARED, archivo, 0);
	close(archivo);

	//memcpy(map, text, strlen(text));

    memset(map, 'B', 5 );

	readline("");

	msync(map, textsize, MS_SYNC);

	munmap(map, textsize);

	return 0;
	*/
//}*/

