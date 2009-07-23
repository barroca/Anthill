#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#define MAXLINE 20
#define RSIZE 200
#define CSIZE 10
ssize_t readline(int fd,void *vptr,size_t maxlen);
int main(int argc, char ** argv)
{
	int fd=0;
	int trr=0;
	int ter=0;
	int j=0;
	int trc=0;
	int tec=0;
	int i=0;
	int k=0;
	int l=0;
	char buff[MAXLINE];
	char filename[20];
	char *token;
	ssize_t n;
	int trainData[RSIZE][CSIZE];
	int testData[RSIZE][CSIZE];
	int one[CSIZE];
	int t;
	int index[CSIZE];
	float distance[CSIZE];
	float temp;
	printf("enter the training data file name\n");
	scanf("%s",filename);
	for(i=0;i<RSIZE;i++)
		for(j=0;j<CSIZE;j++)
			trainData[i][j]=0;
	for(i=0;i<RSIZE;i++)
		for(j=0;j<CSIZE;j++)
			testData[i][j]=0;

		for(j=0;j<CSIZE;j++)
			distance[j]=0;

	fd=open(filename,O_RDONLY,0);

	do {
		if (buff[n-1] == '\n')
			n--;	/*delete new line from the */
		buff[n] = '\0';					/*delete new line from the */
		token=strtok(buff,",");
		if(token==NULL)
			break;
		for(j=0;j<CSIZE;j++)
		{
			trainData[i][j]=atoi(token);
			token=strtok(NULL,",");
			if(token==NULL)
				break;
		}

		i++;
	}while((n=readline(fd,buff,MAXLINE)) > 0);
	close(fd);
	trr=i;
	trc=j+1;
	printf("row:%d col:%d\n",trr,trc);
	for(k=0;k<trr;k++)
	{
		for(l=0;l<trc;l++)
			printf("%d ",trainData[k][l]);
		printf("\n");
	}


	printf("enter the test data file name\n");
	scanf("%s",filename);
	i=0,n=0;
	fd=open(filename,O_RDONLY,0);

	do{
		if (buff[n-1] == '\n')
			n--;
		buff[n] = '\0';
		token=strtok(buff,",");
		if(token==NULL)
			break;
		for(j=0;j<CSIZE;j++)
		{
			testData[i][j]=atoi(token);

			token=strtok(NULL,",");
			if(token==NULL)
				break;
		}
		i++;
	}while ((n=readline(fd,buff,MAXLINE)) > 0);
	close(fd);
	ter=i;
	tec=j+1;
	printf("row:%d col:%d\n",ter,tec);
	for(k=0;k<trr;k++)
	{
		for(l=0;l<trc;l++)
			printf("%d ",testData[k][l]);
		printf("\n");
	}

	for(i=0;i<trr;i++)
     		index[i]=i;

	j=0;
	while(j<ter){
		for(k=0;k<ter;k++)
		{
			one[k]=testData[j][k];

		}
		for(i=0;i<trr;i++)
		{
			temp=0;
			for(k=0;k<(tec-1);k++)
			{
				temp+=pow((trainData[i][k]-one[k]),2);
				printf("temp is %f\n",temp);
			}
			distance[i]=pow(temp,0.5);
		}
		for(i=0;i<trr;i++)
    		{
      			for(k=i+1;k<trr;k++)
      			{
        			if(distance[k]<distance[i])
        			{
          				temp=distance[k];
          				distance[k]=distance[i];
          				distance[i]=temp;

          				t=index[k];
          				index[k]=index[i];
          				index[i]=t;
        			}

      			}
		}
		{
		for(l=0;l<(tec-1);l++)
			printf("%f  ",distance[l]);
		printf("\n");
		}	
		j++;
	}
	return 0;

}
ssize_t readline(int fd, void *vptr, size_t maxlen)
{
	char *ptr, c;
	ssize_t n, rc;

	ptr = vptr;
	for (n = 1; n < maxlen; n++)
	{
        		if ( (rc = read(fd,&c,1)) == 1)
            		{
                		*ptr++ = c;

                		if (c == '\n')
				{
                    			break;
				}
			}
			else if (rc == 0)
			{
				*ptr = 0;
				return(n-1);
			}

	}
	*ptr = 0;
	return(n-1);
}

