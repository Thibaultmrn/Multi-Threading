#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <setjmp.h>
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
pthread_mutex_t mutexNbPacGom;
pthread_mutex_t mutexDelai;
pthread_mutex_t mutexScore;
pthread_cond_t condNbPacGom;
pthread_cond_t condScore;
int nbPacGom = 0, level = 1, delai = 300, score = 0, MAJScore = false; 
sigjmp_buf contexte;
struct sigaction sigAct;

void DessineGrilleBase();
void Attente(int milli);
void setTab(int l, int c, int presence = VIDE, pthread_t tid = 0);
void* fctThreadPacMan(void* param);
void* fctThreadEvent(void* param);
void* fctThreadPacGom(void* param);
void* fctThreadScore(void* param);
void handler_signal(int sig);
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc,char* argv[])
{
  EVENT_GRILLE_SDL event;
  sigset_t mask;
  struct sigaction sigAct;
  char ok;
  pthread_t threadPacMan, threadEvent, threadPacGom, threadScore;
  int ret;

  srand((unsigned)time(NULL));
  pthread_mutex_init(&mutexParam, NULL);
  pthread_mutex_init(&mutexNbPacGom, NULL);
  pthread_mutex_init(&mutexDelai, NULL);
  pthread_mutex_init(&mutexScore, NULL);
  pthread_cond_init(&condNbPacGom, NULL);
  pthread_cond_init(&condScore, NULL);


  // Ouverture de la fenetre graphique
  printf("(MAIN%p) Ouverture de la fenetre graphique\n",pthread_self()); fflush(stdout);
  if (OuvertureFenetreGraphique() < 0)
  {
    printf("Erreur de OuvrirGrilleSDL\n");
    fflush(stdout);
    exit(1);
  }

  DessineGrilleBase();
  DessineChiffre(14, 22, level);

  sigemptyset(&sigAct.sa_mask);
  sigAct.sa_handler = handler_signal;
  sigaction(SIGINT, &sigAct, NULL);
  sigaction(SIGHUP, &sigAct, NULL);
  sigaction(SIGUSR1, &sigAct, NULL);
  sigaction(SIGUSR2, &sigAct, NULL);

  // Exemple d'utilisation de GrilleSDL et Ressources --> code a supprimer
 // DessinePacMan(17,7,GAUCHE);  // Attention !!! tab n'est pas modifie --> a vous de le faire !!!
  /*DessineChiffre(14,25,9);
  /*DessineFantome(5,9,ROUGE,DROITE);
  DessinePacGom(7,4);
  DessineSuperPacGom(9,5);
  DessineFantomeComestible(13,15);
  DessineBonus(5,15);*/
  pthread_create(&threadPacGom,NULL,fctThreadPacGom, NULL);
  pthread_create(&threadPacMan,NULL,fctThreadPacMan,NULL);
  pthread_create(&threadScore,NULL,fctThreadScore,NULL);
  pthread_create(&threadEvent,NULL,fctThreadEvent,&threadPacMan);

  pthread_join(threadEvent,NULL);
  //pthread_join(threadPacMan,NULL);
  //pthread_join(threadPacGom,NULL);
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
    pthread_mutex_lock(&mutexDelai);
    Attente(delai);
    pthread_mutex_unlock(&mutexDelai);
    sigprocmask(SIG_SETMASK, &oldmask, NULL);

    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    pthread_mutex_lock(&mutexParam);
    //setTab(L, C, VIDE);
    //

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
        EffaceCarre(L, C);
        tab[L][C].presence = VIDE;       

        L = newL;
        C = newC;
        if(tab[L][C].presence == PACGOM || tab[L][C].presence == SUPERPACGOM)
        {
          pthread_mutex_lock(&mutexNbPacGom);
          nbPacGom--;  
          pthread_cond_signal(&condNbPacGom); // Réveiller threadPacGom
          pthread_mutex_unlock(&mutexNbPacGom);

          pthread_mutex_lock(&mutexScore);
          if(tab[L][C].presence == PACGOM )
          {
            score++;
            MAJScore = true;
            pthread_cond_signal(&condScore);
          }
          else
          {
            score += 5;
            MAJScore = true;
            pthread_cond_signal(&condScore);
          }
          pthread_mutex_unlock(&mutexScore);
          tab[L][C].presence = PACMAN;
        }
      }
    }
    setTab(L, C, PACMAN, pthread_self());
    DessinePacMan(L, C, dir);

    pthread_mutex_unlock(&mutexParam);
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
      pthread_cancel(pacman_tid);  // Arrêter le thread PacMan
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
    if(tab[L][C-1].presence != MUR)
    {
      dir = GAUCHE; 
    }
    break;

    case SIGHUP:  
    if(tab[L][C+1].presence != MUR)
    {
      dir = DROITE;
    }
    break;

    case SIGUSR1: 
    if(tab[L-1][C].presence != MUR)
    {
      dir = HAUT; 
    }
    break;

    case SIGUSR2: 
    if(tab[L+1][C].presence != MUR)
    {
      dir = BAS;
    }
    break;
  }
}

