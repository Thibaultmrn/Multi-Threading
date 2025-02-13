#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <setjmp.h>
#include "protocole.h" // contient la cle et la structure d'un message

#define FICHIER_UTILISATEUR "clients.dat"
int idQ,idShm,idSem;
int fdPipe[2];
TAB_CONNEXIONS *tab;



typedef struct
{
  char  nom[20];
  int   hash;
} CLIENT;

void afficheTab();
void ajouterClient(const char* nom, const char* motDePasse);
int hash(const char* motDePasse);
int verifieMotDePasse(int pos, const char* motDePasse);
int estPresent(const char* nom);
void handleSIGINT(int sig);
void caddieConnection(MESSAGE m);
void HandlerSIGCHLD(int sig);

sigjmp_buf contexte;

int main()
{
  // TO DO
  struct sigaction C;
  C.sa_handler = handleSIGINT; 
  C.sa_flags = 0;  
  sigemptyset(&C.sa_mask); 

  if (sigaction(SIGINT, &C, NULL) == -1) 
  {
    perror("(SERVEUR) Erreur lors de l'armement du signal SIGINT");
    exit(1);
  }

  struct sigaction A;
  A.sa_handler = HandlerSIGCHLD;
  sigemptyset(&A.sa_mask);
  A.sa_flags = 0;
  if (sigaction(SIGCHLD,&A,NULL) == -1)
  {
    perror("Erreur de sigaction");
    exit(1);
  }
 

  int shmid = shmget(CLE, 1024, IPC_CREAT | 0666);
  if (shmid == -1)
  {
      perror("Erreur shmget (création memoire partagée)");
      exit(1);
  }

  fprintf(stderr,"(SERVEUR %d) Creation de la file de messages\n",getpid());
  if ((idQ = msgget(CLE,IPC_CREAT | IPC_EXCL | 0600)) == -1)
  {
    perror("(SERVEUR) Erreur de msgget");
    exit(1);
  }

  // TO BE CONTINUED

 
  // TO DO
  if (pipe(fdPipe) == -1) 
  {
    perror("(SERVEUR) Erreur lors de la création du pipe");
    exit(1);
  }
  // Initialisation du tableau de connexions
  tab = (TAB_CONNEXIONS*) malloc(sizeof(TAB_CONNEXIONS)); 

  for (int i=0 ; i<6 ; i++)
  {
    tab->connexions[i].pidFenetre = 0;
    strcpy(tab->connexions[i].nom,"");
    tab->connexions[i].pidCaddie = 0;
  }
  tab->pidServeur = getpid();
  tab->pidPublicite = 0;

  afficheTab();

  // Creation du processus Publicite (étape 2)
  // TO DO
  pid_t pidPublicite = fork();
  if (pidPublicite == 0) 
  {
    execl("./Publicite", "Publicite", NULL);
    perror("(SERVEUR) Erreur lors de l'exécution du processus Publicité");
    exit(1);
  } 
  else 
  {
    if (pidPublicite > 0) 
    {
      tab->pidPublicite = pidPublicite;
    } 
    else  
    {
      perror("(SERVEUR) Erreur lors de la création du processus Publicité");
      exit(1);
    }
  }
  // Creation du processus AccesBD (étape 4)
  // TO DO
  fprintf(stderr, "(SERVEUR %d) Création du processus AccesBD\n", getpid());
  pid_t pidAccesBD = fork();
  if (pidAccesBD == -1)
  {
      perror("(SERVEUR) Erreur lors de la création du processus AccesBD");
      exit(1);
  }
  else if (pidAccesBD == 0) 
  {
      close(fdPipe[1]);  
      char fdLecture[10];
      sprintf(fdLecture, "%d", fdPipe[0]);
      execl("./AccesBD", "AccesBD", fdLecture, NULL);
      perror("Erreur lors du lancement d'AccesBD");
      exit(1);
  }
  tab->pidAccesBD = pidAccesBD;
  close(fdPipe[0]);


  MESSAGE m;
  MESSAGE reponse;
  int ret;
  sigsetjmp(contexte, 1);
 
  while(1)
  {

  	fprintf(stderr,"(SERVEUR %d) Attente d'une requete...\n",getpid());
    if (msgrcv(idQ,&m,sizeof(MESSAGE)-sizeof(long),1,0) == -1)
    {
      perror("(SERVEUR) Erreur de msgrcv");
      msgctl(idQ,IPC_RMID,NULL);
      exit(1);
    }

    switch(m.requete)
    {
      case CONNECT :  // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete CONNECT reçue de %d\n",getpid(),m.expediteur);
                       // Recherche de la première ligne vide dans la table de connexions
                      for (int i = 0; i < 6; i++) 
                      {
                          if (tab->connexions[i].pidFenetre == 0) 
                          { 
                              tab->connexions[i].pidFenetre = m.expediteur; 
                              tab->connexions[i].nom[0] = '\0';  
                              tab->connexions[i].pidCaddie = 0;  
                              fprintf(stderr, "(SERVEUR %d) Fenêtre avec PID %d ajoutée à la table de connexions.\n", getpid(), m.expediteur);
                            break;
                          }
                      }
                      break;

      case DECONNECT : // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete DECONNECT reçue de %d\n",getpid(),m.expediteur);
                          
                      for (int i = 0; i < 6; i++) 
                      {
                          if (tab->connexions[i].pidFenetre == m.expediteur) 
                          {  
                              tab->connexions[i].pidFenetre = 0; 
                              fprintf(stderr, "(SERVEUR %d) Fenêtre avec PID %d supprimée de la table de connexions.\n", getpid(), m.expediteur);
                              break;
                          }
                      }
                      
                      break;
      case LOGIN :    // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete LOGIN reçue de %d : --%d--%s--%s--\n",getpid(),m.expediteur,m.data1,m.data2,m.data3);
                      if(m.data1 == 1)
                      { 

                        int verifie = estPresent(m.data2);
                        if(verifie == 0 || verifie == -1)
                        {
                          ajouterClient(m.data2, m.data3);
                          reponse.data1 = 1;
                          strcpy(reponse.data4,"Nouveau client créé avec succès.");

      
                          for (int i = 0; i < 6; i++) 
                          {
                            if (tab->connexions[i].pidFenetre == m.expediteur) 
                            { 
                              tab->connexions[i].pidFenetre = m.expediteur; 
                              strncpy(tab->connexions[i].nom, m.data2, sizeof(tab->connexions[i].nom) - 1);  
                              tab->connexions[i].pidCaddie = 0;  
                              break;
                            }
                          }
                          caddieConnection(m);
                        }
                        else
                        {
                          reponse.data1 = 0;
                          strcpy(reponse.data4,"Nom d'utilisateur déjà existant !");

                        }
                        
                      }
                      else
                      {
                        int position = estPresent(m.data2);
                        if(position != 0)
                        {
                          int verifie = verifieMotDePasse(position, m.data3);
                          if(verifie == 1)
                          {
                            reponse.data1 = 1;
                            strcpy(reponse.data4,"Connexion réussie.");
                     
                            for (int i = 0; i < 6; i++) 
                            {
                              if (tab->connexions[i].pidFenetre == m.expediteur) 
                              {  
                                tab->connexions[i].pidFenetre = m.expediteur;  
                                strncpy(tab->connexions[i].nom, m.data2, sizeof(tab->connexions[i].nom) - 1);  
                                tab->connexions[i].pidCaddie = 0; 
                                break;
                              }
                            }
                            caddieConnection(m);
                          }
                          else
                          {
                            reponse.data1 = 0;
                            strcpy(reponse.data4,"Mot de passe incorrect.");
                          }
                        }
                        else
                        {
                          reponse.data1 = 0;
                          strcpy(reponse.data4,"Nom d'utilisateur non trouvé.");
                        }

                      }
                    
                      reponse.type = m.expediteur;
                      reponse.requete = LOGIN;
                      if (msgsnd(idQ, &reponse, sizeof(MESSAGE) - sizeof(long), 0) == -1) 
                      {
                            perror("Erreur lors de l'envoi de la réponse");
                      }   
                                         
                      if (kill(m.expediteur, SIGUSR1) == -1) 
                      {
                        perror("Erreur envoi signal");
                      }
                      
                      break; 

      case LOGOUT :   // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete LOGOUT reçue de %d\n",getpid(),m.expediteur);
                      fprintf(stderr, "(CLIENT %d) Réponse LOGOUT reçue du serveur.\n", getpid());
                      
                      for (int i = 0; i < 6; i++) 
                      {
                        if (tab->connexions[i].pidFenetre == m.expediteur) 
                        {
                            reponse.type = tab->connexions[i].pidCaddie;
                            reponse.requete = LOGOUT;
                            reponse.expediteur = m.expediteur;
                            strcpy(tab->connexions[i].nom, "");  
                   
                            fprintf(stderr, "(SERVEUR %d) Utilisateur déconnecté pour la fenêtre PID %d\n", getpid(), m.expediteur);
                            msgsnd(idQ, &reponse, sizeof(MESSAGE) - sizeof(long), 0);
                                                     
                            break;
                        }
                      }
                      
                      break;

      case UPDATE_PUB :  // TO DO
                       fprintf(stderr, "(SERVEUR %d) Requête UPDATE_PUB reçue de %d\n", getpid(), m.expediteur);

                      // Parcours de la table de connexions
                      for (int i = 0; i < 6; i++) 
                      {
                          if (tab->connexions[i].pidFenetre != 0) 
                          {
                              //fprintf(stderr, "(SERVEUR %d) Envoi du signal SIGUSR2 à la fenêtre PID %d\n", getpid(), tab->connexions[i].pidFenetre);

                              
                              if (kill(tab->connexions[i].pidFenetre, SIGUSR2) == -1) 
                              {
                                  perror("(SERVEUR) Erreur lors de l'envoi du signal SIGUSR2");
                              }
                          }
                      }
                      break;

      case CONSULT :  // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete CONSULT reçue de %d\n",getpid(),m.expediteur);
                      for (int i = 0; i < 6; i++) 
                      {
                        if (tab->connexions[i].pidFenetre == m.expediteur) 
                        {
                          int pidCaddie = tab->connexions[i].pidCaddie;
                          if(pidCaddie > 0)
                          {
                            
                            reponse.type = pidCaddie;
                            reponse.data1 = m.data1;
                            reponse.requete = CONSULT;
                            reponse.expediteur = m.expediteur;
                            if(msgsnd(idQ,&reponse,sizeof(MESSAGE)-sizeof(long),0) == -1) 
                            {
                              perror("(SERVEUR) Erreur lors de l'envoi de la requête CONSULT");
                            }                      
                            break;
                          }

                        }
                      }
                      break;

      case ACHAT :    // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete ACHAT reçue de %d\n",getpid(),m.expediteur);
                      for (int i = 0; i < 6; i++) 
                      {
                        if (tab->connexions[i].pidFenetre == m.expediteur) 
                        {
                          int pidCaddie = tab->connexions[i].pidCaddie;
                          if(pidCaddie > 0)
                          {
                            reponse.type = pidCaddie;
                            reponse.data1 = m.data1;
                            strcpy(reponse.data2, m.data2);
                            reponse.requete = ACHAT;
                            reponse.expediteur = m.expediteur;
                            if(msgsnd(idQ,&reponse,sizeof(MESSAGE)-sizeof(long),0) == -1) 
                            {
                              perror("(SERVEUR) Erreur lors de l'envoi de la requête ACHAT");
                            }                      
                            break;
                          }

                        }
                      }
                      break;

      case CADDIE :   // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete CADDIE reçue de %d\n",getpid(),m.expediteur);
                      for (int i = 0; i < 6; i++) 
                      {
                        if (tab->connexions[i].pidFenetre == m.expediteur) 
                        {
                          if(tab->connexions[i].pidFenetre > 0)
                          {
                            
                            m.type = tab->connexions[i].pidCaddie;
                            
                            if(msgsnd(idQ,&m,sizeof(MESSAGE)-sizeof(long),0) == -1) 
                            {
                              perror("(SERVEUR) Erreur lors de l'envoi de la requête CONSULT");
                            }                      
                            break;
                          }

                        }
                      }
                      break;

      case CANCEL :   // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete CANCEL reçue de %d\n",getpid(),m.expediteur);
                      for(int i = 0;i < 6; i++)
                      {
                        if(tab->connexions[i].pidFenetre == m.expediteur)
                        {
                          if(tab->connexions[i].pidCaddie > 0)
                          {
                            m.type = tab->connexions[i].pidCaddie;
                            m.requete = CANCEL;
                            if(msgsnd(idQ,&m,sizeof(MESSAGE)-sizeof(long),0) == -1) 
                            {
                              perror("(SERVEUR) Erreur lors de l'envoi de la requête ACHAT");
                            }
                          }
                          break;
                        }
                      }
                      break;

      case CANCEL_ALL : // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete CANCEL_ALL reçue de %d\n",getpid(),m.expediteur);
                      for(int i = 0;i < 6; i++)
                      {
                        if(tab->connexions[i].pidFenetre == m.expediteur)
                        {
                          if(tab->connexions[i].pidCaddie > 0)
                          {
                            m.type = tab->connexions[i].pidCaddie;
                            if(msgsnd(idQ,&m,sizeof(MESSAGE)-sizeof(long),0) == -1) 
                            {
                              perror("(SERVEUR) Erreur lors de l'envoi de la requête ACHAT");
                            }
                          }
                          break;
                        }
                      }
                      break;

      case PAYER : // TO DO
                      fprintf(stderr,"(SERVEUR %d) Requete PAYER reçue de %d\n",getpid(),m.expediteur);

                      for(int i = 0;i < 6; i++)
                      {
                        if(tab->connexions[i].pidFenetre == m.expediteur)
                        {
                          if(tab->connexions[i].pidCaddie > 0)
                          {
                            m.type = tab->connexions[i].pidCaddie;
                            if(msgsnd(idQ,&m,sizeof(MESSAGE)-sizeof(long),0) == -1) 
                            {
                              perror("(SERVEUR) Erreur lors de l'envoi de la requête ACHAT");
                            }
                          }
                          break;
                        }
                      }
                      break;

      case NEW_PUB :  // TO DO
                      fprintf(stderr, "(PUBLICITE %d) Nouvelle publicité reçue : %s\n", getpid(), m.expediteur);
                      break;
    }
    afficheTab();
  }
}

