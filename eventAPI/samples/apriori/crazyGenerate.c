#include <stdio.h>
#include <stdlib.h>

//linhas atributo 10
int main(int argc, char **argv){
	FILE *outPut =NULL;
	int i, j;
	if(argc != 5){
		printf("%s <#linhas> <#atributos> <#prob1(1-100)> <fileName>", argv[0]);	
		exit(1);
	}
	outPut = fopen(argv[4], "w+");
	if(outPut == NULL){
		printf("Could not open file %s to write\n", argv[4]);	
	}
	int linha =  atoi(argv[1]);
	int atributos = atoi(argv[2]);
	int prob = atoi(argv[3]);
	int randValue;
	for(i = 0; i < linha; i++){
		for(j = 0; j < atributos; j++){
			randValue = rand();
			if((randValue%100) > prob){
				fprintf(outPut,"0");
			}else{
				fprintf(outPut,"1");	
			}
		}
		fprintf(outPut,"\n");
	}
}


