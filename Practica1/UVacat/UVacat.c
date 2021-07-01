#include <stdlib.h>
#include <stdio.h>

//#define MAXLINEA 4096
//void MuestraFich(FILE *);


int main(int argc,char *argv[]){

	FILE *fd;
	char *NombreFichero;
	int ret;

	if(argc==1){
		exit(0);
	}

	for(int i=1;i<argc;i++){
		NombreFichero=argv[i];
		fd=fopen(NombreFichero,"r");
		if(fd==NULL){
			printf("UVacat: no puedo abrir fichero\n");
			exit(1);
		}
		//MuestraFich(fd);
		//fclose(fd);
		while((ret=fgetc(fd))!=EOF){
			printf("%c",ret);
		}
		fclose(fd);
	}
	return 0;
}
	/*
	 * void MuestraFich(FILE *FDesc){
	 * 	char Almacen[MAXLINEA];
	 * 	while(fgets(Almacen,MAXLINEA,FDesc)!=NULL)
	 * 		printf("%s",Almacen);
	 * }		
	 */

	//MEMORIA ESTATICA ->
	
	/*
	 * void MuestraFich(FILE *FDesc){
	 * 	char *Almacen;
	 * 	Almacen=(char *)malloc(TAMLINEA);
	 * 	while(fgets(Almacen,MAXLINEA,FDesc)!=NULL) printf("%s",Almacen);
	 * 	free(Almacen);
	 * }
	 */