void afficheTab()
{
  fprintf(stderr,"Pid Serveur   : %d\n",tab->pidServeur);
  fprintf(stderr,"Pid Publicite : %d\n",tab->pidPublicite);
  fprintf(stderr,"Pid AccesBD   : %d\n",tab->pidAccesBD);
  for (int i=0 ; i<6 ; i++)
    fprintf(stderr,"%6d -%20s- %6d\n",tab->connexions[i].pidFenetre,
                                                      tab->connexions[i].nom,
                                                      tab->connexions[i].pidCaddie);
  fprintf(stderr,"\n");
}

int hash(const char* motDePasse)
{
  // TO DO
  short i, somme = 0;
  for(i = 0; motDePasse[i] != '\0'; i++)
  {
    somme += (i+1) * motDePasse[i];
  }
  return somme % 97;
}
int verifieMotDePasse(int pos, const char* motDePasse)
{
  // TO DO
  int fd;
  if((fd = open(FICHIER_UTILISATEUR, O_RDONLY)) == -1)
  {
    printf("Erreur ouverture fichier\n");
    return -1;
  }
  CLIENT user;
  int offset;
  offset = (pos - 1) * sizeof(CLIENT);
  if(lseek(fd, offset, SEEK_SET) == -1 || read(fd, &user, sizeof(CLIENT)) != sizeof(CLIENT))
  {
    close(fd);
    return -1;
  }
  close(fd);
  int hashMotDePasse;
  hashMotDePasse = hash(motDePasse);
  if(user.hash == hashMotDePasse)
    return 1;
  return 0;
}

