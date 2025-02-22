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
void *countOccurence(void* arg)
{
	int fd;
	if((fd = open(FICHIER,O_RDONLY)) == -1)
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
int main()
{
	pthread_t thread;
	int *retThread, ret;
	char word[40];
	if((ret = pthread_create(&thread, NULL, countOccurence, NULL) )!= 0)
	{
		printf("Erreur lors de la création du thread\n");
		return ret;
	}

	if((ret = pthread_join(thread, (void**)&retThread))!=0)
	{
		printf("Erreur lors de l'attente du thread\n");
		return ret;
	}
	printf("\nNombre d'occurences de %s : %d\n", WORD,*retThread);

	free(retThread);
	return 0;
}