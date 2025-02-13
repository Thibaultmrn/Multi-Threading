#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <mysql.h>
#include "protocole.h" // contient la cle et la structure d'un message

int idQ;

ARTICLE articles[10];
int nbArticles = 0;

int fdWpipe;
pid_t pidClient;

MYSQL* connexion;

void handlerSIGALRM(int sig);

int main(int argc,char* argv[])
{
  // Masquage de SIGINT
  sigset_t mask;
  sigaddset(&mask,SIGINT);
  sigprocmask(SIG_SETMASK,&mask,NULL);

  // Armement des signaux
  // TO DO
  struct sigaction sa;
  sa.sa_handler = handlerSIGALRM;
  sa.sa_flags = IPC_NOWAIT;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGALRM, &sa, NULL) == -1) 
  {
      perror("(CADDIE) Erreur lors de l'armement du signal SIGALRM");
      exit(1);
  }  

  // Recuperation de l'identifiant de la file de messages
  fprintf(stderr,"(CADDIE %d) Recuperation de l'id de la file de messages\n",getpid());
  if ((idQ = msgget(CLE,0)) == -1)
  {
    perror("(CADDIE) Erreur de msgget");
    exit(1);
  }



  MESSAGE m;
  MESSAGE reponse;

  // Récupération descripteur écriture du pipe
  fdWpipe = atoi(argv[1]);

  while(1)
  {
    if (msgrcv(idQ,&m,sizeof(MESSAGE)-sizeof(long),getpid(),0) == -1)
    {
      perror("(CADDIE) Erreur de msgrcv");
      exit(1);
    }
    
    //fprintf(stderr, "(CADDIE) Message reçu : requête %d\n", m.requete);
    
    

    switch(m.requete)
    {

      case LOGIN :    // TO DO
                      alarm(0);
                      alarm(60);
                      fprintf(stderr,"(CADDIE %d) Requete LOGIN reçue de %d\n",getpid(),m.expediteur);
                      pidClient = m.expediteur;
                      break;

      case LOGOUT :   {// TO DO
                      alarm(0);
                      alarm(60);
                      fprintf(stderr,"(CADDIE %d) Requete LOGOUT reçue de %d\n",getpid(),m.expediteur);

                      mysql_close(connexion);
                      exit(0);
                      break;
                    }

      case CONSULT :  
                    {   // TO DO
                      alarm(0);
                      alarm(60);
                      fprintf(stderr,"(CADDIE %d) Requete CONSULT reçue de %d\n",getpid(),m.expediteur);

                      m.expediteur = getpid();

                      if (write(fdWpipe, &m, sizeof(MESSAGE)) == -1) 
                      {
                          perror("(CADDIE) Erreur lors de l'écriture dans le pipe");
                          exit(1);
                      }

                      if (msgrcv(idQ, &reponse, sizeof(MESSAGE) - sizeof(long), getpid(), 0) == -1) 
                      {
                          perror("(CADDIE) Erreur lors de la réception de la réponse via la file");
                          exit(1);
                      }

                      if (reponse.data1 == -1) 
                      {
                          fprintf(stderr, "(CADDIE %d) Article non trouvé, aucune réponse envoyée\n", getpid());
                      } 
                      else 
                      {
                        
                          reponse.type = pidClient;
                          fprintf(stderr, " pidClient %d \n", pidClient);
                          reponse.requete = CONSULT;
                          if (msgsnd(idQ, &reponse, sizeof(MESSAGE) - sizeof(long), 0) == -1) 
                          {
                              perror("(CADDIE) Erreur lors de l'envoi de la réponse au client");
                              exit(1);
                          }
                          kill(pidClient, SIGUSR1);
                      }
                      break;
                    }
      case ACHAT :    // TO DO
                  { 
                      alarm(0);
                      alarm(60);
                      fprintf(stderr,"(CADDIE %d) Requete ACHAT reçue de %d\n",getpid(),m.expediteur);
                      // Modifier l'expéditeur pour indiquer que la requête vient de ce Caddie
                      m.expediteur = getpid();

                      if (write(fdWpipe, &m, sizeof(MESSAGE)) == -1) 
                      {
                          perror("(CADDIE) Erreur lors de l'écriture dans le pipe");
                          exit(1);
                      }

                      if (msgrcv(idQ, &reponse, sizeof(MESSAGE) - sizeof(long), getpid(), 0) == -1) 
                      {
                          perror("(CADDIE) Erreur lors de la réception de la réponse via la file");
                          exit(1);
                      }

                      if (strcmp(reponse.data3, "0") != 0) 
                      {
                          if (nbArticles < 10) 
                          {
                            articles[nbArticles].id = reponse.data1;  
                            strcpy(articles[nbArticles].intitule, reponse.data2);  
                            articles[nbArticles].prix = reponse.data5;  
                            articles[nbArticles].stock = atoi(reponse.data3);
                              nbArticles++;
                              fprintf(stderr, "(CADDIE %d) Article ajouté au panier : %s\n", getpid(), reponse.data2);
                          } 
                          else 
                          {
                              fprintf(stderr, "(CADDIE %d) Panier plein, impossible d'ajouter plus d'articles.\n", getpid());
                          }
                      } 
                      else 
                      {
                          fprintf(stderr, "(CADDIE %d) Achat échoué, article non ajouté.\n", getpid());
                      }
                      
                      reponse.type = pidClient;
                      reponse.expediteur = getpid();
                      fprintf(stderr, " pidClient %d \n", pidClient);
                      reponse.requete = ACHAT;
                      if (msgsnd(idQ, &reponse, sizeof(MESSAGE) - sizeof(long), 0) == -1) 
                      {
                          perror("(CADDIE) Erreur lors de l'envoi de la réponse au client");
                          exit(1);
                      }
                      kill(pidClient, SIGUSR1);
                      
                      break;
                    }

      case CADDIE :   // TO DO
                    {  
                      alarm(0);
                      alarm(60);
                      fprintf(stderr,"(CADDIE %d) Requete CADDIE reçue de %d\n",getpid(),m.expediteur);
                      
                      for(int i = 0; i < nbArticles; i++)
                      {
                        fprintf(stderr, " pidClient %d \n", pidClient);
                        reponse.type = pidClient;
                        strcpy(reponse.data2,articles[i].intitule);
                        reponse.data5 = articles[i].prix;
                        sprintf(reponse.data3, "%d", articles[i].stock);
                        sprintf(reponse.data4, "%d", nbArticles);
                        reponse.data1 = i;
                        reponse.requete = CADDIE;
                        reponse.expediteur = getpid();
                        if(msgsnd(idQ, &reponse, sizeof(MESSAGE) - sizeof(long), 0) == -1) 
                        {
                            perror("(CADDIE) Erreur lors de l'envoi de la réponse au client");
                            exit(1);
                        }
                        kill(pidClient, SIGUSR1);
                        
                      }
                      break;
                    }
      case CANCEL : {  // TO DO
                      alarm(0);
                      alarm(60);
                      fprintf(stderr,"(CADDIE %d) Requete CANCEL reçue de %d\n",getpid(),m.expediteur);

                      
                      if(m.data1 < 0 || m.data1 >= nbArticles)
                      {
                        fprintf(stderr, "(CADDIE %d) Indice invalide : %d\n", getpid(), m.data1);
                        break;
                      }

                      MESSAGE cancelReq;
                      cancelReq.requete = CANCEL;
                      cancelReq.expediteur = getpid();
                      cancelReq.data1 = articles[m.data1].id;
                      sprintf(cancelReq.data2, "%d", articles[m.data1].stock); 

                      if (write(fdWpipe, &cancelReq, sizeof(MESSAGE)) == -1) {
                          perror("(CADDIE) Erreur lors de l'écriture dans le pipe vers AccesBD");
                          exit(1);
                      }
                      // Suppression de l'aricle du panier
                      for(int i=0;i<nbArticles -1;i++)
                      {
                        articles[i]=articles[i+1];
                      }
                       nbArticles--;
                      break;
                    }

      case CANCEL_ALL : // TO DO
                      {
                      alarm(0);
                      alarm(60);
                      fprintf(stderr,"(CADDIE %d) Requete CANCEL_ALL reçue de %d\n",getpid(),m.expediteur);

                      for (int i = 0; i < nbArticles; i++) 
                      {
                          MESSAGE cancelReq;
                          memset(&cancelReq, 0, sizeof(MESSAGE));

                          cancelReq.requete = CANCEL;
                          cancelReq.expediteur=getpid();
                          cancelReq.data1 = articles[i].id;  
                          sprintf(cancelReq.data2, "%d", articles[i].stock);  

                          
                          if (write(fdWpipe, &cancelReq, sizeof(MESSAGE)) == -1) 
                          {
                              perror("(CADDIE) Erreur lors de l'écriture dans le pipe vers AccesBD");
                              exit(1);
                          }

                          fprintf(stderr, "(CADDIE %d) Envoi de CANCEL pour l'article ID=%d\n", getpid(), articles[i].id);
                      }

    
                      nbArticles = 0;  
                      break;
                      }

      case PAYER :    // TO DO
                    {
                    alarm(0);
                    alarm(60);
                    fprintf(stderr,"(CADDIE %d) Requete PAYER reçue de %d\n",getpid(),m.expediteur);

                    nbArticles = 0;  

                    
                    MESSAGE reponse;
                    reponse.type = m.expediteur;
                    reponse.requete = PAYER;
                    reponse.data1 = 1;

                    if (msgsnd(idQ, &reponse, sizeof(MESSAGE) - sizeof(long), 0) == -1) 
                    {
                        perror("(CADDIE) Erreur lors de l'envoi de la réponse PAYER");
                        exit(1);
                    }

                    fprintf(stderr, "(CADDIE %d) Panier vidé et paiement effectué avec succès\n", getpid());
                    break;
                                    
                  }
    }
  }
}

