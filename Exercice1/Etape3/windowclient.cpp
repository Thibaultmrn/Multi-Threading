#include "windowclient.h"
#include "ui_windowclient.h"
#include <QMessageBox>
#include <string>
using namespace std;

#include "protocole.h"

#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <signal.h>

extern WindowClient *w;

int idQ, idShm;
bool logged;
char* pShm;
ARTICLE articleEnCours;
float totalCaddie = 0.0;

void handlerSIGUSR1(int sig);
void handlerSIGUSR2(int sig);

#define REPERTOIRE_IMAGES "images/"

WindowClient::WindowClient(QWidget *parent) : QMainWindow(parent), ui(new Ui::WindowClient)
{
    ui->setupUi(this);

    // Configuration de la table du panier (ne pas modifer)
    ui->tableWidgetPanier->setColumnCount(3);
    ui->tableWidgetPanier->setRowCount(0);
    QStringList labelsTablePanier;
    labelsTablePanier << "Article" << "Prix à l'unité" << "Quantité";
    ui->tableWidgetPanier->setHorizontalHeaderLabels(labelsTablePanier);
    ui->tableWidgetPanier->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetPanier->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetPanier->horizontalHeader()->setVisible(true);
    ui->tableWidgetPanier->horizontalHeader()->setDefaultSectionSize(160);
    ui->tableWidgetPanier->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetPanier->verticalHeader()->setVisible(false);
    ui->tableWidgetPanier->horizontalHeader()->setStyleSheet("background-color: lightyellow");

    fprintf(stderr,"(CLIENT %d) Recuperation de l'id de la file de messages\n",getpid());
    // TO DO
    if((idQ = msgget(CLE,0)) == -1)
    {
        perror("Erreur msgget");
        exit(1);
    }

    fprintf(stderr,"(CLIENT %d) Recuperation de l'id de la mémoire partagée\n",getpid());
    // TO DO

    // Attachement à la mémoire partagée
    // TO DO
    

    idShm = shmget(CLE, 1024, 0666);
    if (idShm == -1) {
        perror("Erreur shmget");
        exit(1);
    }



    

    // Vérification si la chaîne est vide
    

    // Détacher la mémoire partagée
    

    // Armement des signaux
    // TO DO
    struct sigaction A;
    A.sa_handler = handlerSIGUSR1;
    sigemptyset(&A.sa_mask);
    A.sa_flags = 0;

    if (sigaction(SIGUSR1, &A, NULL) == -1) {
        perror("Erreur sigaction");
        exit(1);
    }
    signal(SIGUSR1, handlerSIGUSR1);
    
    struct sigaction B;
    B.sa_handler = handlerSIGUSR2;
    sigemptyset(&B.sa_mask);
    B.sa_flags = 0;

    if (sigaction(SIGUSR2, &B, NULL) == -1) {
        perror("Erreur sigaction");
        exit(1);
    }
    signal(SIGUSR2, handlerSIGUSR2);
    // Envoi d'une requete de connexion au serveur
    // TO DO
    MESSAGE msg;
    msg.type = 1;
    msg.expediteur = getpid();
    msg.requete = CONNECT;

    if(msgsnd(idQ, &msg, sizeof(MESSAGE)-sizeof(long),0)==-1)
    {
        perror("Erreur msgsnd CONNECT");
        exit(1);
    }
    fprintf(stderr, "(CLIENT %d) Requête CONNECT envoyée au serveur.\n", getpid());
    // Exemples à supprimer

    //setPublicite("Promotions sur les concombres !!!");
    //setArticle("pommes",5.53,18,"pommes.jpg");
    //ajouteArticleTablePanier("cerises",8.96,2);
}

