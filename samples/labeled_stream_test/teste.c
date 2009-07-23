#include <stdio.h>
#include <stdlib.h>

//	void *label;

#define	BLABLABLA	7
// Argumentos
// 1 -> label
// 2 -> lablesize
int hash(void *label, int size){
  	int dest, lixo;
	char *labelchar = (char*)label;
	printf("label = %s\n", (char *)label);
	sscanf(label, "%d %d", &dest, &lixo);
	
	printf("%d %d", dest, lixo);  
	return dest;
}

// Argumentos
// 1 -> mensage
// 2 -> mensage size
// 3 -> label
// Retorna o tamanho do label
int getlabel(void *msg, int size, void *label){
	*(void **)label = *(void**)msg;
	
//	printf("%s\n",*(char **)msg);
//	printf("%s\n",*(char **)label);
	return 2;
}

int main(){
  	void *label1;
	int size, rethash, dest, lixo;
	char *teste = (char *) malloc(sizeof(char) * 100);

	sprintf(teste, "%d %d", 1, 10);

	printf("teste = %s\n", teste);
	size = 100;

	printf("size = %d\n", size);
	size = getlabel(&teste, size, &label1);
	printf("getlabel = %d\n", size);
	printf("Label = %s\n", label1);

	
	rethash = hash(label1,getlabel(teste, size, label1));
	printf("hash = %d\n", rethash);
	printf("Alou mamae\n");
	return 0;
}