void handlerSIGALRM(int sig)
{
  fprintf(stderr,"(CADDIE %d) Time Out !!!\n",getpid());

  if (nbArticles > 0) 
  {
      
      for (int i = 0; i < nbArticles; i++) 
      {
          MESSAGE cancelReq;
          memset(&cancelReq, 0, sizeof(MESSAGE));

          cancelReq.requete = CANCEL;
          cancelReq.expediteur=getpid();
          cancelReq.data1 = articles[i].id; 
          sprintf(cancelReq.data2, "%d", articles[i].stock);  


          if (write(fdWpipe, &cancelReq, sizeof(MESSAGE)) == -1) 
          {
              perror("(CADDIE) Erreur lors de l'écriture dans le pipe vers AccesBD");
              exit(1);
          }

          fprintf(stderr, "(CADDIE %d) Envoi de CANCEL pour l'article ID=%d\n", getpid(), articles[i].id);
      }
    }

  MESSAGE timeoutMsg;
  memset(&timeoutMsg, 0, sizeof(MESSAGE));
  timeoutMsg.requete = TIME_OUT;
  timeoutMsg.type = pidClient;
  timeoutMsg.expediteur=getpid();

  if (msgsnd(idQ, &timeoutMsg, sizeof(MESSAGE) - sizeof(long), 0) == -1) 
  {
      perror("(CADDIE) Erreur lors de l'envoi de la requête TIME_OUT au client");
  }
  
  if (kill(pidClient, SIGUSR1) == -1)
  {
      perror("Erreur lors de l'envoi du signal SIGUSR1");
      exit(1);
  }
  printf("%d",pidClient);
  exit(0);
}