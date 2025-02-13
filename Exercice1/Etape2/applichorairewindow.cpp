#include "applichorairewindow.h"
#include "ui_applichorairewindow.h"
#include "unistd.h"
#include <QDesktopWidget>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <iostream>
#include <sstream>
#include "Timetable.h"
using namespace planning;

using namespace std;

ApplicHoraireWindow::ApplicHoraireWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::ApplicHoraireWindow)
{
    ui->setupUi(this);
    ::close(2);

    // Configuration de la table des professeurs
    ui->tableWidgetProfesseurs->setColumnCount(3);
    ui->tableWidgetProfesseurs->setRowCount(0);
    QStringList labelsTableProfesseurs;
    labelsTableProfesseurs << "id" << "Nom" << "Prenom";
    ui->tableWidgetProfesseurs->setHorizontalHeaderLabels(labelsTableProfesseurs);
    ui->tableWidgetProfesseurs->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetProfesseurs->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetProfesseurs->horizontalHeader()->setVisible(true);
    ui->tableWidgetProfesseurs->horizontalHeader()->setDefaultSectionSize(100);
    ui->tableWidgetProfesseurs->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetProfesseurs->verticalHeader()->setVisible(false);
    ui->tableWidgetProfesseurs->horizontalHeader()->setStyleSheet("background-color: lightyellow");

    // Configuration de la table des groupes
    ui->tableWidgetGroupes->setColumnCount(2);
    ui->tableWidgetGroupes->setRowCount(0);
    QStringList labelsTableGroupes;
    labelsTableGroupes << "id" << "Nom";
    ui->tableWidgetGroupes->setHorizontalHeaderLabels(labelsTableGroupes);
    ui->tableWidgetGroupes->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->tableWidgetGroupes->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetGroupes->horizontalHeader()->setVisible(true);
    ui->tableWidgetGroupes->horizontalHeader()->setDefaultSectionSize(100);
    ui->tableWidgetGroupes->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetGroupes->verticalHeader()->setVisible(false);
    ui->tableWidgetGroupes->horizontalHeader()->setStyleSheet("background-color: lightyellow");

    // Configuration de la table des locaux
    ui->tableWidgetLocaux->setColumnCount(2);
    ui->tableWidgetLocaux->setRowCount(0);
    QStringList labelsTableLocaux;
    labelsTableLocaux << "id" << "Nom";
    ui->tableWidgetLocaux->setHorizontalHeaderLabels(labelsTableLocaux);
    ui->tableWidgetLocaux->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetLocaux->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetLocaux->horizontalHeader()->setVisible(true);
    ui->tableWidgetLocaux->horizontalHeader()->setDefaultSectionSize(100);
    ui->tableWidgetLocaux->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetLocaux->verticalHeader()->setVisible(false);
    ui->tableWidgetLocaux->horizontalHeader()->setStyleSheet("background-color: lightyellow");

    // Configuration de la table des cours
    ui->tableWidgetCourss->setColumnCount(8);
    ui->tableWidgetCourss->setRowCount(0);
    QStringList labelsTableCourss;
    labelsTableCourss << "code" << "Jour" << "Heure" << "Duree" << "Local" << "Intitule" << "Professeur" << "Groupes";
    ui->tableWidgetCourss->setHorizontalHeaderLabels(labelsTableCourss);
    ui->tableWidgetCourss->setSelectionMode(QAbstractItemView::NoSelection);
    //ui->tableWidgetCourss->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetCourss->horizontalHeader()->setVisible(true);
    ui->tableWidgetCourss->horizontalHeader()->setDefaultSectionSize(100);
    ui->tableWidgetCourss->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetCourss->verticalHeader()->setVisible(false);
    ui->tableWidgetCourss->horizontalHeader()->setStyleSheet("background-color: lightyellow");

    // Pour faire joli
    ui->pushButtonAjouterProfesseur->setStyleSheet("background-color: lightgreen");
    ui->pushButtonAjouterGroupe->setStyleSheet("background-color: lightgreen");
    ui->pushButtonAjouterLocal->setStyleSheet("background-color: lightgreen");
    ui->pushButtonSupprimerProfesseur->setStyleSheet("background-color: orange");
    ui->pushButtonSupprimerGroupe->setStyleSheet("background-color: orange");
    ui->pushButtonSupprimerLocal->setStyleSheet("background-color: orange");
    ui->pushButtonPlanifier->setStyleSheet("background-color: lightblue");
    ui->pushButtonSelectionner->setStyleSheet("background-color: lightblue");

    // On fixe la taille et la position de la fenetre
    setFixedSize(1070,647);
    move((QApplication::desktop()->width()-width())/2,(QApplication::desktop()->height()-height())/2);

    // Tests a supprimer ------------------------------------------------
    /*addTupleTableProfessors("1;Wagner;Jean-Marc");
    addTupleTableProfessors("4;Leonard;Anne");
    addTupleTableProfessors("6;Quettier;Patrick");

    addTupleTableGroups("4;INFO2_D201");
    addTupleTableGroups("6;INFO2_I201");
    addTupleTableGroups("7;INFO2_R201");
    addTupleTableGroups("9;INFO2_D202");
    addTupleTableGroups("10;INFO2_R202");

    addTupleTableClassrooms("23;AN");
    addTupleTableClassrooms("25;LP03");
    addTupleTableClassrooms("29;LE0");

    addTupleTableCourses("1;Lundi;8h30;2h00;AN;Théorie C++;Wagner Jean-Marc;INFO2 D201,INFO2 D202");
    addTupleTableCourses("3;Mardi;10h30;1h30;AN;Théorie UNIX;Quettier Patrick;INFO2 R201,INFO2 R202");
    addTupleTableCourses("4;Jeudi;13h30;2h00;LE0;Labo C++;Leonard Anne;INFO2 I201");*/
    // -------------------------------------------------------------------
}

