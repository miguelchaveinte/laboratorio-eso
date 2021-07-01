/*
 * MIGUEL CHAVEINTE GARCIA (migchav)
 * X2- ESO
 * PRACTICA 2 - UVashell --- EXAMEN
 * 25-03-2021
 * FUNCIONA:
 * 	Basic Shell : SI
 * 	Built-in Commands : SI
 * 	Redirection : SI
 * 	Parallel Commands: SI
 * 	Program Errors: SI
 * 	Miscellaneous Hints: SI
 * 	Tests : SI (Todos los tests pasan)
 */



#include<stdio.h>
#include<stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include<string.h>
#include<sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>


#undef DEPURA //define  o undef

#define SI 0
#define NO 1

#define MAXARGS 512

//Mensaje error programa
char error_message[30]= "An error has occurred\n";

//Funciones
void ejecutaParalelo(char *);
void ejecutaRedireccion(char *);
int numCaracteres(char *[]);
void ejecutaComando(char *,char*,bool);
void quitarEspacio(char [], char []);
void strip(char *s);

//Funcion examen
void eliminarDolar(char *);

//main
int main(int argc,char *argv[]){
	
	FILE *fd;//Descriptor de la entrada de UVash
	int Interactivo;

	char * Comando;
	size_t Tam;
	ssize_t Ret;

	if(argc>2){  //Como maximo dos argumentos de entrada.
		fprintf(stderr, "%s", error_message);
		exit(1);
	}
	//argc==1 o argc==2
	if(argc==2){		//hay fichero de entrada
		Interactivo=NO;
		fd=fopen(argv[1],"r");
		if(fd==NULL){
			fprintf(stderr, "%s", error_message);
			exit(1);
		}
	}

	else{
		Interactivo=SI;
		fd=stdin;
	}

	//fd vale stdin o el descriptor del fichero

	//Bucle principal
	//
	while(1){
		if(Interactivo==SI)
			printf("UVash> ");

		//Preparacion de getline
		Comando=NULL;
		Tam=0;
		Ret=getline(&Comando,&Tam,fd);
		if(Ret==-1)
			exit(0);

		Comando[Ret-1]='\0';
		ejecutaParalelo(Comando);

		//Libero memoria Comando asignada por getline
		free(Comando);	
	}

}//main


//Funcion divide por & para diferenciar los comandos paralelos

void ejecutaParalelo(char * Comando){

	//Comprobar comandos paralelos
	
	char Separador[]="&";
	char *cadenaSep;
	char *inputComando[MAXARGS];
	int j;

	cadenaSep=strtok(Comando,Separador);
	j=0;
	while(cadenaSep!=NULL){
		inputComando[j]=cadenaSep;
		j++;

		cadenaSep=strtok(NULL,Separador);
	}
	inputComando[j]=cadenaSep;


	// Solo recibimos un & como comando
	if(j==0 && strstr(Comando,Separador)!=NULL) fprintf(stderr, "%s", error_message);

	else{

		if(inputComando[1]==NULL){ //NO HAY COMANDO PARALELO
			if(inputComando[0]==NULL){} //Recibimos un espacio 
			else{
				ejecutaRedireccion(inputComando[0]); 
				wait(NULL);	//Esperamos a que acabe
			}

		}

		else{
			int numeroComandos=numCaracteres(inputComando); //Contamos cuantos comandos paralelos hay 
			for(int i=0;i<numeroComandos;i++){ // Lanzamos cada uno de los comandos separados
				ejecutaRedireccion(inputComando[i]);
			} 
			for (int i=0;i<numeroComandos;i++)
				wait(NULL);  // Esperamos a que acaben todos los comandos
		}
	}

}


// Funcion que divide cada comando si este contiene una redireccion a un fichero de salida

void ejecutaRedireccion(char * Comando){

	
	//comprobar si hay redireccion
	char Redireccion[]=">";
	char *cadenaIndependiente;
	char *cadenaRedireccion[MAXARGS];
	int  i;

	char caracterDolar[]="$"; //MODIFICACION EXAMEN	
	char comandoTime[1024]="time ";

	bool tieneRedireccion=false;

	
	char *nuevoComando=(char *) malloc (sizeof(char)*MAXARGS);
	
	//MODIFICACION EXAMEN	
	//Si tiene el caracter dolar le quitamos y concatenamos con el string time
	if(strstr(Comando,caracterDolar)!=NULL){
		eliminarDolar(Comando);
		strcat(comandoTime,Comando);
		strcpy(nuevoComando,comandoTime);
	}
	else//Sino copia el comando
		strcpy(nuevoComando,Comando);
	//*/
	
	
	if(strstr(nuevoComando,Redireccion)!=NULL) tieneRedireccion=true;	// Comprobar que tieneel caracter > de redireccion

	cadenaIndependiente=strtok(nuevoComando,Redireccion);
	i=0;
	while(cadenaIndependiente!=NULL){
		cadenaRedireccion[i]=cadenaIndependiente;
		i++;

		cadenaIndependiente=strtok(NULL,Redireccion);
	}
	

	cadenaRedireccion[i]=cadenaIndependiente;

	
	if(i>2){ // mas de un caracter '>' en el mismo comando
		fprintf(stderr, "%s", error_message);
	}

	else{
		if(cadenaRedireccion[1]==NULL){ //no hay redireccion
			if(tieneRedireccion) fprintf(stderr, "%s", error_message); //error porque contenia el caracter >
			else ejecutaComando(cadenaRedireccion[0],NULL,false);
		}
		else{
			ejecutaComando(cadenaRedireccion[0],cadenaRedireccion[1],true);
		}
	}
	free(nuevoComando);
}

