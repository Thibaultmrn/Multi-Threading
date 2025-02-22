#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
//
#define FICHIER1 "Serveur.cpp"
#define FICHIER2 "windowclient.cpp"
#define FICHIER3 "Caddie.cpp"
#define FICHIER4 "applichorairewindow.cpp"
#define WORD "break"
//gcc -o Exercice1 Exercice1.c -pthread
void *countOccurence1(void* arg);
void *countOccurence2(void* arg);
void *countOccurence3(void* arg);
void *countOccurence4(void* arg);

int main()
{
	pthread_t thread[4];
	int *retThread[4], ret;

	if((ret = pthread_create(&thread[0], NULL, countOccurence1, NULL) )!= 0)
	{
		printf("Erreur lors de la création du thread\n");
		return ret;
	}

	if((ret = pthread_create(&thread[1], NULL, countOccurence2, NULL) )!= 0)
	{
		printf("Erreur lors de la création du thread\n");
		return ret;
	}

	if((ret = pthread_create(&thread[2], NULL, countOccurence3, NULL) )!= 0)
	{
		printf("Erreur lors de la création du thread\n");
		return ret;
	}

	if((ret = pthread_create(&thread[3], NULL, countOccurence4, NULL) )!= 0)
	{
		printf("Erreur lors de la création du thread\n");
		return ret;
	}

	for(int i = 0; i < 4; i++)
	{
		if((ret = pthread_join(thread[i], (void**)&retThread[i]))!=0)
		{
			printf("Erreur lors de l'attente du thread\n");
			return ret;
		}
	}
    for (int i = 0; i < 4; i++) 
    {
        printf("\nOccurrences du mot '%s' dans '%d' : %d\n", WORD, i +1, *retThread[i]);
        free(retThread[i]);
    }
	return 0;
}
void *countOccurence1(void* arg)
{
	int fd;
	if((fd = open(FICHIER1,O_RDONLY)) == -1)
	{
		printf("Erreur lors de l'ouverture du fichier\n");
	}

	size_t word = strlen(WORD);
	char buffer[word + 1];
	buffer[word] = '\0';
	
	off_t i = 0;
	ssize_t bytes_read;
	int *count = malloc(sizeof(int));
	if (count == NULL) 
	{
        printf("Erreur d'allocation mémoire");
        close(fd);
        pthread_exit(NULL);
    }
	*count = 0;
	while(1)
	{
		printf("*\n");
		lseek(fd,i,SEEK_SET);
		bytes_read = read(fd,buffer, word);
		if (bytes_read < (ssize_t)word) break;

		if(strcmp(buffer, WORD)==0)
		{
			(*count)++;
		}
		i++;
	}
    close(fd);
    pthread_exit(count);

}

void *countOccurence2(void* arg)
{
	int fd;
	if((fd = open(FICHIER2,O_RDONLY)) == -1)
	{
		printf("Erreur lors de l'ouverture du fichier\n");
	}

	size_t word = strlen(WORD);
	char buffer[word + 1];
	buffer[word] = '\0';
	
	off_t i = 0;
	ssize_t bytes_read;
	int *count = malloc(sizeof(int));
	if (count == NULL) 
	{
        printf("Erreur d'allocation mémoire");
        close(fd);
        pthread_exit(NULL);
    }
	*count = 0;
	while(1)
	{
		printf("\t");
		printf("*\n");
		lseek(fd,i,SEEK_SET);
		bytes_read = read(fd,buffer, word);
		if (bytes_read < (ssize_t)word) break;

		if(strcmp(buffer, WORD)==0)
		{
			(*count)++;
		}
		i++;
	}
    close(fd);
    pthread_exit(count);

}

void *countOccurence3(void* arg)
{
	int fd;
	if((fd = open(FICHIER3,O_RDONLY)) == -1)
	{
		printf("Erreur lors de l'ouverture du fichier\n");
	}

	size_t word = strlen(WORD);
	char buffer[word + 1];
	buffer[word] = '\0';
	
	off_t i = 0;
	ssize_t bytes_read;
	int *count = malloc(sizeof(int));
	if (count == NULL) 
	{
        printf("Erreur d'allocation mémoire");
        close(fd);
        pthread_exit(NULL);
    }
	*count = 0;
	while(1)
	{
		printf("\t\t");
		printf("*\n");
		lseek(fd,i,SEEK_SET);
		bytes_read = read(fd,buffer, word);
		if (bytes_read < (ssize_t)word) break;

		if(strcmp(buffer, WORD)==0)
		{
			(*count)++;
		}
		i++;
	}
    close(fd);
    pthread_exit(count);

}

void *countOccurence4(void* arg)
{
	int fd;
	if((fd = open(FICHIER4,O_RDONLY)) == -1)
	{
		printf("Erreur lors de l'ouverture du fichier\n");
	}

	size_t word = strlen(WORD);
	char buffer[word + 1];
	buffer[word] = '\0';
	
	off_t i = 0;
	ssize_t bytes_read;
	int *count = malloc(sizeof(int));
	if (count == NULL) 
	{
        printf("Erreur d'allocation mémoire");
        close(fd);
        pthread_exit(NULL);
    }
	*count = 0;
	while(1)
	{
		printf("\t\t\t");
		printf("*\n");
		lseek(fd,i,SEEK_SET);
		bytes_read = read(fd,buffer, word);
		if (bytes_read < (ssize_t)word) break;

		if(strcmp(buffer, WORD)==0)
		{
			(*count)++;
		}
		i++;
	}
    close(fd);
    pthread_exit(count);

}