#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void Busca(FILE *,char *);

int main(int argc,char *argv[]){
	
	int i;
	FILE *fd;

	if(argc<2){
		printf("UVagrep: searchterm [file ...]\n");
		exit(1);
	}
	
	//Primer caso:no hay ficheros
	if(argc==2){
		fd=stdin;
		Busca(fd,argv[1]);
		exit(0);
	}

	//Segundo caso:hay  uno o vario ficheros
	for(i=2;i<argc;i++){
		fd=fopen(argv[i],"r");
		if(fd==NULL){
			printf("UVagrep: cannot open file\n");
			exit(1);
		}
		Busca(fd,argv[1]);
		fclose(fd);
	}  //sig.fichero

}


void Busca(FILE *Fich,char *termino){
	//variables
	char * linea;
	size_t tam;
	linea=NULL;
	tam=0;
	int valor;

	//bucle principal
	while((valor=getline(&linea,&tam,Fich))!=-1){
	//	printf("Linea :%s",linea);
		
		//Busca termino en linea
		if(strstr(linea,termino)!=NULL){
			printf("%s",linea);
		}

		//Lo piede getline:
		free(linea);
		tam=0;
	}
}
