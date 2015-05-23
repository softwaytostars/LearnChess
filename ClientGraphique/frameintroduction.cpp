#include "frameintroduction.h"
#include "ui_frameintroduction.h"
#include "infosgraphiques.h"

FrameIntroduction::FrameIntroduction(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameIntroduction)
{
    ui->setupUi(this);
    InfosGraphiques::instance()->AbonnerUtilisateurTraductions(this);

    setLayout(ui->verticalLayout);
    ui->label->setObjectName("labelWelcome");
    ui->labelImage->setPixmap(QPixmap (":/ressources/Pieces2.jpg"));
    //ui->labelImage->setScaledContents(true);

    Translate();
}

FrameIntroduction::~FrameIntroduction()
{
    delete ui;
    InfosGraphiques::instance()->DesabonnerUtilisateurTraductions(this);
}

void FrameIntroduction::Translate() {

    ui->label->setText(tr("Welcome in LearnChess !"));
    ui->textBrowser->setText(tr("The purpose of this software is to teach the basics of the wonderful game of Chess. \n")
                            + tr("In this game, there is no room for luck, only tactics and strategy are important. \n \n")
                            + tr("At Chess, the player who wins is the one who is able to checkmate the ennemy King. \n")
                            + tr ("A King is said checkmate when : \n")
                            + tr("- he is attacked by an ennemy piece. \n")
                            + tr("- There is no friendly piece that can capture the attacking piece. \n")
                            + tr("- There is no friendly piece that can interpose between the King and the attacking piece. \n \n")
                            + tr("The software is organized into different tabs :\n")
                            + tr("- The Piece and moves tab allows to present the different pieces of the game and how they move and capture.\n")
                            + tr("- The Special moves tab presents special moves allowed.\n")
                            + tr("- The Checkmate patterns tab is very important. It describes some main configurations for mating the ennemy King.\n")
                            + tr("- The Tactics tab presents different tactics used in Chess.\n")
                            + tr("- The Games tab offers two sub-tabs. The first allows you to view games of great masters or other games that the user could add. The second lets you play against the computer.\n\n")
                            + tr("All tabs (except the last) propose a demonstration sub-tab and a training sub-tab that allows you to practice to make sure you understood well the demonstrations.")
                            + tr("Have fun!")
                             );
}


