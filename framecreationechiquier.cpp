#include "framecreationechiquier.h"
#include "ui_framecreationechiquier.h"
#include "decochessboardviewwithrotation.h"
#include "chessboardview.h"
#include "qlistepieces.h"

#include <QPushButton>
#include "TypeDemo.hpp"

FrameCreationEchiquier::FrameCreationEchiquier(ObserverChessManager &aObserver, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameCreationEchiquier)
{
    ui->setupUi(this);
    setLayout (ui->MainverticalLayout);
    ui->MainverticalLayout->setAlignment (Qt::AlignLeft | Qt::AlignTop);

    ui->groupBox->setTitle(tr("Chessboard Initialization"));
    ui->groupBox->setLayout(ui->horizontalLayoutGroupRadio);
    ui->horizontalLayoutGroupRadio->setAlignment (Qt::AlignLeft | Qt::AlignTop);
    ui->radioButtonVider->setText(tr("Empty"));
    ui->radioButtonRemplir->setText(tr("Filled"));

    //creation de la vue du chessboard
    _ChessView = new ChessBoardViewWithRotation (new ChessBoardView (0));

    //ne pas indiquer checkbox pour voir commentaires
    _ChessView->setUseCommentaries (false);

    _ChessView->RegisterObserver(aObserver);

    ui->verticalLayoutBoard->addWidget(_ChessView);

    //creation liste pieces noires
    _ListePiecesBlack = new QListePieces (BLACK, true, this);
    ui->verticalLayout_3->addWidget(_ListePiecesBlack);

    //creation liste pieces blanches
    _ListePiecesWhite = new QListePieces (WHITE, true, this);
    ui->verticalLayout_2->addWidget(_ListePiecesWhite);


    ui->radioButtonVider->setChecked(true);

    setModePermissifEtDragOutOfScene (true, true);

}

void FrameCreationEchiquier::setModePermissifEtDragOutOfScene (bool aModeDragOut, bool aModePermissif) {

    //permet suppression item lorsque mis en dehors de la scene
    _ChessView->setFreeUseChessBoard (aModeDragOut, aModePermissif);
}

FrameCreationEchiquier::~FrameCreationEchiquier()
{
    delete ui;
}


void FrameCreationEchiquier::on_radioButtonRemplir_toggled(bool checked)
{
    if (_ChessView && checked)
        _ChessView->FillBoard();
}

void FrameCreationEchiquier::on_radioButtonVider_toggled(bool checked)
{
    if (_ChessView && checked)
        _ChessView->EmptyBoard();
}

bool FrameCreationEchiquier::getArticlesPiecesDeCouleur (eColorPiece aColorPiece, tlisteArticlesPieces& aListeArticles) const {

    bool lOk = _ChessView && _ChessView->getArticlesPiecesDeCouleur(aColorPiece, aListeArticles);
    return lOk;

}

bool FrameCreationEchiquier::RotationIsAsked () const {

    if (_ChessView)
        return _ChessView->RotationIsAsked();
    else return false;

}




