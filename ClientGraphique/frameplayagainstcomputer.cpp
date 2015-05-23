#include "frameplayagainstcomputer.h"
#include "ui_frameplayagainstcomputer.h"
#include "decochessboardviewwithrotation.h"
#include "chessboardview.h"
#include "IControleur.h"
#include "infosgraphiques.h"
#include <QMessageBox>

FramePlayAgainstComputer::FramePlayAgainstComputer(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FramePlayAgainstComputer),
    _ChessView(0),
    _EtatAlgo(eAlgoNone)
{
    ui->setupUi(this);
    InfosGraphiques::instance()->AbonnerUtilisateurTraductions(this);

    setLayout (ui->MainverticalLayout);

    ui->groupBoxChoixPlayer->setLayout(ui->HLayoutRadios);
    ui->HLayoutRadios->setAlignment (Qt::AlignLeft | Qt::AlignTop);
    ui->radioButtonWhite->setChecked(true);
    ui->progressBar->setVisible(false);

    ui->horizontalLayoutButtons->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->Button_StopAlgo->setEnabled(false);
    ui->Button_Init->setEnabled(false);
    ui->label->setVisible(false);

    Translate();

}

void FramePlayAgainstComputer::Actualiser() {

    if (!_ChessView) {
        _ChessView = new ChessBoardViewWithRotation (new ChessBoardView(0));
        _ChessView->setUseInfosAllowMoves (true);//permet de voir coche permettant de visualiser moves autorises
        _ChessView->FillBoard ();//remplit l'echiquier
        ui->HLayoutForChessBoard->addWidget(_ChessView);
        on_radioButtonWhite_clicked();
    }
}

FramePlayAgainstComputer::~FramePlayAgainstComputer()
{
    delete ui;
    InfosGraphiques::instance()->DesabonnerUtilisateurTraductions(this);
}

void FramePlayAgainstComputer::Translate() {

    ui->groupBoxChoixPlayer->setTitle(tr("Choice for player"));
    ui->radioButtonWhite->setText(tr("White"));
    ui->radioButtonBlack->setText(tr("Black"));
    ui->Button_StopAlgo->setText(tr("Stop computation"));
    ui->Button_Init->setText(tr("New Game"));
    _MessageGagne = tr("Computer wins!");
    _MessagePerd  = tr(" Congratulations, Computer is mate!");
    _MessageNulle = tr("1/2-1/2 It is a stalemate!");

    //pour remettre les traducs
    switch (_EtatAlgo) {

        case eAlgoLost  : DoOnCheckMate(); break;
        case eAlgoWins  : DoOnAlgoVictorious (); break;
        case eAlgoEqual : DoOnPat();

    }
}

void FramePlayAgainstComputer::DoOnStartPlay() {

    _ChessView->setPermettreRotation (false);
    ui->progressBar->setVisible(true);
    ui->groupBoxChoixPlayer->setEnabled(false);
    ui->Button_StopAlgo->setEnabled(true);
    ui->Button_Init->setEnabled(true);
}

void FramePlayAgainstComputer::DoOnEndPlay() {

    ui->progressBar->setVisible(false);
    ui->groupBoxChoixPlayer->setEnabled(true);
    ui->Button_StopAlgo->setEnabled(false);
    _ChessView->setPermettreRotation (true);
}

void FramePlayAgainstComputer::SetInfosMateOrStaleMate (const QString& aInfos) {

    ui->label->setVisible(true);
    ui->label->setText(aInfos);
    QFont f = QFont (ui->label->font());
    f.setPointSize(f.pointSize()+2);
    f.setBold(true);
    ui->label->setFont(f);

    //envoi aussi un messagebox!
    QMessageBox msg;
    msg.setText(aInfos);
    msg.exec();
}

void FramePlayAgainstComputer::DoOnCheckMate() {

    _EtatAlgo = eAlgoLost;

    QString lInfos;
    if (_ColorComputer == WHITE) lInfos = "0-1";
    else lInfos = "1-0";
    lInfos += _MessagePerd;
    SetInfosMateOrStaleMate (lInfos);
}

void FramePlayAgainstComputer::DoOnPat() {

    _EtatAlgo = eAlgoEqual;

    SetInfosMateOrStaleMate (_MessageNulle);
}

void FramePlayAgainstComputer::DoOnAlgoVictorious () {

    _EtatAlgo = eAlgoWins;

    QString lInfos;
    if (_ColorComputer == WHITE) lInfos = "1-0";
    else lInfos = "0-1";
    lInfos += _MessageGagne;
    SetInfosMateOrStaleMate (lInfos);
}

void FramePlayAgainstComputer::on_radioButtonWhite_clicked()
{
    _ColorComputer = BLACK;
    if (_ChessView)
        _ChessView->getControleur().setModePlayAgainstComputer(*this, _ColorComputer);
}

void FramePlayAgainstComputer::on_radioButtonBlack_clicked()
{
    _ColorComputer = WHITE;
    if (_ChessView)
        _ChessView->getControleur().setModePlayAgainstComputer(*this, _ColorComputer);
}

void FramePlayAgainstComputer::on_Button_Init_clicked()
{
    _EtatAlgo = eAlgoNone;
    ui->label->setVisible(false);
    if (_ChessView) {
        _ChessView->getControleur().StopAllComputations();
        _ChessView->FillBoard();
        _ChessView->getControleur().setModePlayAgainstComputer(*this, _ColorComputer);
    }
}

void FramePlayAgainstComputer::on_Button_StopAlgo_clicked()
{
    if (_ChessView)
        _ChessView->getControleur().StopAllComputations();
}
