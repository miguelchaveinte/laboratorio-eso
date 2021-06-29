#include <stdio.h>
#include <stdlib.h>

#define INICIO 1
#define CUENTA 0

void Comprime(FILE *);

//externas:mantienen el estado entre llamadas a Comprime

        int Estado;
        char CaracterR;
        int NumR;

int main(int argc,char *argv[]){

	//Vars
	int i;
	FILE *fd;

	if(argc<2){
		printf("UVazip: file1 [file2 ...]\n");
		exit(1);
	}

	Estado=INICIO;
	//Recorro cada fichero
	for(i=1;i<argc;i++){
		
		fd=fopen(argv[i],"r");
		if(fd==NULL){
			printf("UVagrep: cannot open file\n");
			exit(1);
		}
		Comprime(fd);
		fclose(fd);

	}//FIN DEL ULTIMO CARACTER
	//printf("%d-%c ",NumR,CaracterR);
        fwrite(&NumR,sizeof(int),1,stdout);
        printf("%c",CaracterR);


}

void Comprime(FILE *Fich){
	
	//vars
	char * linea;
	size_t valor;
	ssize_t tamanno;

	int i;

	linea=NULL;
	valor=0;
	NumR=0;

	while((tamanno=getline(&linea,&valor,Fich))!=-1){
		for (i=0;i<tamanno;i++){
			if(Estado==CUENTA){
				if(CaracterR==linea[i]){
					NumR++;
				}
				else{//Salimos:imprimir salida codificada y reiniciar Estado
					//printf("%d-%c ",NumR,CaracterR);
					fwrite(&NumR,sizeof(int),1,stdout);
					printf("%c",CaracterR);
					NumR=1;
					CaracterR=linea[i];
					Estado=CUENTA;
				}
			}
			else{//INICIO
				NumR=1;
				CaracterR=linea[i];
				Estado=CUENTA;
			}
		}//for dentro linea

		//SIempr antes de leeer la siguiente linea
		free(linea);
		valor=0;
	}//while entre lineas

}
