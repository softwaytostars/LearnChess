#ifndef QLISTEPIECES_H
#define QLISTEPIECES_H

#include <QListWidget>
#include "TypePiece.hpp"
#include "ClientGraphique/IUserPiecesGraphiques.h"

class QListePieces : public QListWidget,
                     public IUserPiecesGraphiques
{
    Q_OBJECT
public:
    explicit QListePieces(eColorPiece aColorPieces,
                          bool aDrag,
                          QWidget *parent=0,
                          const tQVectorTypePiece& aTypePiecesFiltrees=tQVectorTypePiece());
    virtual ~QListePieces();
    bool get_GenrePiece (const QListWidgetItem *aItemWidget, eTypePiece& aGenrePiece) const;
    virtual void RechargerGraphiquesPieces ();
    void ReInitListe ();
    void ReInitListe (const QVector<QString>& aListNames);
protected:
    virtual void startDrag(Qt::DropActions);
    virtual void dragMoveEvent(QDragMoveEvent *event);
private:
    eColorPiece _ColorPieces;
    bool _CanDrag;
    const tQVectorTypePiece _TypePiecesFiltrees;
    
};

#endif // QLISTEPIECES_H