WindowClient::~WindowClient()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles : ne pas modifier /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setNom(const char* Text)
{
  if (strlen(Text) == 0 )
  {
    ui->lineEditNom->clear();
    return;
  }
  ui->lineEditNom->setText(Text);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* WindowClient::getNom()
{
  strcpy(nom,ui->lineEditNom->text().toStdString().c_str());
  return nom;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setMotDePasse(const char* Text)
{
  if (strlen(Text) == 0 )
  {
    ui->lineEditMotDePasse->clear();
    return;
  }
  ui->lineEditMotDePasse->setText(Text);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* WindowClient::getMotDePasse()
{
  strcpy(motDePasse,ui->lineEditMotDePasse->text().toStdString().c_str());
  return motDePasse;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setPublicite(const char* Text)
{
  if (strlen(Text) == 0 )
  {
    ui->lineEditPublicite->clear();
    return;
  }
  ui->lineEditPublicite->setText(Text);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setImage(const char* image)
{
  // Met à jour l'image
  char cheminComplet[80];
  sprintf(cheminComplet,"%s%s",REPERTOIRE_IMAGES,image);
  QLabel* label = new QLabel();
  label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  label->setScaledContents(true);
  QPixmap *pixmap_img = new QPixmap(cheminComplet);
  label->setPixmap(*pixmap_img);
  label->resize(label->pixmap()->size());
  ui->scrollArea->setWidget(label);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WindowClient::isNouveauClientChecked()
{
  if (ui->checkBoxNouveauClient->isChecked()) return 1;
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setArticle(const char* intitule,float prix,int stock,const char* image)
{
  ui->lineEditArticle->setText(intitule);
  if (prix >= 0.0)
  {
    char Prix[20];
    sprintf(Prix,"%.2f",prix);
    ui->lineEditPrixUnitaire->setText(Prix);
  }
  else ui->lineEditPrixUnitaire->clear();
  if (stock >= 0)
  {
    char Stock[20];
    sprintf(Stock,"%d",stock);
    ui->lineEditStock->setText(Stock);
  }
  else ui->lineEditStock->clear();
  setImage(image);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WindowClient::getQuantite()
{
  return ui->spinBoxQuantite->value();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setTotal(float total)
{
  if (total >= 0.0)
  {
    char Total[20];
    sprintf(Total,"%.2f",total);
    ui->lineEditTotal->setText(Total);
  }
  else ui->lineEditTotal->clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::loginOK()
{
  ui->pushButtonLogin->setEnabled(false);
  ui->pushButtonLogout->setEnabled(true);
  ui->lineEditNom->setReadOnly(true);
  ui->lineEditMotDePasse->setReadOnly(true);
  ui->checkBoxNouveauClient->setEnabled(false);

  ui->spinBoxQuantite->setEnabled(true);
  ui->pushButtonPrecedent->setEnabled(true);
  ui->pushButtonSuivant->setEnabled(true);
  ui->pushButtonAcheter->setEnabled(true);
  ui->pushButtonSupprimer->setEnabled(true);
  ui->pushButtonViderPanier->setEnabled(true);
  ui->pushButtonPayer->setEnabled(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::logoutOK()
{
  ui->pushButtonLogin->setEnabled(true);
  ui->pushButtonLogout->setEnabled(false);
  ui->lineEditNom->setReadOnly(false);
  ui->lineEditMotDePasse->setReadOnly(false);
  ui->checkBoxNouveauClient->setEnabled(true);

  ui->spinBoxQuantite->setEnabled(false);
  ui->pushButtonPrecedent->setEnabled(false);
  ui->pushButtonSuivant->setEnabled(false);
  ui->pushButtonAcheter->setEnabled(false);
  ui->pushButtonSupprimer->setEnabled(false);
  ui->pushButtonViderPanier->setEnabled(false);
  ui->pushButtonPayer->setEnabled(false);

  setNom("");
  setMotDePasse("");
  ui->checkBoxNouveauClient->setCheckState(Qt::CheckState::Unchecked);

  setArticle("",-1.0,-1,"");

  w->videTablePanier();
  totalCaddie = 0.0;
  w->setTotal(-1.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles Table du panier (ne pas modifier) /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::ajouteArticleTablePanier(const char* article,float prix,int quantite)
{
    char Prix[20],Quantite[20];

    sprintf(Prix,"%.2f",prix);
    sprintf(Quantite,"%d",quantite);

    // Ajout possible
    int nbLignes = ui->tableWidgetPanier->rowCount();
    nbLignes++;
    ui->tableWidgetPanier->setRowCount(nbLignes);
    ui->tableWidgetPanier->setRowHeight(nbLignes-1,10);

    QTableWidgetItem *item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(article);
    ui->tableWidgetPanier->setItem(nbLignes-1,0,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(Prix);
    ui->tableWidgetPanier->setItem(nbLignes-1,1,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(Quantite);
    ui->tableWidgetPanier->setItem(nbLignes-1,2,item);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::videTablePanier()
{
    ui->tableWidgetPanier->setRowCount(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WindowClient::getIndiceArticleSelectionne()
{
    QModelIndexList liste = ui->tableWidgetPanier->selectionModel()->selectedRows();
    if (liste.size() == 0) return -1;
    QModelIndex index = liste.at(0);
    int indice = index.row();
    return indice;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions permettant d'afficher des boites de dialogue (ne pas modifier ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::dialogueMessage(const char* titre,const char* message)
{
   QMessageBox::information(this,titre,message);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::dialogueErreur(const char* titre,const char* message)
{
   QMessageBox::critical(this,titre,message);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////// CLIC SUR LA CROIX DE LA FENETRE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::closeEvent(QCloseEvent *event)
{
  // TO DO (étape 1)
  // Envoi d'une requete DECONNECT au serveur
    MESSAGE msg;
    msg.type = 1;
    msg.expediteur = getpid();
    msg.requete = LOGOUT;

    if(msgsnd(idQ, &msg, sizeof(MESSAGE) - sizeof(long), 0) == -1)
    {
        perror("Erreur msgsnd LOGOUT");
    }
    else
    {
        fprintf(stderr,"(CLIENT %d) Requête DECONNECT envoyée au serveur.\n", getpid());
    }
  
    msg.type = 1;
    msg.expediteur = getpid();
    msg.requete = DECONNECT;

    if(msgsnd(idQ, &msg, sizeof(MESSAGE) - sizeof(long), 0) == -1)
    {
        perror("Erreur msgsnd DECONNECT");
    }

  exit(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions clics sur les boutons ////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonLogin_clicked()
{
    // Envoi d'une requete de login au serveur
    // TO DO
    MESSAGE msg;
    msg.type = 1;
    msg.expediteur = getpid();
    msg.requete = LOGIN;

    msg.data1 = isNouveauClientChecked();
    const char* nom  = getNom();
    strncpy(msg.data2, nom, sizeof(msg.data2) - 1);
    msg.data2[sizeof(msg.data2) - 1] = '\0';

    const char* motdepasse  = getMotDePasse();
    strncpy(msg.data3, motdepasse, sizeof(msg.data2) - 1);
    msg.data3[sizeof(msg.data3) - 1] = '\0';

    if (msgsnd(idQ, &msg, sizeof(MESSAGE) - sizeof(long), 0) == -1) 
    {
        perror("Erreur msgsnd LOGIN");
        return;
    }

    fprintf(stderr, "(CLIENT %d) Requête LOGIN envoyée au serveur.\n", getpid());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonLogout_clicked()
{
    if (totalCaddie > 0) 
    {
        MESSAGE cancelAllReq;
        memset(&cancelAllReq, 0, sizeof(MESSAGE));
        cancelAllReq.type = 1;
        cancelAllReq.requete = CANCEL_ALL;
        cancelAllReq.expediteur = getpid(); 

        if (msgsnd(idQ, &cancelAllReq, sizeof(MESSAGE) - sizeof(long), 0) == -1) 
        {
            perror("(CLIENT) Erreur lors de l'envoi de la requête CANCEL_ALL");
            exit(1);
        }

        w->videTablePanier();
        totalCaddie = 0.0;
        w->setTotal(-1.0);
    }

    MESSAGE msg;
    msg.type = 1;
    msg.expediteur = getpid();
    msg.requete = LOGOUT;

    if (msgsnd(idQ, &msg, sizeof(MESSAGE) - sizeof(long), 0) == -1) 
    {
        perror("Erreur msgsnd LOGIN");
        return;
    }
    logoutOK();

    setNom("");
    setMotDePasse("");

    w->videTablePanier();
    totalCaddie = 0.0;
    w->setTotal(-1.0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonSuivant_clicked()
{
    // TO DO (étape 3)
    // Envoi d'une requete CONSULT au serveur
    if (articleEnCours.id > 0)
    { 
        MESSAGE msg;
        msg.type = 1;
        msg.expediteur = getpid();
        msg.data1 = articleEnCours.id + 1;
        msg.requete = CONSULT;
        msgsnd(idQ, &msg, sizeof(MESSAGE) - sizeof(long), 0); 
    }
    
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonPrecedent_clicked()
{
    // TO DO (étape 3)
    // Envoi d'une requete CONSULT au serveur
    if (articleEnCours.id > 1)  
    {
        MESSAGE msg;
        msg.type = 1;
        msg.expediteur = getpid();
        msg.data1 = articleEnCours.id - 1;
        msg.requete = CONSULT;
        msgsnd(idQ, &msg, sizeof(MESSAGE) - sizeof(long), 0);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonAcheter_clicked()
{
    // TO DO (étape 5)
    // Envoi d'une requete ACHAT au serveur

    MESSAGE msg;
    msg.type = 1;
    msg.expediteur = getpid();
    msg.data1 = articleEnCours.id;
    sprintf(msg.data2 , "%d", getQuantite());
    msg.requete = ACHAT;
    msgsnd(idQ, &msg, sizeof(MESSAGE) - sizeof(long), 0); 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonSupprimer_clicked()
{
    // TO DO (étape 6)
    // Envoi d'une requete CANCEL au serveur
    int indice = getIndiceArticleSelectionne();
    if (indice == -1) 
    {
        dialogueErreur("Erreur", "Aucun article sélectionné pour la suppression.");
        return;
    }
    MESSAGE msg;
    msg.type = 1;
    msg.expediteur = getpid();
    msg.data1 = indice;
    msg.requete = CANCEL;
    if (msgsnd(idQ, &msg, sizeof(MESSAGE) - sizeof(long), 0) == -1) 
    {
        perror("(CLIENT) Erreur lors de l'envoi de la requête CANCEL");
        return;
    }

    w->videTablePanier();
    totalCaddie = 0.0;
    w->setTotal(-1.0);


    MESSAGE caddie;
    caddie.type = 1;
    caddie.expediteur = getpid();
    caddie.requete = CADDIE;
    if (msgsnd(idQ, &caddie, sizeof(MESSAGE) - sizeof(long), 0) == -1) 
    {
        perror("(CLIENT) Erreur lors de l'envoi de la requête CADDIE");
        return;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonViderPanier_clicked()
{
    // TO DO (étape 6)
    // Envoi d'une requete CANCEL_ALL au serveur
    MESSAGE m;
    m.type = 1;  
    m.requete = CANCEL_ALL;  
    m.expediteur = getpid();  

    if (msgsnd(idQ, &m, sizeof(MESSAGE) - sizeof(long), 0) == -1) 
    {
        perror("(CLIENT) Erreur lors de l'envoi de la requête CANCEL_ALL");
        return;
    }

    // Mise à jour du caddie
    w->videTablePanier();
    totalCaddie = 0.0;
    w->setTotal(-1.0);



    // Envoi requete CADDIE au serveur
    MESSAGE caddieReq;
    caddieReq.type = 1;
    caddieReq.requete = CADDIE;
    caddieReq.expediteur = getpid();

    if (msgsnd(idQ, &caddieReq, sizeof(MESSAGE) - sizeof(long), 0) == -1) 
    {
        perror("(CLIENT) Erreur lors de l'envoi de la requête CADDIE");
        return;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonPayer_clicked()
{
    // TO DO (étape 7)
    // Envoi d'une requete PAYER au serveur
    if(totalCaddie>0)
    {
      MESSAGE m;
    memset(&m, 0, sizeof(MESSAGE));
    m.type = 1;  
    m.requete = PAYER;  
    m.expediteur = getpid();  

    if (msgsnd(idQ, &m, sizeof(MESSAGE) - sizeof(long), 0) == -1) 
    {
        perror("(CLIENT) Erreur lors de l'envoi de la requête PAYER");
        return;
    }

    char tmp[100];
    sprintf(tmp, "Merci pour votre paiement de %.2f ! Votre commande sera livrée tout prochainement.", totalCaddie);
    dialogueMessage("Payer...", tmp);

    w->videTablePanier();
    totalCaddie = 0.0;
    w->setTotal(-1.0);

    }
    else
    {
      w->dialogueErreur("Erreur Paiement","Votre panier est vide veuillez selectionner des articles\n");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Handlers de signaux ////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void handlerSIGUSR1(int sig)
{
    MESSAGE m;
  
    while(msgrcv(idQ,&m,sizeof(MESSAGE)-sizeof(long),getpid(),IPC_NOWAIT) != -1)  // !!! a modifier en temps voulu !!!
    {
      switch(m.requete)
      {
        case LOGIN :
                    if (m.data1 == 1) 
                    { 
                        printf("Connexion réussie ! Bienvenue, utilisateur.\n");
                        w->dialogueMessage("Bienvenue, utilisateur.",m.data4);
                        w->loginOK();
                        m.type = 1;
                        m.expediteur = getpid();
                        m.data1 = 1;
                        m.requete = CONSULT;
                        if (msgsnd(idQ, &m, sizeof(MESSAGE) - sizeof(long), 0) == -1) 
                        {
                            perror("Erreur msgsnd LOGIN");
                        }

                    } 
                    else 
                    {
                        w->dialogueErreur("Erreur",m.data4);
                    }
                                     
                    break;
        case CONSULT : // TO DO (étape 3)
                    articleEnCours.id = m.data1;
                    strcpy(articleEnCours.intitule, m.data2);
                    articleEnCours.prix = m.data5;
                    articleEnCours.stock = atoi(m.data3);
                    strcpy(articleEnCours.image, m.data4);
                    w->setArticle(articleEnCours.intitule,articleEnCours.prix,articleEnCours.stock,articleEnCours.image);
                    break;

        case ACHAT : // TO DO (étape 5)
                    if(atoi(m.data3) > 0)
                    {
                        char message[100];
                        snprintf(message, sizeof(message), "%s %s %s %s", m.data3, "unité(s) de", articleEnCours.intitule, "achetées avec succès");
                        w->dialogueMessage("succès", message);
                        
                        m.type = 1;
                        m.expediteur = getpid();
                        m.requete = CADDIE;
                        if (msgsnd(idQ, &m, sizeof(MESSAGE) - sizeof(long), 0) == -1) 
                        {
                            perror("Erreur msgsnd CADDIE");
                        }
                        w->videTablePanier();
                        totalCaddie = 0;

                    }
                    else
                        w->dialogueErreur("Erreur","Stock insuffisant !");
                    break;

         case CADDIE : // TO DO (étape 5)
                    w->ajouteArticleTablePanier(m.data2, m.data5, atoi(m.data3)); 
                    totalCaddie +=m.data5 * atoi(m.data3);
                    w->setTotal(totalCaddie);  
                    break;

         case TIME_OUT : // TO DO (étape 6)
                   {
                        fprintf(stderr, "TIME_OUT reçu !! \n");
    
                        w->logoutOK();

                        w->dialogueMessage("TIME_OUT", "vous avez été automatiquement déconnecté pour cause d’inactivité.");
                        
                      }
                    break;

         case BUSY : // TO DO (étape 7)
                    break;

         default :
                    break;
      }
    }
}
void handlerSIGUSR2(int sig) 
{
    pShm = (char *)shmat(idShm, NULL, 0);
    if (pShm == (char *)-1) {
        perror("Erreur shmat");
        exit(1);
    }
    if (pShm != NULL) 
    {
        w->setPublicite(pShm);
    }
    shmdt(pShm);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
