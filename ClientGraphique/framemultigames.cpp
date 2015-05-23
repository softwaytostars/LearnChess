#include "framemultigames.h"
#include "ui_framemultigames.h"
#include "chessboardview.h"
#include "decochessboardviewwithrotation.h"
#include "infosgraphiques.h"

FrameMultiGames::FrameMultiGames(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameMultiGames)
{
    ui->setupUi(this);
    InfosGraphiques::instance()->AbonnerUtilisateurTraductions(this);

    setLayout(ui->MainVLayout);
    int lnbre = 0;
    ui->comboBox->insertItem (lnbre, "1");
    lnbre++;
    ui->comboBox->insertItem (lnbre, "2");
    lnbre++;
    ui->comboBox->insertItem (lnbre, "3");
    lnbre++;
    ui->comboBox->insertItem (lnbre, "4");
    lnbre++;
    ui->comboBox->insertItem (lnbre, "5");
    lnbre++;
    ui->comboBox->insertItem (lnbre, "6");
    ui->comboBox->setCurrentIndex(0);//prend un seul frame par defaut

    Translate();
}

FrameMultiGames::~FrameMultiGames()
{
    for (int i=0; i < ui->gridLayout->count(); i++) {
        if (!ui->gridLayout->itemAt(i)) continue;
        QWidget *lwidget = ui->gridLayout->itemAt(i)->widget();
        if (lwidget)
            delete lwidget;
    }
    delete ui;
    InfosGraphiques::instance()->DesabonnerUtilisateurTraductions(this);
}

void FrameMultiGames::Translate() {

    ui->comboBox->setToolTip(tr("Select the number of boards you want"));
}

void FrameMultiGames::on_comboBox_currentIndexChanged(const QString &arg1)
{
    bool lOk;
    int lnbreFrames = arg1.toInt(&lOk);
    if (!lOk) return;
    int lrow, lcol;

    //regarde les frames à supprimer
    QVector<QWidget *> lRemovalListe;
    if (lnbreFrames < ui->gridLayout->count()) {
        for (int i=lnbreFrames; i < ui->gridLayout->count(); i++) {
            if (!ui->gridLayout->itemAt(i)) continue;
            QWidget *lwidget = ui->gridLayout->itemAt(i)->widget();
            if (lwidget)
                lRemovalListe.append(lwidget);
        }
    }

    for (QVector<QWidget *>::const_iterator it=lRemovalListe.begin();
         it != lRemovalListe.end();
         ++it) {
        ui->gridLayout->removeWidget(*it);
        delete (*it);
    }

    for (int i = 0; i< lnbreFrames; i++) {

        lrow = i/2;//on impose que 2 colonnes maxi
        lcol = i%2;
        if (ui->gridLayout->itemAt(i)) continue;
        ui->gridLayout->addWidget(new ChessBoardViewWithRotation (new ChessBoardView (0)), lrow, lcol);
    }
}

void FrameMultiGames::ReInit () {

    for (int i=0; i < ui->gridLayout->count(); i++) {

        if (!ui->gridLayout->itemAt(i)) continue;
        QWidget *lwidget = ui->gridLayout->itemAt(i)->widget();
        IChessBoardView * lView=0;
        if (lwidget) {
            lView = dynamic_cast<IChessBoardView *>(lwidget);
            if (lView) lView->EmptyBoard();
        }
    }
}
