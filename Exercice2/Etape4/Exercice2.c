#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <signal.h> 

//gcc -o Exercice2 Exercice2.c -pthread
void cleanupMaster(void* param);
void handlerSignal(int sig);
void handlerSIGUSR1(int sig);
void* slaveThread(void* param);
void* masterThread(void* param); 
struct sigaction sigAct;

int main()
{
    pthread_t slaves[4], threadMaster;
    int ret;
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_SETMASK, &mask, NULL);

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



    for (int i = 0; i < 4; i++) 
    {
        pthread_join(slaves[i], NULL);
    }
    printf("Tous les threads Slaves sont terminés, annulation du thread Master\n");
    pthread_cancel(threadMaster);

    pthread_join(threadMaster, NULL);

    printf("Thread principal terminé\n");
    return 0;
}
void* masterThread(void* param)
{
    printf("Thread Master %lu en attente du signal SIGINT\n", pthread_self());
    pthread_cleanup_push(cleanupMaster, NULL);
    while(1)
    {
        pause();
    }
    pthread_cleanup_pop(1);
}
void* slaveThread(void* param)
{
    sigemptyset(&sigAct.sa_mask);
    sigAct.sa_handler = handlerSIGUSR1;
    sigaction(SIGUSR1, &sigAct, NULL);

    printf("Thread %u en attente du signal SIGUSR1\n", pthread_self());
    pause();
    return NULL;
}

void handlerSignal(int sig)
{
    printf("Thread %lu a reçu le signal SIGINT et va terminer.\n", pthread_self());
    kill(getpid(),SIGUSR1); 
}

void handlerSIGUSR1(int sig)
{
    printf("Thread Slave %lu a reçu SIGUSR1 et va terminer.\n", pthread_self());
    pthread_exit(NULL);
}

void cleanupMaster(void* param)
{
    printf("Thread Master %lu se termine proprement après annulation.\n", pthread_self());
}