int estPresent(const char* nom)
{
  // TO DO
  int fd, rc;
  fd = open(FICHIER_UTILISATEUR, O_RDONLY);
  if(fd == -1)
  {
    return -1;
  }
  else
  {
    short position = 1;
    CLIENT user;
    while((rc = read(fd,&user, sizeof(CLIENT))) == sizeof(CLIENT))
    {
      if(strcmp(nom, user.nom) == 0)
      {
        close(fd);
        return position;
      }
      position++;
    }
    close(fd);
  }
  
  return -1;
}
void ajouterClient(const char* nom, const char* motDePasse)
{
  int fd = open(FICHIER_UTILISATEUR, O_WRONLY | O_APPEND | O_CREAT, 0644);
  if(fd == -1)
  {
    perror("Erreur ouverture fichier pour ajout");
    exit(1);
  }

  CLIENT newClient;
  strncpy(newClient.nom, nom, sizeof(newClient.nom) - 1);
  newClient.nom[sizeof(newClient.nom) - 1] = '\0';
  newClient.hash = hash(motDePasse);
  
  if(write(fd, &newClient, sizeof(CLIENT)) != sizeof(CLIENT))
  {
    perror("Erreur lors de l'écriture dans le fichier utilisateur");
    close(fd);
    exit(1);
  }
  
  close(fd);
  fprintf(stderr, "Nouveau client ajouté : %s\n", nom);
}