ApplicHoraireWindow::~ApplicHoraireWindow() {
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles Table des professeurs /////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::addTupleTableProfessors(const string& tuple) {
    // Parsing du tuple recu
    istringstream ss(tuple);
    string token;

    string id, lastName, firstName;
    getline(ss,id,';');
    getline(ss,lastName,';');
    getline(ss,firstName,';');

    // Ajout dans la table
    int nbLignes = ui->tableWidgetProfesseurs->rowCount();
    nbLignes++;
    ui->tableWidgetProfesseurs->setRowCount(nbLignes);

    QTableWidgetItem *item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::fromStdString(id));
    ui->tableWidgetProfesseurs->setItem(nbLignes-1,0,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setText(QString::fromStdString(lastName));
    ui->tableWidgetProfesseurs->setItem(nbLignes-1,1,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setText(QString::fromStdString(firstName));
    ui->tableWidgetProfesseurs->setItem(nbLignes-1,2,item);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::clearTableProfessors() {
    ui->tableWidgetProfesseurs->setRowCount(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ApplicHoraireWindow::getIndexProfessorSelection() const {
    QModelIndexList liste = ui->tableWidgetProfesseurs->selectionModel()->selectedRows();
    if (liste.size() == 0) return -1;
    QModelIndex index = liste.at(0);
    int indice = index.row();
    return indice;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles Table des groupes /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::addTupleTableGroups(const string& tuple) {
    // Parsing du tuple recu
    istringstream ss(tuple);
    string token;

    string id, name;
    getline(ss,id,';');
    getline(ss,name,';');

    // Ajout possible
    int nbLignes = ui->tableWidgetGroupes->rowCount();
    nbLignes++;
    ui->tableWidgetGroupes->setRowCount(nbLignes);

    QTableWidgetItem *item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::fromStdString(id));
    ui->tableWidgetGroupes->setItem(nbLignes-1,0,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setText(QString::fromStdString(name));
    ui->tableWidgetGroupes->setItem(nbLignes-1,1,item);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::clearTableGroups() {
    ui->tableWidgetGroupes->setRowCount(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
list<int> ApplicHoraireWindow::getIndexesGroupsSelection() const {
    QModelIndexList liste = ui->tableWidgetGroupes->selectionModel()->selectedRows();
    list<int> result;
    for (int i=0 ; i<liste.size() ; i++) result.push_back(liste.at(i).row());
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles Table des locaux //////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::addTupleTableClassrooms(const string& tuple) {
    // Parsing du tuple recu
    istringstream ss(tuple);
    string token;

    string id, name;
    getline(ss,id,';');
    getline(ss,name,';');

    // Ajout possible
    int nbLignes = ui->tableWidgetLocaux->rowCount();
    nbLignes++;
    ui->tableWidgetLocaux->setRowCount(nbLignes);

    QTableWidgetItem *item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::fromStdString(id));
    ui->tableWidgetLocaux->setItem(nbLignes-1,0,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setText(QString::fromStdString(name));
    ui->tableWidgetLocaux->setItem(nbLignes-1,1,item);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::clearTableClassrooms() {
    ui->tableWidgetLocaux->setRowCount(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ApplicHoraireWindow::getIndexClassroomSelection() const {
    QModelIndexList liste = ui->tableWidgetLocaux->selectionModel()->selectedRows();
    if (liste.size() == 0) return -1;
    QModelIndex index = liste.at(0);
    int indice = index.row();
    return indice;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles Table des professeurs /////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::addTupleTableCourses(const string& tuple) {
    // Parsing du tuple recu
    istringstream ss(tuple);
    string token;

    string code, day, hour, duration, classroom, title, professor, groups;
    getline(ss,code,';');
    getline(ss,day,';');
    getline(ss,hour,';');
    getline(ss,duration,';');
    getline(ss,classroom,';');
    getline(ss,title,';');
    getline(ss,professor,';');
    getline(ss,groups,';');

    // Ajout possible
    int nbLignes = ui->tableWidgetCourss->rowCount();
    nbLignes++;
    ui->tableWidgetCourss->setRowCount(nbLignes);

    QTableWidgetItem *item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::fromStdString(code));
    ui->tableWidgetCourss->setItem(nbLignes-1,0,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setText(QString::fromStdString(day));
    ui->tableWidgetCourss->setItem(nbLignes-1,1,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setText(QString::fromStdString(hour));
    ui->tableWidgetCourss->setItem(nbLignes-1,2,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setText(QString::fromStdString(duration));
    ui->tableWidgetCourss->setItem(nbLignes-1,3,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setText(QString::fromStdString(classroom));
    ui->tableWidgetCourss->setItem(nbLignes-1,4,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setText(QString::fromStdString(title));
    ui->tableWidgetCourss->setItem(nbLignes-1,5,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setText(QString::fromStdString(professor));
    ui->tableWidgetCourss->setItem(nbLignes-1,6,item);

    item = new QTableWidgetItem;
    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    item->setText(QString::fromStdString(groups));
    ui->tableWidgetCourss->setItem(nbLignes-1,7,item);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::clearTableCourses() {
    ui->tableWidgetCourss->setRowCount(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions permettant d'afficher des boites de dialogue /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::dialogMessage(const string& title,const string& message) {
   QMessageBox::information(this,QString::fromStdString(title),QString::fromStdString(message));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::dialogError(const string& title,const string& message) {
   QMessageBox::critical(this,QString::fromStdString(title),QString::fromStdString(message));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
string ApplicHoraireWindow::dialogInputText(const string& title,const string& question) {
    return QInputDialog::getText(this,QString::fromStdString(title),QString::fromStdString(question)).toStdString();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ApplicHoraireWindow::dialogInputInt(const string& title,const string& question) {
    return QInputDialog::getInt(this,QString::fromStdString(title),QString::fromStdString(question));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
string ApplicHoraireWindow::dialogInputFileForLoad(const string& question)
{
  QString fileName = QFileDialog::getOpenFileName(this,QString::fromStdString(question), "", "Tous les fichiers (*.*)");
  return fileName.toStdString();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
string ApplicHoraireWindow::dialogInputFileForSave(const string& question)
{
  QString fileName = QFileDialog::getSaveFileName(this,QString::fromStdString(question), "", "Tous les fichiers (*.*)");
  return fileName.toStdString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles aux widgets de la fenetre /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
string ApplicHoraireWindow::getProfessorLastName() const {
    return ui->lineEditNomProfesseur->text().toStdString();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::setProfessorLastName(const string& lastName) {
    ui->lineEditNomProfesseur->setText(QString::fromStdString(lastName));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
string ApplicHoraireWindow::getProfessorFirstName() const {
    return ui->lineEditPrenomProfesseur->text().toStdString();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::setProfessorFirstName(const string& firstName) {
    ui->lineEditPrenomProfesseur->setText(QString::fromStdString(firstName));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
string ApplicHoraireWindow::getGroupName() const {
    return ui->lineEditNumeroGroupe->text().toStdString();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::clearGroupName() {
    ui->lineEditNumeroGroupe->setText("");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
string ApplicHoraireWindow::getClassroomName() const {
    return ui->lineEditNomLocal->text().toStdString();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::setClassroomName(const string& name) {
    ui->lineEditNomLocal->setText(QString::fromStdString(name));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
string ApplicHoraireWindow::getDaySelection() const {
    return ui->comboBoxJour->currentText().toStdString();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ApplicHoraireWindow::getHourStart() const {
    if (ui->lineEditHeuresDebut->text().toStdString() == "") return -1;
    return stoi(ui->lineEditHeuresDebut->text().toStdString());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::clearHourStart() {
    ui->lineEditHeuresDebut->setText("");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ApplicHoraireWindow::getMinuteStart() const {
    if (ui->lineEditMinutesDebut->text().toStdString() == "") return -1;
    return stoi(ui->lineEditMinutesDebut->text().toStdString());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::clearMinuteStart() {
    ui->lineEditMinutesDebut->setText("");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ApplicHoraireWindow::getDuration() const {
    if (ui->lineEditDuree->text().toStdString() == "") return -1;
    return stoi(ui->lineEditDuree->text().toStdString());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::clearDuration() {
    ui->lineEditDuree->setText("");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
string ApplicHoraireWindow::getTitle() const {
    return ui->lineEditIntitule->text().toStdString();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::setTitle(const string& title) {
    ui->lineEditIntitule->setText(QString::fromStdString(title));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ApplicHoraireWindow::isDayChecked() const {
    return ui->checkBoxJour->isChecked();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::setDayChecked(bool b) {
    ui->checkBoxJour->setChecked(b);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ApplicHoraireWindow::isProfessorChecked() const {
    return ui->checkBoxProfesseur->isChecked();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::setProfessorChecked(bool b) {
    ui->checkBoxProfesseur->setChecked(b);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ApplicHoraireWindow::isGroupChecked() const {
    return ui->checkBoxGroupe->isChecked();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::setGroupChecked(bool b) {
    ui->checkBoxGroupe->setChecked(b);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ApplicHoraireWindow::isClassroomChecked() const {
    return ui->checkBoxLocal->isChecked();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::setClassroomChecked(bool b) {
    ui->checkBoxLocal->setChecked(b);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions clics sur les boutons ////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_pushButtonAjouterProfesseur_clicked() {
    cout << "Clic sur bouton Ajouter Professeur" << endl;
    // TO DO (Etape 9)

    string lastName = getProfessorLastName();
    string firstName = getProfessorFirstName();

    if (lastName.empty()) 
    {
        dialogError("Erreur de PRENOM", "Le prénom du professeur ne peut pas être vide.");
        return;
    }
    else if(firstName.empty())
    {
        dialogError("Erreur de NOM", "Le nom du professeur ne peut pas être vide.");
        return;
    }

    Timetable::getInstance().addProfessor(lastName, firstName);

    clearTableProfessors(); 

    const auto& professors = Timetable::getInstance().getProfessors();
    for (const auto& professor : professors) 
    {
        addTupleTableProfessors(professor.tuple());
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_pushButtonAjouterGroupe_clicked() {
    cout << "Clic sur bouton Ajouter Groupe" << endl;
    // TO DO (Etape 9)
    string groupName = getGroupName();

    if (groupName.empty()) 
    {
        dialogError("Erreur de Nom", "Nom du groupe vide !");
        return;
    }

    Timetable::getInstance().addGroup(groupName);

    clearTableGroups();

    const auto& groups = Timetable::getInstance().getGroups(); 

    for (const auto& group : groups) 
    {
        addTupleTableGroups(group.tuple());
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_pushButtonAjouterLocal_clicked() {
    cout << "Clic sur bouton Ajouter Local" << endl;
    // TO DO (Etape 9)

    string classroomName = getClassroomName();

    if (classroomName.empty()) 
    {
        dialogError("Erreur", "Nom de la salle vide !");
        return;
    }

    int seatingCapacity = dialogInputInt("Capacité", "Entrez la capacité de la salle :");

    if (seatingCapacity <= 0) 
    {
        dialogError("Erreur", "Capacité invalide !");
        return;
    }

    Timetable::getInstance().addClassroom(classroomName, seatingCapacity);

    
    clearTableClassrooms();

    const auto& classrooms = Timetable::getInstance().getClassrooms(); 

    for (const auto& classroom : classrooms) 
    {
        addTupleTableClassrooms(classroom.tuple());
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_pushButtonSupprimerProfesseur_clicked() {
    cout << "Clic sur bouton Supprimer Professeur" << endl;
    // TO DO (Etape 9)

    int index = getIndexProfessorSelection();

    if (index == -1) 
    {
        dialogError("Erreur", "Aucun professeur sélectionné !");
        return;
    }

    Timetable::getInstance().deleteProfessorByIndex(index);

    clearTableProfessors();

    const auto& professors = Timetable::getInstance().getProfessors();

    for (const auto& professor : professors) 
    {
        addTupleTableProfessors(professor.tuple());
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_pushButtonSupprimerGroupe_clicked() {
    cout << "Clic sur bouton Supprimer Groupe" << endl;
    // TO DO (Etape 9)

    // Récupérer les indices des lignes sélectionnées
    list<int> indexes = getIndexesGroupsSelection();

    if (indexes.empty()) 
    {
        dialogError("Erreur", "Aucun groupe sélectionné !");
        return;
    }

    indexes.sort(greater<int>());  // il faut inverser les indexes dans le containers indexes pour une bonne suppression 

    for (int index : indexes) 
    {
        Timetable::getInstance().deleteGroupByIndex(index);
    }

    clearTableGroups();

    const auto& groups = Timetable::getInstance().getGroups();

    for (const auto& group : groups) 
    {
        addTupleTableGroups(group.tuple());
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_pushButtonSupprimerLocal_clicked() {
    cout << "Clic sur bouton Supprimer Local" << endl;
    // TO DO (Etape 9)

    int index = getIndexClassroomSelection();

    if (index == -1) 
    {
        dialogError("Erreur", "Aucun local sélectionné !");
        return;
    }

    Timetable::getInstance().deleteClassroomByIndex(index);

    clearTableClassrooms();

    const auto& classrooms = Timetable::getInstance().getClassrooms();

    for (const auto& classroom : classrooms) 
    {
        addTupleTableClassrooms(classroom.tuple());
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_pushButtonPlanifier_clicked()
{
    cout << "Clic sur bouton Planifier" << endl;
    try
    {
        // Vérifier la sélection d'un professeur
        int indexP = getIndexProfessorSelection();
        if (indexP == -1) 
        {
            dialogError("Erreur", "Veuillez sélectionner un professeur.");
            return;
        }

        // Récupérer l'ID du professeur à partir de l'index sélectionné
        const auto& professors = Timetable::getInstance().getProfessors();
        auto itProf = professors.begin();
        advance(itProf, indexP);
        int professorId = itProf->getId();

        // Vérifier la sélection d'un local
        int indexC = getIndexClassroomSelection();
        if (indexC == -1) 
        {
            dialogError("Erreur", "Veuillez sélectionner un local.");
            return;
        }

        // Récupérer l'ID du local à partir de l'index sélectionné
        const auto& classrooms = Timetable::getInstance().getClassrooms();
        auto itClass = classrooms.begin();
        std::advance(itClass, indexC);
        int classroomId = itClass->getId();

        // Vérifier la sélection des groupes
        list<int> groupIndexes = getIndexesGroupsSelection();
        if (groupIndexes.empty()) 
        {
            dialogError("Erreur", "Veuillez sélectionner au moins un groupe.");
            return;
        }

        // Récupérer les IDs des groupes à partir des indices sélectionnés
        const auto& groups = Timetable::getInstance().getGroups();
        list<int> groupIds;
        for (int index : groupIndexes) 
        {
            auto itGroup = groups.begin();
            advance(itGroup, index);
            groupIds.push_back(itGroup->getId());
        }

        // Récupérer les informations temporelles
        string day = getDaySelection();
        int startTimeH = getHourStart();
        int startTimeM = getMinuteStart();
        int durationTime = getDuration();

        Time start(startTimeH, startTimeM);      // Peut lancer une TimeException
        Time duration(durationTime);            // Peut lancer une TimeException
        Timing timing(day, start, duration);    // Peut lancer une TimingException

        // Vérifier l'intitulé du cours
        string courseTitle = getTitle();
        if (courseTitle.empty()) 
        {
            dialogError("Erreur", "Veuillez entrer un titre pour le cours.");
            return;
        }

        // Instancier un objet Course
        Course course;
        course.setTitle(courseTitle.c_str());
        course.setProfessorId(professorId);
        course.setClassroomId(classroomId);
        for (int id : groupIds) 
        {
            course.addGroupId(id);
        }

        // Planifier le cours
        Timetable::getInstance().schedule(course, timing);

        // Mettre à jour la table des cours
        addTupleTableCourses(Timetable::getInstance().tuple(course));
    }
    catch (const TimeException& e)
    {
        dialogError("Erreur de Temps","Professeur indisponible pour cet horaire");
    }
    catch (const TimingException& e)
    {
        dialogError("Erreur de Planification", "Professeur indisponible pour cet horaire");
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_pushButtonSelectionner_clicked()
{
    cout << "Clic sur bouton Selectionner" << endl;
    // TO DO (Etape 13)

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions clics sur les items de menu //////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_actionQuitter_triggered()
{
    cout << "Clic sur Menu Fichier --> Item Quitter" << endl;
    QApplication::quit();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_actionOuvrir_triggered()
{
    cout << "Clic sur Menu Fichier --> Item Ouvrir" << endl;
    // TO DO (Etape 10)

    string timetableName = dialogInputText("Ouvrir l'horaire", "Entrez le nom de l'horaire à ouvrir :");

    if (timetableName.empty()) 
    {
        dialogError("Erreur", "Le nom de l'horaire ne peut pas être vide.");
        return;
    }

    // Vérifier si le fichier existe
    ifstream testFile(timetableName + "_config.dat");
    if (!testFile) 
    {
        dialogError("Erreur", "Le fichier spécifié n'existe pas.");
        return;
    }
    testFile.close();
    
    Timetable::getInstance().load(timetableName);
    

    // Met à jour les tables

    clearTableProfessors();
    for (const auto& professor : Timetable::getInstance().getProfessors()) 
    {
        addTupleTableProfessors(professor.tuple());
    }
     clearTableGroups();
    for (const auto& group : Timetable::getInstance().getGroups()) 
    {
        addTupleTableGroups(group.tuple());
    }
    clearTableClassrooms();
    for (const auto& classroom : Timetable::getInstance().getClassrooms()) 
    {
        addTupleTableClassrooms(classroom.tuple());
    }
    clearTableCourses();
    for (const auto& course : Timetable::getInstance().getCourses()) 
    {
        addTupleTableCourses(Timetable::getInstance().tuple(course));
    }

    dialogMessage("Ouverture réussie", "L'horaire a été chargé avec succès.");

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_actionNouveau_triggered()
{
    cout << "Clic sur Menu Fichier --> Item Nouveau" << endl;
    // TO DO (Etape 10)

    //vider tous les coneneurs
    Timetable::getInstance().vider();

    Schedulable::setCurrentId(1);

    // Met à jour les tables
    
    clearTableProfessors();
    
    clearTableGroups();
    
    clearTableClassrooms();

    clearTableCourses();

    dialogMessage("Nouvel horaire", "Un nouvel horaire a été créé.");

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_actionEnregistrer_triggered()
{
    cout << "Clic sur Menu Fichier --> Item Enregistrer" << endl;
    // TO DO (Etape 10)

    string timetableName = dialogInputText("Enregistrer l'horaire", "Entrez le nom de l'horaire :");

    if (timetableName.empty()) 
    {
        dialogError("Erreur", "Le nom de l'horaire ne peut pas être vide.");
        return;
    }

    Timetable::getInstance().save(timetableName);
    dialogMessage("Enregistrement réussi", "L'horaire a été enregistré avec succès.");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_actionSupprimerProfesseur_triggered() {
    cout << "Clic sur Menu Supprimer --> Item Professeur" << endl;
    // TO DO (Etape 9)

    int id = dialogInputInt("Supprimer Professeur", "Entrez l'ID du professeur à supprimer :");

    Timetable::getInstance().deleteProfessorById(id);

    
    clearTableProfessors();

    const auto& professors = Timetable::getInstance().getProfessors();

    for (const auto& professor : professors) 
    {
        addTupleTableProfessors(professor.tuple());
    }


}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_actionSupprimerGroupe_triggered() {
    cout << "Clic sur Menu Supprimer --> Item Groupe" << endl;
    // TO DO (Etape 9)

    int id = dialogInputInt("Supprimer Groupe", "Entrez l'ID du groupe à supprimer :");

    Timetable::getInstance().deleteGroupById(id);

    clearTableGroups();

    const auto& groups = Timetable::getInstance().getGroups();

    for (const auto& group : groups) 
    {
        addTupleTableGroups(group.tuple());
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_actionSupprimerLocal_triggered() {
    cout << "Clic sur Menu Supprimer --> Item Local" << endl;
    // TO DO (Etape 9)

    int id = dialogInputInt("Supprimer Local", "Entrez l'ID du local à supprimer :");

    Timetable::getInstance().deleteClassroomById(id);

    clearTableClassrooms();

    const auto& classrooms = Timetable::getInstance().getClassrooms();

    for (const auto& classroom : classrooms) 
    {
        addTupleTableClassrooms(classroom.tuple());
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_actionSupprimerCours_triggered()
{
    cout << "Clic sur Menu Supprimer --> Item Cours" << endl;
    // TO DO (Etape 11)

    int courseCode = dialogInputInt("Supprimer Cours", "Entrez le code du cours à supprimer :");

    // Récupérer l'instance de Timetable
    Timetable& timetable = Timetable::getInstance();

    // Parcourir le conteneur des cours pour trouver le cours à supprimer
    auto& courses = timetable.getCourses();
    
    bool trouver = false;

    for (auto it = courses.begin();it != courses.end(); ++it) 
    {
        if (it->getCode() == courseCode) 
        {
            trouver = true;
            courses.erase(it);
            break;
        }
    }

    if (trouver==false) 
    {
        dialogError("Erreur", "Aucun cours trouvé avec ce code.");
        return;
    }


    // Mettre à jour la table des cours
    clearTableCourses(); 
    for (const auto& course : courses) 
    {
        addTupleTableCourses(timetable.tuple(course));
    }

    dialogMessage("Succès", "Le cours a été supprimé avec succès.");

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_actionImporterProfesseurs_triggered()
{
    cout << "Clic sur Menu Importer --> Item Professeurs" << endl;
    // TO DO (Etape 12)
    
    string filename = dialogInputFileForLoad("Entrez le nom du fichier CSV pour les professeurs :");
    if (filename.empty())
    {
        dialogError("Erreur Fichier","Veuillez entrer un fichier valide");
        return;
    }

    Timetable::getInstance().importProfessorsFromCsv(filename);

    clearTableProfessors();
    const auto& professors = Timetable::getInstance().getProfessors();

    for (const auto& professor : professors) 
    {
        addTupleTableProfessors(professor.tuple());
    }

    dialogMessage("Succès", "Les professeurs ont été importés avec succès.");
    

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_actionImporterGroupes_triggered()
{
    cout << "Clic sur Menu Importer --> Item Groupes" << endl;
    // TO DO (Etape 12)

    string filename = dialogInputFileForLoad("Entrez le nom du fichier CSV pour les groupes :");

    if (filename.empty())
    {
        return;
    }

    Timetable::getInstance().importGroupsFromCsv(filename);

    clearTableGroups();
    const auto& groups = Timetable::getInstance().getGroups();

    for (const auto& group : groups) 
    {
        addTupleTableGroups(group.tuple());
    }

    dialogMessage("Succès", "Les groupes ont été importés avec succès.");

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_actionImporterLocaux_triggered()
{
    cout << "Clic sur Menu Importer --> Item Locaux" << endl;
    // TO DO (Etape 12)

    string filename = dialogInputFileForLoad("Entrez le nom du fichier CSV pour les locaux :");
    if (filename.empty())
    {
        return;
    } 

    Timetable::getInstance().importClassroomsFromCsv(filename);

    clearTableClassrooms();
    const auto& classrooms = Timetable::getInstance().getClassrooms();

    for (const auto& classroom : classrooms) 
    {
        addTupleTableClassrooms(classroom.tuple());
    }

    dialogMessage("Succès", "Les locaux ont été importés avec succès.");

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_actionExporterProfesseur_triggered()
{
    cout << "Clic sur Menu Exporter horaire --> Item Professeur" << endl;
    // TO DO (Etape 12)

    int id = dialogInputInt("Exporter Horaire", "Entrez l'ID du professeur à exporter :");
    Timetable::getInstance().exportProfessorTimetable(id);
    dialogMessage("Exportation réussie", "L'horaire a été exporté avec succès.");

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_actionExporterGroupe_triggered()
{
    cout << "Clic sur Menu Exporter horaire --> Item Groupe" << endl;
    // TO DO (Etape 12)

    int id = dialogInputInt("Exporter Horaire", "Entrez l'ID du groupe à exporter :");
    Timetable::getInstance().exportGroupTimetable(id);
    dialogMessage("Exportation réussie", "L'horaire du groupe a été exporté avec succès.");

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ApplicHoraireWindow::on_actionExporterLocal_triggered()
{
    cout << "Clic sur Menu Exporter horaire --> Item Local" << endl;
    // TO DO (Etape 12)

    int id = dialogInputInt("Exporter Horaire", "Entrez l'ID du local à exporter :");
    Timetable::getInstance().exportClassroomTimetable(id);
    dialogMessage("Exportation réussie", "L'horaire du local a été exporté avec succès.");

}
