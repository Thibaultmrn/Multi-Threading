#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
//
#define FICHIER "Serveur.cpp"
#define WORD "return"
//gcc -o Exercice1 Exercice1.c -pthread

typedef struct
{
	char *fileName;
	char *word;
	int tab;
}ThreadData;

void *countOccurence(void* param);

int main()
{
	pthread_t thread[4];
    ThreadData threadData[4] = 
    {
        "Serveur.cpp", "break", 0,
        "windowclient.cpp", "break", 1,
        "Caddie.cpp", "break", 2,
        "applichorairewindow.cpp", "break", 3
    };

	int *retThread[4], ret;
	
	for(int i = 0; i < 4; i++)
	{
		if((ret = pthread_create(&thread[i], NULL, countOccurence, &threadData[i]) )!= 0)
		{
			printf("Erreur lors de la création du thread %d\n", i + 1);
			return ret;
		}
	}

	for(int i = 0; i<4;i++)
	{
		if((ret = pthread_join(thread[i], (void**)&retThread[i]))!=0)
		{
			printf("Erreur lors de l'attente du thread %d\n", i + 1);
			return ret;
		}
	}

    for (int i = 0; i < 4; i++) 
    {
        printf("\nOccurrences du mot '%s' dans '%s' : %d\n", threadData[i].word, threadData[i].fileName, *retThread[i]);
        free(retThread[i]);
    }
	return 0;
}

void *countOccurence(void* param)
{
	ThreadData *data = (ThreadData *)param;



	size_t word = strlen(data->word);
	char buffer[word + 1];
	buffer[word] = '\0';
	
	int i = 0;
	ssize_t bytes_read;
	int *count = malloc(sizeof(int));
	if (count == NULL) 
	{
        printf("Erreur d'allocation mémoire");
        pthread_exit(NULL);
    }
	*count = 0;
	while(1)
	{
		int fd;
		if((fd = open(data->fileName,O_RDONLY)) == -1)
		{
			printf("Erreur lors de l'ouverture du fichier\n");
			pthread_exit(NULL);
		}

		for (int i = 0; i < data->tab; i++) 
	    {
        	printf("\t");
    	}

		printf("*\n");


		lseek(fd,i,SEEK_SET);
		bytes_read = read(fd,buffer, word);
		close(fd);

		if (bytes_read < (ssize_t)word)break;

		if(strcmp(buffer, data->word)==0)
		{
			(*count)++;
		}
		i++;
	}

    pthread_exit(count);
}