// Funcion que ejecuta el comando pasado y redirigirlo a un fichero si es necesario
void ejecutaComando(char * Comando,char * fich, bool hayRedireccion){

	char Delimitador[]= " ";
        char *Palabra;
        char *Argumentos[MAXARGS];
        int i;
        char * Ejecutable;
        pid_t pid;
        int RetExec;
	int fd;
	int intentoFich;
	int intentoError;


	strip(Comando); // quitamos tabuladores en el comando



	//parse
	//Primera vez fuera del bucle , distinta invocacion ver man
	Palabra=strtok(Comando,Delimitador);
	i=0;
	while(Palabra!=NULL){
		Argumentos[i]=Palabra;
		i++;

		Palabra=strtok(NULL,Delimitador);
	}//while
	Argumentos[i]=Palabra; //NULL


	if(Argumentos[0]!=NULL){ 

		//COMPROBACION QUE EL COMANDO  ES EXIT
		if(strcmp(Argumentos[0],"exit")==0){
			if(i!=1) fprintf(stderr, "%s", error_message); // si exit tiene algun argumento detras
			exit(0);
		}


		//COMPROBACION QUE EL COMANDO CD
		if(strcmp(Argumentos[0],"cd")==0){
			if(chdir(Argumentos[1])!=0) fprintf(stderr, "%s", error_message);// si hay un error al cambiar de directorio
		}
		
		//MODIFICACION EXAMEN	
		//EXAMEN: COMANDO HazDir que nos crea un directorio nuevo llamado DirectorioExamen
		else if(strcmp(Argumentos[0],"HazDir")==0){
			if(mkdir("./DirectorioExamen",0755)!=0) fprintf(stderr, "%s", error_message);
		}

		else{
		//Comprobacion por compilacion condicional
		#ifdef DEPURA
		i=0;
		while(Argumentos[i]!=NULL){	
			printf("Argumentos (%d)=%s\n",i,Argumentos[i]);
			i++;
		}
		#endif
		Ejecutable=Argumentos[0];
	
		//Creo un proceso hijo
		pid=fork();
		if(pid<0){ // error al crear un hijo
        	        fprintf(stderr, "%s", error_message);
               		exit(1);
        	}

		else if(pid==0){
			if(hayRedireccion){
				char sinEspacios [strlen(fich)];
				quitarEspacio(fich,sinEspacios); // quitamos los espacios de delant y de detras de la ruta del fichero
				char espacio [] =" ";
				if(strstr(sinEspacios,espacio)!=NULL) fprintf(stderr, "%s", error_message);  //si hay un espacio entre medias de  la ruta del fichero 
				fd=open(sinEspacios,O_WRONLY | O_CREAT | O_TRUNC, 0777);
				if (fd==-1){  // problemas al abrir el fichero 
					fprintf(stderr, "%s", error_message);
					exit(1);
				}
				intentoFich=dup2(fd,1);
				intentoError=dup2(fd,2);
				if(intentoFich==-1 || intentoError==-1){ // error al abrir alguno de los descriptores de ficheros
					fprintf(stderr, "%s", error_message);
					exit(1);
				}
			}
			RetExec=execvp(Ejecutable,Argumentos); //ejecucion del comando
			if(RetExec==-1){  // error al ejecutar el comando
				fprintf(stderr, "%s", error_message);
			}
			close(fd); // cierre fichero
			exit(0);
		}
		}//else
	}//if !=null
}//Ejecuta


//Funcion que cuenta el numero de elementos del array que se nos pasa como argumento
int numCaracteres(char *array[]){
	int contador=0;
	for (int i=0;array[i]!=NULL;i++){
		contador++;
	}
	return contador;

}

//Funcion que nos quita los espacios en blanco de delante y detras de una cadena
void quitarEspacio(char cadenaOrigen[], char cadenaNueva[]){
	int a,b,tam;
	tam=strlen(cadenaOrigen);
	b=0;
	bool caracter=true;
	for(a=0;a<tam && caracter;a++){
		if(cadenaOrigen[a]!=' '){
			cadenaNueva[b]=cadenaOrigen[a];
			b++;
			caracter=false;
		}
	}

	for(a=a;a<tam;a++){
		cadenaNueva[b]=cadenaOrigen[a];
                b++;

	}
	cadenaNueva[b]='\0';
	if(cadenaNueva[b-1]==' '){
		b=b-1;
		while(cadenaNueva[b]==' '){
			cadenaNueva[b]='\0';
			b--;
		}
	}
}


//Funcion que elimina los tabuladores de la cadena dada
void strip(char *s) {
    char *p2 = s;
    while(*s != '\0') {
        if(*s != '\t' &&  *s!='\n') {
            *p2++ = *s++;
        } else {
            *p2++=' ';
            ++s;
        }
    }
    *p2 = '\0';
}

//Funcion que elimina el dolar de una cadena. TODO:Refactorizar con strip . no tiempo
void eliminarDolar(char *s){
    char *p2 = s;
    while(*s != '\0') {
        if(*s != '$') {
            *p2++ = *s++;
        } else {
            *p2++=' ';
            ++s;
        }
    }
    *p2 = '\0';

}

