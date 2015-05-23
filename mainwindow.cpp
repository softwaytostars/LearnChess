#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

#include "chessboardview.h"
#include "infosgraphiques.h"
#include "infosconfiguration.h"
#include "implinitiationdemomove.h"
#include "implinitiationtrainingmove.h"
#include "ClientGraphique/frameintroduction.h"
#include "ClientGraphique/framevisualizegames.h"
#include "ClientGraphique/frameplayagainstcomputer.h"
#include "ClientGraphique/IActualisable.hpp"
#include "TypeDemo.hpp"
#include "frameinitiation.h"
#include "IControleur.h"
#include "Preferences/applicationstyle.h"
#include "Preferences/applicationlanguage.h"

// Initialisation du singleton a  0
MainWindow* MainWindow::_instance = 0;

MainWindow* MainWindow::instance () {
    if (!_instance) {
        _instance   = new MainWindow();
    }
    return _instance;
}

void MainWindow::kill () {
    if (_instance) {
        delete _instance;
        _instance = 0;
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    setWindowTitle ("LearnChess");

    ui->statusBar->setVisible(false);
    ui->mainToolBar->setVisible(false);

    setWindowIcon (QIcon (":/ressources/60859.ico"));

    ui->tabWidget->setFont(QFont ("Arial", 10));
    ui->tabWidget->clear();
    ui->tabWidget->setElideMode(Qt::ElideNone);
    connect (ui->tabWidget, SIGNAL (currentChanged (int)), this, SLOT (OngletChanged(int)));

    //onglet pour introduction
    _FrameIntroduction = new FrameIntroduction (this);
    ui->tabWidget->addTab(_FrameIntroduction, "Introduction");

    //onglet pour découverte pieces et mouvements
    _OngletsPiecesAndMoves = new QTabWidget(ui->tabWidget);
    ui->tabWidget->addTab(_OngletsPiecesAndMoves, "Pieces and Moves");

    //onglet pour decouverte mvts speciaux
    _OngletsSpecialMoves = new QTabWidget(ui->tabWidget);
    ui->tabWidget->addTab(_OngletsSpecialMoves, "Special Moves");

    //onglet pour découverte tactique
    _OngletsSchemasMat = new QTabWidget(ui->tabWidget);
    ui->tabWidget->addTab(_OngletsSchemasMat, "Checkmate patterns");

    //onglet pour découverte tactique
    _OngletsTactics = new QTabWidget(ui->tabWidget);
    ui->tabWidget->addTab(_OngletsTactics, "Some tactics");

//    //onglet pour découverte stratégie
//    QTabWidget* lOngletsStrategie = new QTabWidget(ui->tabWidget);
//    ui->tabWidget->addTab(lOngletsStrategie, tr("Some strategy"));

    //onglet pour voir parties
    _OngletsParties = new QTabWidget(ui->tabWidget);
    ui->tabWidget->addTab(_OngletsParties, "Games");


    /**** Ajout des sous onglets de lOngletsPiecesAndMoves ****/
    _FrameInitiationDemoMove = new FrameInitiation(eInitierPiecesMoves, new ImplInitiationDemoMove(), this);
    _OngletsPiecesAndMoves->addTab(_FrameInitiationDemoMove,
                                   "Demos");
    _FrameInitiationTrainingMove = new FrameInitiation(eInitierPiecesMoves, new ImplInitiationTrainingMove(), this);
    _OngletsPiecesAndMoves->addTab(_FrameInitiationTrainingMove,
                                   "Training");

    /**** Ajout des sous onglets de lOngletsSpecialMoves ****/
    _FrameInitiationDemoSpecialMove = new FrameInitiation(eInitierSpecialMoves, new ImplInitiationDemoMove(), this);
    _OngletsSpecialMoves->addTab(_FrameInitiationDemoSpecialMove,
                                 "Demos");
    _FrameInitiationTrainingSpecialMove = new FrameInitiation(eInitierSpecialMoves, new ImplInitiationTrainingMove(), this);
    _OngletsSpecialMoves->addTab(_FrameInitiationTrainingSpecialMove,
                                 "Training");

    /**** Ajout des sous onglets de lOngletsSchemasMat ****/
    _FrameInitiationDemoSchemaMat = new FrameInitiation(eInitierPatternsMat, new ImplInitiationDemoMove(), this);
    _OngletsSchemasMat->addTab(_FrameInitiationDemoSchemaMat,
                                 "Demos");
    _FrameInitiationTrainingSchemaMat = new FrameInitiation(eInitierPatternsMat, new ImplInitiationTrainingMove(), this);
    _OngletsSchemasMat->addTab(_FrameInitiationTrainingSchemaMat,
                                 "Training");

    /**** Ajout des sous onglets de lOngletsTactics ****/
    _FrameInitiationDemoTactiques = new FrameInitiation(eInitierTactiques, new ImplInitiationDemoMove(), this);
    _OngletsTactics->addTab(_FrameInitiationDemoTactiques,
                            "Demos");
    _FrameInitiationTrainingTactiques = new FrameInitiation(eInitierTactiques, new ImplInitiationTrainingMove(), this);
    _OngletsTactics->addTab(_FrameInitiationTrainingTactiques,
                            "Training");

    /**** Ajout des sous onglets de lOngletsParties ****/
    _FrameVisualizeGames = new FrameVisualizeGames(this);
    _OngletsParties->addTab(_FrameVisualizeGames,
                            "Visualize Games");
    _FramePlayAgainstComputer = new FramePlayAgainstComputer (this);
    _OngletsParties->addTab(_FramePlayAgainstComputer,
                            "Play against computer");


    /* creation de menus */
    createActionsForFileMenu();
    _fileMenu = menuBar()->addMenu (tr("&File"));
    _fileMenu->addAction(_ActionExit);
    _fileMenu->addSeparator();
    _fileMenu->addAction(_ActionChangeDirForPGNGames);

    //Menu preference
    _PrefMenu = menuBar()->addMenu (tr("&Preferences"));
    //option choix des types de pieces
    int lPrefTypePieceActual = InfosConfiguration::instance()->getLastTypePieces();
    _MenuChoixTypePieces = _PrefMenu->addMenu(tr("&Choice type of pieces"));
    //sous-menus du menu choix des pieces
    tMapPrefPieceName lMapPrefPieces = InfosGraphiques::instance()->get_MapPrefTypePiecesName ();
    for (tMapPrefPieceName::const_iterator itPref=lMapPrefPieces.begin();
         itPref!=lMapPrefPieces.end();
         ++itPref) {
        QAction *lActionChoixPieces = _MenuChoixTypePieces->addAction(itPref.value());
        lActionChoixPieces->setCheckable(true);
        lActionChoixPieces->setData((int)itPref.key());
        lActionChoixPieces->setChecked(lPrefTypePieceActual == (int) itPref.key());
    }
    connect(_MenuChoixTypePieces, SIGNAL(triggered(QAction*)), this, SLOT(ChangeTypePieces(QAction*)));

    _PrefMenu->addSeparator();

    //option choix du style de l'application
     int lPrefTypeStyleActual = InfosConfiguration::instance()->getLastStyleApp();
    _MenuChoixTypeStyleApplication = _PrefMenu->addMenu (tr("&Choice of application colors"));
    //sous-menus du menu choix du style de l'application
    tMapPrefStyleAppName lMapPrefStyle = InfosGraphiques::instance()->get_MapPrefStyleAppName ();
    for (tMapPrefStyleAppName::const_iterator itPref=lMapPrefStyle.begin();
         itPref!=lMapPrefStyle.end();
         ++itPref) {
        QAction *lActionChoixStyle = _MenuChoixTypeStyleApplication->addAction(itPref.value());
        lActionChoixStyle->setCheckable(true);
        lActionChoixStyle->setData((int)itPref.key());
        lActionChoixStyle->setChecked(lPrefTypeStyleActual == (int) itPref.key());
    }
    connect(_MenuChoixTypeStyleApplication, SIGNAL(triggered(QAction*)), this, SLOT(ChangeStyleAppli(QAction*)));


    _PrefMenu->addSeparator();
    //option choix du langage de l'application
    QString lPrefLangageToStrActual = InfosConfiguration::instance()->getLastLanguageAppToStr();
    _MenuChoixLangueApplication = _PrefMenu->addMenu (tr("&Choice of language"));
    //sous-menus du menu choix du style de l'application
    tMapPrefLangueName lMapPrefLangue = InfosConfiguration::instance()->get_MapPrefLangueName();
    for (tMapPrefLangueName::const_iterator itPref=lMapPrefLangue.begin();
         itPref!=lMapPrefLangue.end();
         ++itPref) {
        QAction *lActionChoixLangue = _MenuChoixLangueApplication->addAction(itPref.value());
        lActionChoixLangue->setCheckable(true);
        lActionChoixLangue->setData((int)itPref.key());
        lActionChoixLangue->setChecked(lPrefLangageToStrActual == (QString) itPref.value());
    }
    connect(_MenuChoixLangueApplication, SIGNAL(triggered(QAction*)), this, SLOT(ChangeLangueAppli(QAction*)));


    /* menu options */
    _OptionsMenu = menuBar()->addMenu (tr("&Options"));
    _ActionMenuModeConfigurator = _OptionsMenu->addAction(tr("&Make Configurations"));
    _ActionMenuModeConfigurator->setCheckable(true);
    _ActionMenuModeConfigurator->setChecked(InfosConfiguration::instance()->getModeConfigurateur());
    connect(_ActionMenuModeConfigurator, SIGNAL(triggered()), this, SLOT(ChangeModeConfigurateur()));

    //menu infos
    _InfosMenu = menuBar()->addMenu (tr("&Infos"));
    QAction*lActionVersion = _InfosMenu->addAction(tr("&Version"));
    connect(lActionVersion, SIGNAL(triggered()), this, SLOT(ShowVersion()));

    setCentralWidget(ui->tabWidget);

    //connecte les sous onglets
    for (int i=0; i < ui->tabWidget->count(); i++) {
        if (dynamic_cast<QTabWidget*> (ui->tabWidget->widget(i)))
            connect (ui->tabWidget->widget(i), SIGNAL (currentChanged (int)), this, SLOT (OngletChanged(int)));
    }

    translate();

    ActualiserAffichageCourant ();
}

void MainWindow::ActualiserAffichageCourant () {

    IActualisable* lFrame = dynamic_cast<IActualisable*> (getCurrentWidget (ui->tabWidget));
    if (lFrame)
        lFrame->Actualiser();
}

void MainWindow::OngletChanged (int ) {

    ActualiserAffichageCourant ();
}

void MainWindow::Exit () {

    qApp->exit();
}

void MainWindow::ChangePGNDir () {

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     InfosConfiguration::instance()->getLastDirForPGNFiles(),
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
    if (dir.size() != 0) {
        InfosConfiguration::instance()->setLastDirForPGNFiles(dir);
        ActualiserAffichageCourant ();
    }
}

void MainWindow::ChangeTypePieces (QAction *aAction) {

    if (aAction) {
        bool lOk;
        int lPrefTypePieces = aAction->data().toInt(&lOk);
        if (lOk) {
            QAction *lAction =0;
            foreach(lAction, _MenuChoixTypePieces->actions())
                if (lAction) lAction->setChecked(false);

            aAction->setChecked(true);
            InfosConfiguration::instance()->setLastTypePieces((ePreferencesPieces) lPrefTypePieces);
            InfosGraphiques::instance()->RechargerPiecesGraphiquesDesAffichages();
        }
    }

}

void MainWindow::ChangeStyleAppli (QAction *aAction) {

    if (aAction) {
        bool lOk;
        int lPrefStyleApp = aAction->data().toInt(&lOk);
        if (lOk) {
            QAction *lAction =0;
            foreach(lAction, _MenuChoixTypeStyleApplication->actions())
                if (lAction) lAction->setChecked(false);

            aAction->setChecked(true);
            InfosConfiguration::instance()->setLastStyleApp((ePreferenceStyle) lPrefStyleApp);
            ApplicationStyle lAppStyle;
            lAppStyle.SetStyle ((ePreferenceStyle)lPrefStyleApp);
        }
    }
}

void MainWindow::ChangeLangueAppli (QAction *aAction) {

    if (aAction) {
        bool lOk;
        int lPrefLangueApp = aAction->data().toInt(&lOk);
        if (lOk) {
            QAction *lAction =0;
            foreach(lAction, _MenuChoixLangueApplication->actions())
                if (lAction) lAction->setChecked(false);

            aAction->setChecked(true);
            InfosConfiguration::instance()->setLanguage((eLangue) lPrefLangueApp);
            ApplicationLanguage lAppLangue;
            //l'appel au changer de translator va appeler l'evenement ChangeEvent.
            lAppLangue.SetLanguage(InfosConfiguration::instance()->getLastLanguageAppToStr());
        }
    }

}

void MainWindow::ShowVersion () {

    //show dialog info
    QMessageBox msg;
    msg.setText("LearnChess v1.0\nBased on Qt 4.8.3.");

    msg.exec();
}

void MainWindow::ChangeModeConfigurateur () {

    InfosConfiguration::instance()->setModeConfigurateur(!InfosConfiguration::instance()->getModeConfigurateur());
    ActualiserAffichageCourant ();
}


QWidget * MainWindow::getCurrentWidget(QTabWidget* atab) {

    QWidget *lWidget = atab->currentWidget();
    QTabWidget* lsubTab =0;
    if (lWidget && (lsubTab=dynamic_cast <QTabWidget*> (lWidget)))
        return (getCurrentWidget (lsubTab));
    return lWidget;
}

void MainWindow::createActionsForFileMenu() {

    _ActionExit = new QAction (tr("&Exit"), this);
    _ActionExit->setShortcuts (QKeySequence::Close);
    _ActionExit->setToolTip (tr("Close the application"));
    connect(_ActionExit, SIGNAL(triggered()), this, SLOT(Exit()));


    _ActionChangeDirForPGNGames = new QAction (tr("&Change default PGN repertory"), this);
    _ActionChangeDirForPGNGames->setShortcuts (QKeySequence::Underline);
    connect(_ActionChangeDirForPGNGames, SIGNAL(triggered()), this, SLOT(ChangePGNDir()));


}

MainWindow::~MainWindow()
{
 //   delete _onglets;
    delete _fileMenu;
    delete ui;
    InfosGraphiques::kill();
    InfosConfiguration::kill();
}

void MainWindow::changeEvent(QEvent *event) {

    //c'est ici que l'on fait appel au translate des form
    if (event->type() == QEvent::LanguageChange) {

        translate ();
        InfosGraphiques::instance()->RechargerTraductions ();
    }

    QMainWindow::changeEvent(event);

}

void MainWindow::translate () {

    ui->tabWidget->setTabText(ui->tabWidget->indexOf(_FrameIntroduction), tr("Introduction"));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(_OngletsPiecesAndMoves), tr("Pieces and Moves"));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(_OngletsSpecialMoves), tr("Special Moves"));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(_OngletsSchemasMat), tr("Checkmate patterns"));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(_OngletsTactics), tr("Some tactics"));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(_OngletsParties), tr("Games"));

    _OngletsPiecesAndMoves->setTabText(_OngletsPiecesAndMoves->indexOf(_FrameInitiationDemoMove), tr("Demos"));
    _OngletsPiecesAndMoves->setTabText(_OngletsPiecesAndMoves->indexOf(_FrameInitiationTrainingMove), tr("Training"));

    _OngletsSpecialMoves->setTabText(_OngletsSpecialMoves->indexOf(_FrameInitiationDemoSpecialMove), tr("Demos"));
    _OngletsSpecialMoves->setTabText(_OngletsSpecialMoves->indexOf(_FrameInitiationTrainingSpecialMove), tr("Training"));

    _OngletsSchemasMat->setTabText(_OngletsSchemasMat->indexOf(_FrameInitiationDemoSchemaMat), tr("Demos"));
    _OngletsSchemasMat->setTabText(_OngletsSchemasMat->indexOf(_FrameInitiationTrainingSchemaMat), tr("Training"));

    _OngletsTactics->setTabText(_OngletsTactics->indexOf(_FrameInitiationDemoTactiques), tr("Demos"));
    _OngletsTactics->setTabText(_OngletsTactics->indexOf(_FrameInitiationTrainingTactiques), tr("Training"));

    _OngletsParties->setTabText(_OngletsParties->indexOf(_FrameVisualizeGames), tr("Visualize Games"));
    _OngletsParties->setTabText(_OngletsParties->indexOf(_FramePlayAgainstComputer), tr("Play against computer"));

    _fileMenu->setTitle (tr("&File"));
    _PrefMenu->setTitle (tr("&Preferences"));
    _MenuChoixTypePieces->setTitle (tr("&Choice type of pieces"));
    _MenuChoixTypePieces->setToolTip(tr("Choose a new set of graphics for pieces"));
    _MenuChoixTypeStyleApplication->setTitle (tr("&Choice of application colors"));
    _MenuChoixTypeStyleApplication->setToolTip(tr("Choose a set of colors for the whole application"));
    _MenuChoixLangueApplication->setTitle (tr("&Choice of language"));
    _MenuChoixLangueApplication->setToolTip(tr("Choose a language for the whole application"));

    _ActionMenuModeConfigurator->setText(tr("&Make Configurations"));
    _ActionMenuModeConfigurator->setToolTip(tr("This option allows to create new demonstrations and trainings."));

    _ActionExit->setText (tr("&Exit"));

    _ActionChangeDirForPGNGames->setText(tr("&Change default PGN repertory"));
    _ActionChangeDirForPGNGames->setToolTip(tr("Choose a new directory for looking PGN files"));

}

