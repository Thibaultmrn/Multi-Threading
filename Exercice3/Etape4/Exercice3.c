#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h> 

//gcc -o Exercice3 Exercice3.c -pthread

pthread_mutex_t mutexParam; 
pthread_mutex_t mutexCompteur;
pthread_cond_t condCompteur;
pthread_key_t cle;

int compteur = 0;

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
struct sigaction sigAct;

void* fctThread(void* param);
void handlerSIGINT(int sig);
void destructeur (void *p);

int main()
{
    pthread_t threads[4];
    sigset_t mask;

    pthread_mutex_init(&mutexParam, NULL);
    pthread_mutex_init(&mutexCompteur, NULL);
    pthread_cond_init(&condCompteur, NULL);
    pthread_key_create(&cle, destructeur);

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    for(int i=0; i < 4; i++)
    {
        pthread_mutex_lock(&mutexParam); 
        memcpy(&Param, &data[i], sizeof(DONNEE));
        pthread_mutex_unlock(&mutexParam);
        pthread_create(&threads[i],NULL, fctThread, (void*)&Param);
    }

    pthread_mutex_lock(&mutexCompteur);
    while(compteur >0)
    {
        pthread_cond_wait(&condCompteur, &mutexCompteur);
    }
    pthread_mutex_unlock(&mutexCompteur);

    printf("Thread principal terminé\n");

    return 0;
}

void* fctThread(void* param)
{
    DONNEE* info = (DONNEE*) param; 
    pid_t pid = getpid();
    pthread_t tid = pthread_self();

    sigemptyset(&sigAct.sa_mask);
    sigAct.sa_handler = handlerSIGINT;
    sigaction(SIGINT, &sigAct, NULL);

    char* nomThread = malloc(strlen(info->nom)+1);
    strcpy(nomThread, info->nom);
    pthread_setspecific(cle, nomThread);

    pthread_mutex_lock(&mutexCompteur);
    compteur++;
    pthread_mutex_unlock(&mutexCompteur);

    printf("Thread %d.%u lancé\n", pid,tid);
    printf("Nom : %s\n", info->nom);

    struct timespec ts;
    ts.tv_sec = info->nbSecondes;
    ts.tv_nsec = 0;

    nanosleep(&ts, NULL);

    printf("Thread %d.%u se termine\n", pid,tid);

    pthread_mutex_lock(&mutexCompteur);
    compteur--;
    if(compteur == 0)
    {
        pthread_cond_signal(&condCompteur);//reveil le thread princ
    }
    pthread_mutex_unlock(&mutexCompteur);

    pthread_exit(NULL);
}

void handlerSIGINT(int sig)
{
    char* nomThread = (char*)pthread_getspecific(cle);
    printf("Thread %d.%u s'occupe de <%s>\n", getpid(), pthread_self(), nomThread);

}

void destructeur (void *p)
{
    free(p);
}