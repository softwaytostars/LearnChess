#ifndef DIALOGASKPIECE_H
#define DIALOGASKPIECE_H

#include <QDialog>
#include "TypePiece.hpp"
#include "ClientGraphique/IUserTraductions.h"

class QListePieces;
class QListWidgetItem;

namespace Ui {
class DialogAskPiece;
}

class DialogAskPiece : public QDialog,
                        public IUserTraductions
{
    Q_OBJECT
    
public:
    explicit DialogAskPiece(eColorPiece aColor, const tQVectorTypePiece &aTypesPiecesFiltrees = tQVectorTypePiece());
    ~DialogAskPiece();
    static bool getTypePiece (eTypePiece& aTypePieceChoisi, eColorPiece aColor,  const tQVectorTypePiece &aTypesPiecesFiltrees = tQVectorTypePiece());
    inline eTypePiece getTypePieceChoisi () const {return _CurrentTypeDemoMoveSelected;}
    //IUserTraductions
    virtual void Translate();
private:
    Ui::DialogAskPiece *ui;
    QListePieces *_ListePieces;
    eTypePiece _CurrentTypeDemoMoveSelected; /**< Le type de la piece choisie */
private slots:
    void OnClickImagePiece (QListWidgetItem* aItemWidget);
};

#endif // DIALOGASKPIECE_H