void* fctThreadPacGom(void* param)
{
  sigsetjmp(contexte, 1);
  for(int i = 0; i < NB_LIGNE; i++)
  {
    for(int j = 0; j < NB_COLONNE; j++)
    {
      if(tab[i][j].presence == VIDE)
      {
        if((j == 1 && i == 2)||(j == 15 && i == 2)||(j == 1 && i == 15)||(j == 15 && i == 15))
        {
          DessineSuperPacGom(i,j);
          tab[i][j].presence = SUPERPACGOM;
          pthread_mutex_lock(&mutexNbPacGom);
          nbPacGom++;
          pthread_mutex_unlock(&mutexNbPacGom);
        }
        else 
        {
          if((j != 8 && i != 15) || (j != 8 && i != 8)||(j != 8 && i != 9))
          {
            DessinePacGom(i,j);
            tab[i][j].presence = PACGOM;
            pthread_mutex_lock(&mutexNbPacGom);
            nbPacGom++;
            pthread_mutex_unlock(&mutexNbPacGom);
          }
        }
      }

    }
  }

  pthread_mutex_lock(&mutexNbPacGom);
  int tmp = nbPacGom;
  DessineChiffre(12, 24, tmp % 10);
  tmp /= 10;
  DessineChiffre(12, 23, tmp % 10);
  tmp /= 10;
  DessineChiffre(12, 22, tmp % 10);
  pthread_mutex_unlock(&mutexNbPacGom);

  while(1)
  {
    pthread_mutex_lock(&mutexNbPacGom);
    while (nbPacGom > 0) 
    {

      pthread_cond_wait(&condNbPacGom, &mutexNbPacGom);

      tmp = nbPacGom;
      DessineChiffre(12, 24, tmp % 10);
      tmp /= 10;
      DessineChiffre(12, 23, tmp % 10);
      tmp /= 10;
      DessineChiffre(12, 22, tmp % 10);
    }
    if(nbPacGom == 0)
    {
      level ++;
      DessineChiffre(14, 22, level);
      pthread_mutex_lock(&mutexDelai);
      delai /= 2;
      pthread_mutex_unlock(&mutexDelai);
      pthread_mutex_unlock(&mutexNbPacGom);
      siglongjmp(contexte,1);
    }
    pthread_mutex_unlock(&mutexNbPacGom);

  }

  return 0;
}

void* fctThreadScore(void* param)
{
  while(1)
  {
    pthread_mutex_lock(&mutexScore);

    while(MAJScore == false)
    {
      pthread_cond_wait(&condScore, &mutexScore);
    }

    int tmpScore = score;
    pthread_mutex_unlock(&mutexScore);


    DessineChiffre(16, 25, tmpScore % 10);
    tmpScore /= 10;
    DessineChiffre(16, 24, tmpScore % 10);
    tmpScore /= 10;
    DessineChiffre(16, 23, tmpScore % 10);
    tmpScore /= 10;
    DessineChiffre(16, 22, tmpScore % 10);

    pthread_mutex_lock(&mutexScore);
    MAJScore = false;
    pthread_mutex_unlock(&mutexScore);
  }
  return 0;
}