#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <signal.h> 

//gcc -o Exercice2 Exercice2.c -pthread
void handlerSignal(int sig);

void* fctThread(void* param);
struct sigaction sigAct;

int main()
{
    pthread_t thread[4];
    int ret;

    sigemptyset(&sigAct.sa_mask);
    sigAct.sa_handler = handlerSignal;
    sigaction(SIGINT, &sigAct, NULL);

    for(int i =0; i < 4; i++)
    {
        ret = pthread_create(&thread[i],NULL,fctThread, NULL);
    }

    printf("Thread principal %u en attente du signal SIGINT\n", pthread_self());
    pause();

    pthread_exit(NULL);
    return 0;
}

void* fctThread(void* param)
{
    printf("Thread %u en attente du signal SIGINT\n", pthread_self());
    pause();
    return NULL;
}

void handlerSignal(int sig)
{
    printf("Thread %lu a reçu le signal SIGINT et va terminer.\n", pthread_self());
    pthread_exit(NULL);
}