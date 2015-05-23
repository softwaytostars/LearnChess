#include "dialogaskpiece.h"
#include "ui_dialogaskpiece.h"
#include "qlistepieces.h"
#include "infosgraphiques.h"
#include <QDialogButtonBox>
#include <QPushButton>

DialogAskPiece::DialogAskPiece(eColorPiece aColor, const tQVectorTypePiece& aTypesPiecesFiltrees) :
    QDialog(0),
    ui(new Ui::DialogAskPiece),
    _CurrentTypeDemoMoveSelected(Pawn)
{
    ui->setupUi(this);
    setLayout(ui->verticalLayout);
    ui->verticalLayout->setAlignment(Qt::AlignTop);

    InfosGraphiques::instance()->AbonnerUtilisateurTraductions(this);

    //creation liste pieces possibles
    _ListePieces = new QListePieces (aColor, false, this, aTypesPiecesFiltrees);
    ui->LayoutListe->setContentsMargins(0, 5, 0, 0);
    ui->LayoutListe->addWidget(_ListePieces);
    ui->LayoutListe->setAlignment(Qt::AlignHCenter);

    setModal(true);
    //empeche toute interation avec les autres fenetres tant que pas repondu
    setWindowModality(Qt::ApplicationModal);

    //connect signal clicked
    connect(_ListePieces, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(OnClickImagePiece(QListWidgetItem*)));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->buttonBox->setCenterButtons(true);

    Translate();
}

DialogAskPiece::~DialogAskPiece()
{
    delete ui;
    InfosGraphiques::instance()->DesabonnerUtilisateurTraductions(this);
}

void DialogAskPiece::Translate() {

    setWindowTitle(tr("Choose your piece"));

}


void DialogAskPiece::OnClickImagePiece (QListWidgetItem* aItemWidget) {

    //verifie si on trouve un type de piece pour l'item cliqué
    if (!InfosGraphiques::instance()->get_GenrePiece (aItemWidget->text(), _CurrentTypeDemoMoveSelected)) return;
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

bool DialogAskPiece::getTypePiece (eTypePiece& aTypePieceChoisi, eColorPiece aColor, const tQVectorTypePiece& aTypesPiecesFiltrees) {

    DialogAskPiece diag (aColor, aTypesPiecesFiltrees);
    if (diag.exec() == QDialog::Accepted) {
        aTypePieceChoisi = diag.getTypePieceChoisi ();
        return true;
    }else
        return false;
}
