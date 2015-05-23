#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Types/TypePreferences.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    static MainWindow* instance ();
    static void kill ();
    void ActualiserAffichageCourant ();
protected:
    void changeEvent(QEvent *event);
private:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static MainWindow* _instance;

    Ui::MainWindow *ui;
    QTabWidget *_onglets;
    QMenu *_fileMenu,
          *_PrefMenu,
          *_MenuChoixTypePieces,
          *_MenuChoixTypeStyleApplication,
          *_MenuChoixLangueApplication,
          *_OptionsMenu,
          *_InfosMenu;
    QAction *_ActionExit,
            *_ActionChangeDirForPGNGames,
            *_ActionMenuModeConfigurator;

    //les onglets
    QTabWidget *_OngletsPiecesAndMoves,
               *_OngletsSpecialMoves,
               *_OngletsSchemasMat,
               *_OngletsTactics,
               *_OngletsParties;
    QWidget *_FrameIntroduction,
            *_FrameInitiationDemoMove,
            *_FrameInitiationTrainingMove,
            *_FrameInitiationDemoSpecialMove,
            *_FrameInitiationTrainingSpecialMove,
            *_FrameInitiationDemoSchemaMat,
            *_FrameInitiationTrainingSchemaMat,
            *_FrameInitiationDemoTactiques,
            *_FrameInitiationTrainingTactiques,
            *_FrameVisualizeGames,
            *_FramePlayAgainstComputer;

    void createActionsForFileMenu();
    QWidget * getCurrentWidget(QTabWidget* atab);
    void translate ();
private slots:
    void Exit ();
    void ChangePGNDir ();
    void OngletChanged (int );
    void ChangeTypePieces  (QAction *aAction);
    void ChangeStyleAppli  (QAction *aAction);
    void ChangeLangueAppli (QAction *aAction);
    void ShowVersion ();
    void ChangeModeConfigurateur ();
};

#endif // MAINWINDOW_H
