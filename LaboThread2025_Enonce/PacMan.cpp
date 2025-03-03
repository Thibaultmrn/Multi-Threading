#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "GrilleSDL.h"
#include "Ressources.h"

// Dimensions de la grille de jeu
#define NB_LIGNE 21
#define NB_COLONNE 17

// Macros utilisees dans le tableau tab
#define VIDE         0
#define MUR          1
#define PACMAN       2
#define PACGOM       3
#define SUPERPACGOM  4
#define BONUS        5
#define FANTOME      6

// Autres macros
#define LENTREE 15
#define CENTREE 8

typedef struct
{
  int L;
  int C;
  int couleur;
  int cache;
} S_FANTOME;

typedef struct {
  int presence;
  pthread_t tid;
} S_CASE;

S_CASE tab[NB_LIGNE][NB_COLONNE];
int L = 15, C = 8, dir = GAUCHE;

pthread_mutex_t mutexParam;
struct sigaction sigAct;

void DessineGrilleBase();
void Attente(int milli);
void setTab(int l, int c, int presence = VIDE, pthread_t tid = 0);
void* fctThreadPacMan(void* param);
void* fctThreadEvent(void* param);
void handler_signal(int sig);
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc,char* argv[])
{
  EVENT_GRILLE_SDL event;
  sigset_t mask;
  struct sigaction sigAct;
  char ok;
  pthread_t threadPacMan, threadEvent;
  int ret;

  srand((unsigned)time(NULL));
  pthread_mutex_init(&mutexParam, NULL);

  // Ouverture de la fenetre graphique
  printf("(MAIN%p) Ouverture de la fenetre graphique\n",pthread_self()); fflush(stdout);
  if (OuvertureFenetreGraphique() < 0)
  {
    printf("Erreur de OuvrirGrilleSDL\n");
    fflush(stdout);
    exit(1);
  }

  DessineGrilleBase();

  sigemptyset(&sigAct.sa_mask);
  sigAct.sa_handler = handler_signal;
  sigaction(SIGINT, &sigAct, NULL);
  sigaction(SIGHUP, &sigAct, NULL);
  sigaction(SIGUSR1, &sigAct, NULL);
  sigaction(SIGUSR2, &sigAct, NULL);

  // Exemple d'utilisation de GrilleSDL et Ressources --> code a supprimer
 // DessinePacMan(17,7,GAUCHE);  // Attention !!! tab n'est pas modifie --> a vous de le faire !!!
  /*DessineChiffre(14,25,9);
  DessineFantome(5,9,ROUGE,DROITE);
  DessinePacGom(7,4);
  DessineSuperPacGom(9,5);
  DessineFantomeComestible(13,15);
  DessineBonus(5,15);*/
  pthread_create(&threadPacMan,NULL,fctThreadPacMan,NULL);
  pthread_create(&threadEvent,NULL,fctThreadEvent,&threadPacMan);

  pthread_join(threadEvent,NULL);
  //pthread_join(threadPacMan,NULL);

  ok = 0;
  while(!ok)
  {
    event = ReadEvent();
    if (event.type == CROIX) ok = 1;
    if (event.type == CLAVIER)
    {
      switch(event.touche)
      {
        case 'q' : ok = 1; break;
        case KEY_RIGHT : printf("Fleche droite !\n"); break;
        case KEY_LEFT : printf("Fleche gauche !\n"); break;
      }
    }
  }
  printf("Attente de 1500 millisecondes...\n");
  Attente(1500);
  // -------------------------------------------------------------------------
  
  // Fermeture de la fenetre
  printf("(MAIN %p) Fermeture de la fenetre graphique...",pthread_self()); fflush(stdout);
  FermetureFenetreGraphique();
  printf("OK\n"); fflush(stdout);

  exit(0);
}

//*********************************************************************************************
void Attente(int milli) {
  struct timespec del;
  del.tv_sec = milli/1000;
  del.tv_nsec = (milli%1000)*1000000;
  nanosleep(&del,NULL);
}

//*********************************************************************************************
void setTab(int l, int c, int presence, pthread_t tid) {
  tab[l][c].presence = presence;
  tab[l][c].tid = tid;
}

//*********************************************************************************************
void DessineGrilleBase() {
  int t[NB_LIGNE][NB_COLONNE]
    = { {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
        {1,0,1,1,0,1,1,0,1,0,1,1,0,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,1,1,0,1,0,1,1,1,0,1,0,1,1,0,1},
        {1,0,0,0,0,1,0,0,1,0,0,1,0,0,0,0,1},
        {1,1,1,1,0,1,1,0,1,0,1,1,0,1,1,1,1},
        {1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,1,1},
        {1,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,1},
        {0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0},
        {1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1},
        {1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,1,1},
        {1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1,1},
        {1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
        {1,0,1,1,0,1,1,0,1,0,1,1,0,1,1,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,1},
        {1,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,1},
        {1,0,0,0,0,1,0,0,1,0,0,1,0,0,0,0,1},
        {1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}};

  for (int l=0 ; l<NB_LIGNE ; l++)
    for (int c=0 ; c<NB_COLONNE ; c++) {
      if (t[l][c] == VIDE) {
        setTab(l,c);
        EffaceCarre(l,c);
      }
      if (t[l][c] == MUR) {
        setTab(l,c,MUR); 
        DessineMur(l,c);
      }
    }
}

//*********************************************************************************************

void* fctThreadPacMan(void* param)
{
  while(1)
  {
    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGHUP);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);


    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    pthread_mutex_lock(&mutexParam);
    setTab(L, C, VIDE);
    EffaceCarre(L, C);

    int newC = C, newL = L;
    switch(dir)
    {
      case GAUCHE : 
      newC--;
      break;

      case DROITE :
      newC++;
      break;

      case HAUT : 
      newL--;
      break;

      case BAS :
      newL++;
      break;
    }

    if (newL >= 0 && newL < NB_LIGNE && newC >= 0 && newC < NB_COLONNE) 
    {
      if (tab[newL][newC].presence != MUR) 
      {
        L = newL;
        C = newC;
      }
    }
    setTab(L, C, PACMAN, pthread_self());
    DessinePacMan(L, C, dir);

    pthread_mutex_unlock(&mutexParam);
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
    
    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    Attente(300);
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
  }

}

void* fctThreadEvent(void* param)
{
  EVENT_GRILLE_SDL event;
  pthread_t pacman_tid = *(pthread_t*)param;

  while(1)
  {
    event = ReadEvent();

    if(event.type == CROIX)
    {
      pthread_exit(NULL);
    }
    if(event.type == CLAVIER)
    {
      switch (event.touche)
      {
        case KEY_LEFT:
        pthread_kill(pacman_tid, SIGINT);
        break;

        case KEY_RIGHT:
        pthread_kill(pacman_tid, SIGHUP);
        break;
        case KEY_UP:
        pthread_kill(pacman_tid, SIGUSR1);
        break;
        case KEY_DOWN:
        pthread_kill(pacman_tid, SIGUSR2);
        break;
      }
    }
  }
}

void handler_signal(int sig) 
{
  switch(sig) 
  {
    case SIGINT:  
    dir = GAUCHE; 
    break;
    case SIGHUP:  
    dir = DROITE; 
    break;
    case SIGUSR1: 
    dir = HAUT; 
    break;
    case SIGUSR2: 
    dir = BAS; 
    break;
  }
}