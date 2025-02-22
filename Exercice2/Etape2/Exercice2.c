#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <signal.h> 

//gcc -o Exercice2 Exercice2.c -pthread
void handlerSignal(int sig);

void* slaveThread(void* param);
void* masterThread(void* param); 
struct sigaction sigAct;

int main()
{
    pthread_t slaves[4], threadMaster;
    int ret;
    sigset_t mask;

    sigemptyset(&sigAct.sa_mask);
    sigAct.sa_handler = handlerSignal;
    sigaction(SIGINT, &sigAct, NULL);

    ret = pthread_create(&threadMaster,NULL,masterThread, NULL);

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    for(int i =0; i < 4; i++)
    {
        ret = pthread_create(&slaves[i],NULL,slaveThread, NULL);
    }

    printf("Thread principal %u en attente du signal SIGINT\n", pthread_self());
    pause();

    pthread_exit(NULL);
    return 0;
}
void* masterThread(void* param)
{
    printf("Thread Master %lu en attente du signal SIGINT\n", pthread_self());
    while(1)
    {
        pause();
    }
}
void* slaveThread(void* param)
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