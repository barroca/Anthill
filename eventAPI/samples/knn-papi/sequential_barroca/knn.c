#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define ROW_SIZE 200
#define COL_SIZE 10

#define MAXLINESIZE 1024




int main(int argc, char ** argv)
{
#pragma ANTHILL
	char trainFileName [100];
	char testFileName [100];
	char line[MAXLINESIZE];
	char * token;
	float trainData[ROW_SIZE][COL_SIZE];
	float testData[ROW_SIZE][COL_SIZE];
	int trainClass[ROW_SIZE];
	int tempClass[ROW_SIZE];
	float distance[ROW_SIZE];
	float final_distance[ROW_SIZE];
	float temp_distance;
	float temp1;
	int testClass[ROW_SIZE];
	int index[ROW_SIZE];
	int t;
	int k;
	int l;
#pragma INDUCTION i
	int i;
#pragma INDUCTION j
	int j;
#pragma IGNORE lixo
	int lixo;
	FILE * Arq;
	int nu_points;
	int nu_test;
	int nu_dimentions;
	int temp_dimentions;
	int iteration;
	int aux1;
	int auxClass1;

	int aux2;
	int auxClass2;
	int aux3;
	int auxClass3;

	temp1=0.0;
	aux1=0;
	auxClass1=0;
	aux2=0;
	auxClass2=0;
	aux3=0;
	auxClass3=0;
	trainFileName[0]=0;
	testFileName[0]=0;
	trainData[0][0]=0;
	testData[0][0]=0;
	trainClass[0]=0;
	tempClass[0]=0;
	distance[0]=0;
	final_distance[0]=0;
	testClass[0]=0;
	index[0]=0;
	t=0;

	lixo=0;
	line[0]=0;
	k=0;
	l=0;
	iteration=0;
	i=0;
	j=0;
	temp_distance=0.0;
	Arq=NULL;
	token=NULL;
	nu_points=0;
	nu_test=0;
	nu_dimentions=0;
	temp_dimentions=0;



	if(argc<4){
		printf("Usage: %s <train_file> <test_file> <k>\n",argv[0]);
		exit(0);
	}
	strcpy(trainFileName,argv[1]);
	strcpy(testFileName,argv[2]);
	k=atoi(argv[3]);

#ifdef DEBUG
	printf("Arguments:\n\ttrain: %s\n\ttest: %s\n\tk: %d\n",trainFileName,testFileName,k);
#endif	
#pragma READ BEGIN
	{
		if ( (Arq = fopen(trainFileName,"r+") ) ){
			nu_points =0;
			nu_dimentions =0;
			temp_dimentions = 0;
			for(i=0;(!feof(Arq));i=0){
				line[0]='\0';
				fgets(line,MAXLINESIZE,Arq);
				l = strlen(line);
				//remove \n char
				if(line[l-1]=='\n')l--;
				line[l]='\0';

#ifdef DEBUG
				printf("Line read (size %d): %s\n",l,line);
#endif

				if(l>0){
					token=strtok(line," ");
					for(i=0;(token!=NULL);i=0){
						trainData[nu_points][temp_dimentions]=atof(token);
#ifdef DEBUG
						printf("token read %s\n",token);
#endif
						temp_dimentions=temp_dimentions+1;
						token=strtok(NULL," ");

					}
					nu_dimentions=temp_dimentions-1;
					trainClass[nu_points]=(int)trainData[nu_points][nu_dimentions];
#ifdef DEBUG
					printf("Class %d\n",trainClass[nu_points]);
					printf("Nu Dimensoes %d\n",nu_dimentions);
#endif

					nu_points=nu_points+1;

					temp_dimentions=0;
				}
				lixo=lixo+1;






			}

#ifdef DEBUG
			printf("nu_points %d\n",nu_points);
#endif


		}else{
			printf("Cannot open TrainFile: %s\n",trainFileName);
			exit(0);
		}
		fclose(Arq);
		if ( (Arq = fopen(testFileName,"r+") ) ){
			nu_test=0;
			temp_dimentions=0;
			for(i=0;(!feof(Arq));i=0){
				line[0]='\0';
				fgets(line,MAXLINESIZE,Arq);
				l = strlen(line);
				//remove \n char
				if(line[l-1]=='\n')l--;
				line[l]='\0';
#ifdef DEBUG
				printf("Line read (size %d): %s\n",l,line);
#endif
				if(l>0){
					token=strtok(line," ");
					for(i=0;(token!=NULL);i=0){
						testData[nu_test][temp_dimentions]=atof(token);
#ifdef DEBUG
						printf("token read %s\n",token);
#endif
						temp_dimentions=temp_dimentions+1;
						
						token=strtok(NULL," ");

					}
					if(temp_dimentions!=nu_dimentions){
						printf("Error, wrong number of dimensions on test file: %s\n",testFileName);
						exit(0);
					}
					temp_dimentions=0;
					nu_test=nu_test+1;
				}
				lixo=lixo+1;
			}
#ifdef DEBUG
			printf("nu_test %d\n",nu_test);
#endif



		}else{
			printf("Cannot open TestFile: %s\n",testFileName);
			exit(0);
		}
		fclose(Arq);
	}


	iteration=0;
	while(iteration<nu_test){
		for(i=0;i<nu_points;i++){
			temp_distance=0.0;
			for(j=0;j<nu_dimentions;j++){
				temp_distance+=pow((trainData[i][j]-testData[iteration][j]),2);
#ifdef DEBUG
				printf("temp distance is %f\n",temp_distance);
#endif
			}
			distance[i]=pow(temp_distance,0.5);
#ifdef DEBUG
				printf("distance is %f\n",distance[i]);
#endif

			index[i]=i;
		}
		for(i=0;i<nu_points;i++){
			for(j=i+1;j<nu_points;j++){
				if(distance[j]<distance[i]){
					temp1=distance[i];
					distance[i]=distance[j];
					distance[j]=temp1;
					t=index[i];
					index[i]=index[j];
					index[j]=t;
				}
				lixo=lixo+1;
			}
		}
		for (i=0;i<ROW_SIZE;i++){
			tempClass[i]=0;
		}
		for(i=0;i<k;i++){
			aux1=index[i];
			auxClass1 = trainClass[aux1];
			tempClass[auxClass1]=0;
		}

		for(i=0;i<k;i++){
#ifdef DEBUG
			printf("Class %d\n",trainClass[index[i]]);
#endif
			aux2=index[i];
			auxClass2=trainClass[aux2];
			tempClass[auxClass2]=tempClass[auxClass2]+1;
		}
		for(i=0;i<ROW_SIZE;i++){
			auxClass3 = testClass[iteration];
			if(tempClass[i]>tempClass[auxClass3]){
				testClass[iteration]=i;
			}
				lixo=lixo+1;

		}

		final_distance[iteration]=distance[0];

		iteration=iteration+1;
	}
	
#pragma WRITE BEGIN
	{
		for(i=0;i<nu_test;i++){
			printf("Distance: %f\tClass: %d\n",final_distance[i],testClass[i]);
			
		}
	}


	return 0;
}
