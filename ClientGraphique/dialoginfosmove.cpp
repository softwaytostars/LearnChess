#include "dialoginfosmove.h"
#include "ui_dialoginfosmove.h"

#include <QDialogButtonBox>

#include "chessboardview.h"
#include "Demos/IFournisseurDemo.h"


DialogInfosMove::DialogInfosMove(int aTypeDemoMove, const IFournisseurDemo &aFournisseurDemo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInfosMove)
{
    ui->setupUi(this);
    setWindowTitle(tr("Description"));
    setLayout(ui->verticalLayout);
  //  setMinimumHeight(500);
    ui->textBrowser->setMinimumHeight(150);
    ui->textBrowser->setMaximumHeight(150);

    QDialogButtonBox *buttonBox = new QDialogButtonBox (this);
    buttonBox->addButton(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    ui->LayoutButtonBox->addWidget(buttonBox);
    setModal(false);

    _ChessView = new ChessBoardView (this);
    ui->verticalLayout_4->addWidget(_ChessView);
    _ChessView->setGeometry(0, 0, 401, 401);
    _ChessView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _ChessView->setEnabled(false);

    ArticlePiece lPiece;
    tlisteArticlesPieces lPieces;

    if (aFournisseurDemo.setStaticPieceAndShowItsPossiblePath(aTypeDemoMove, lPiece)) {
        _ChessView->AddStaticPieceAndShowAllowedDirections (lPiece);
    }else if (aFournisseurDemo.setStaticPieces(lPieces))
        _ChessView->AddStaticPiecesWithoutShowingDirections(lPieces);
    else
        _ChessView->setVisible(false);

    ui->textBrowser->setText(aFournisseurDemo.get_DetailedDescription(aTypeDemoMove));

}

DialogInfosMove::~DialogInfosMove()
{
    delete ui;
}

void DialogInfosMove::ShowInfosForDemoMove (int aTypeDemoMove, const IFournisseurDemo &aFournisseurDemo)  {

    DialogInfosMove diag (aTypeDemoMove, aFournisseurDemo);
    diag.exec();
}
