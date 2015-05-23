#include "qlistepieces.h"
#include "infosgraphiques.h"
#include <QMimeData>
#include <QMouseEvent>
#include <sstream>

QListePieces::QListePieces(eColorPiece aColorPieces, bool aDrag, QWidget *parent, const tQVectorTypePiece& aTypePiecesFiltrees) :
    QListWidget(parent),
    _ColorPieces (aColorPieces),
    _CanDrag(aDrag),
    _TypePiecesFiltrees(aTypePiecesFiltrees)
{

    InfosGraphiques::instance()->AbonnerUtilisateurPiecesGraphiques(this);

    //taille fixe pour les largeurs de liste
    setMaximumWidth(100);
    setMinimumWidth(100);

    //met un curseur avec une main pour montrer que ca peut bouger
    if (_CanDrag) {
      setCursor(Qt::OpenHandCursor);
      setDragEnabled(true);
      setDragDropMode(QAbstractItemView::DragOnly);
    }

    setIconSize(QSize(30,30));
    ReInitListe ();

}

void QListePieces::ReInitListe () {

    this->clear();

    //met les images des pieces dans les listes
    if (!_TypePiecesFiltrees.contains(Rook))
        InfosGraphiques::instance()->AjouterImagePieceDansListe (Rook,   _ColorPieces, this);
    if (!_TypePiecesFiltrees.contains(Bishop))
        InfosGraphiques::instance()->AjouterImagePieceDansListe (Bishop, _ColorPieces, this);
    if (!_TypePiecesFiltrees.contains(Queen))
        InfosGraphiques::instance()->AjouterImagePieceDansListe (Queen,  _ColorPieces, this);
    if (!_TypePiecesFiltrees.contains(Knight))
        InfosGraphiques::instance()->AjouterImagePieceDansListe (Knight, _ColorPieces, this);
    if (!_TypePiecesFiltrees.contains(King))
        InfosGraphiques::instance()->AjouterImagePieceDansListe (King,   _ColorPieces, this);
    if (!_TypePiecesFiltrees.contains(Pawn))
        InfosGraphiques::instance()->AjouterImagePieceDansListe (Pawn,   _ColorPieces, this);

}

void QListePieces::ReInitListe (const QVector<QString>& aListNames) {

    this->clear();

    eTypePiece lGenrePiece;
    for (int i=0; i < aListNames.count(); i++) {

        if (InfosGraphiques::instance()->get_GenrePiece(aListNames[i], lGenrePiece) &&
                !_TypePiecesFiltrees.contains(lGenrePiece))
            InfosGraphiques::instance()->AjouterImagePieceDansListe (lGenrePiece,   _ColorPieces, this);
    }

}

QListePieces::~QListePieces() {

    InfosGraphiques::instance()->DesabonnerUtilisateurPiecesGraphiques(this);
}

bool QListePieces::get_GenrePiece (const QListWidgetItem *aItemWidget, eTypePiece& aGenrePiece) const {

    return (InfosGraphiques::instance()->get_GenrePiece (aItemWidget->text(), aGenrePiece));
}

//permet de lancer un drag avec des donnees (QMimedata)
void QListePieces::startDrag(Qt::DropActions)
{
   QListWidgetItem *item = currentItem();
   QMimeData *mimeData = new QMimeData;
   //serialise les donnees Typepiece et colorpiece en texte
   std::ostringstream oss;
   oss << int (_ColorPieces);//convertit la valeur enum en string
   mimeData->setText("mimeDataPiece:"+item->text() + ":" +QString(oss.str().c_str()));//contient nom piece + couleur

   //cree l'objet drag
   QDrag *drag = new QDrag(this);
   //lui associe les data
   drag->setMimeData(mimeData);
   //pour mettre une icone sur le drag
   drag->setPixmap(item->icon().pixmap(this->iconSize()));
   //pour mettre l'icone sur le cursor
   drag->setHotSpot(QPoint(drag->pixmap().width()/2, drag->pixmap().height()/2));
   //execute le drag
   drag->exec();
}

//permet d'eviter le forbidden cursor
void QListePieces::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void QListePieces::RechargerGraphiquesPieces () {

    eTypePiece lGenrePiece;
    QString lNomPiece;
    for (int lrow=0; lrow < count(); lrow++) {

        QListWidgetItem *lWidgetItem = this->item(lrow);
        if (lWidgetItem) {

            lNomPiece = lWidgetItem->text();
            if (InfosGraphiques::instance()->get_GenrePiece(lNomPiece, lGenrePiece)) {

                QPixmap lImage;
                if (InfosGraphiques::instance()->get_GraphicPiece (_ColorPieces, lGenrePiece, lImage))
                   lWidgetItem->setIcon(QIcon(lImage));

            }
        }
    }
}