void handleSIGINT(int sig) 
{
    fprintf(stderr, "(SERVEUR) Signal SIGINT reçu, nettoyage des ressources...\n");

    if (msgctl(idQ, IPC_RMID, NULL) == -1) {
        perror("(SERVEUR) Erreur lors de la suppression de la file de messages");
    } else {
        fprintf(stderr, "(SERVEUR) File de messages supprimée\n");
    }

    if (fdPipe[0] != -1 && fdPipe[1] != -1) {
        close(fdPipe[0]);
        close(fdPipe[1]);
        fprintf(stderr, "(SERVEUR) Pipe fermé\n");
    }

    if (idShm != -1) {
        shmctl(idShm, IPC_RMID, NULL);
        fprintf(stderr, "(SERVEUR) Mémoire partagée supprimée\n");
    }

    exit(0);
}
void caddieConnection(MESSAGE m)
{
  MESSAGE reponse;
  pid_t pidCaddie = fork();
  if (pidCaddie == 0) 
  {

      printf("(SERVEUR) lancement du Processus Caddie PID = %d\n", getpid());
      close(fdPipe[0]); 
      char fdEcriture[10];
      sprintf(fdEcriture, "%d", fdPipe[1]);
      execl("./Caddie", "Caddie", fdEcriture, NULL);
      perror("Erreur lors du lancement de Caddie");
      exit(1);
  } 
  else if (pidCaddie > 0) 
  {
      for (int i = 0; i < 6; i++) 
      {
          if (tab->connexions[i].pidFenetre == m.expediteur) 
          {
              tab->connexions[i].pidCaddie = pidCaddie; 

              reponse.type = tab->connexions[i].pidCaddie;
              reponse.expediteur = m.expediteur;
              reponse.requete = LOGIN;
        
              msgsnd(idQ, &reponse, sizeof(MESSAGE) - sizeof(long), 0);
              break;
          }
      }
  }
}
void HandlerSIGCHLD(int sig) 
{
    int id;
    int status;
    int resultat;

    while((id = wait(&status)) != -1) 
    {
        if (WIFEXITED(status)) 
        {
            resultat = WEXITSTATUS(status);

            for (int i = 0; i < 6; i++) 
            {
                if (tab->connexions[i].pidCaddie == id) 
                {
                    printf("(SERVEUR %d) suppression du fils zombi (PID: %d)\n", getpid(), id);
                    tab->connexions[i].pidCaddie = 0;
                    siglongjmp(contexte,sig);
                    
                }
            }
        } 
        else
        {
            fprintf(stderr, "(SERVEUR %d) Processus Caddie %d mal terminé", getpid(), id);
        }
    }
    
    

}