#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

//gcc -o Exercice3 Exercice3.c -pthread

pthread_mutex_t mutexParam; 

typedef struct 
{  
    char nom[20]; 
    int nbSecondes; 
} DONNEE;

DONNEE data[] = { "MATAGNE",15, 
                    "WILVERS",10, 
                    "WAGNER",17, 
                    "QUETTIER",8, 
                    "",0 };
DONNEE Param;
void* fctThread(void* param);

int main()
{
    pthread_t threads[4];
    pthread_mutex_init(&mutexParam, NULL); 
    for(int i=0; i < 4; i++)
    {
        pthread_mutex_lock(&mutexParam); 
        memcpy(&Param, &data[i], sizeof(DONNEE));
 
        pthread_create(&threads[i],NULL, fctThread, (void*)&Param);
    }

    for(int i =0; i < 4; i++)
    {
        pthread_join(threads[i], NULL);
    }
    printf("Thread principal terminé\n");

    return 0;
}

void* fctThread(void* param)
{
    DONNEE* info = (DONNEE*) param; 
    pid_t pid = getpid();
    pthread_t tid = pthread_self();

    printf("Thread %d.%u lancé\n", pid,tid);
    printf("Nom : %s\n", info->nom);

    struct timespec ts;
    ts.tv_sec = info->nbSecondes;
    ts.tv_nsec = 0;
    pthread_mutex_unlock(&mutexParam);
    nanosleep(&ts, NULL);

    printf("Thread %d.%u se termine\n", pid,tid);

    pthread_exit(NULL);
}