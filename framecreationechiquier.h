#ifndef FRAMECREATIONECHIQUIER_H
#define FRAMECREATIONECHIQUIER_H

#include <QFrame>
#include <QListWidgetItem>
#include "Data/ArticlePiece.hpp"

namespace Ui {
class FrameCreationEchiquier;
}

class ObserverChessManager;
class ChessBoardViewWithRotation;
class QListePieces;


class FrameCreationEchiquier : public QFrame
{
    Q_OBJECT
    
public:
    explicit FrameCreationEchiquier(ObserverChessManager& aObserver, QWidget *parent = 0);
    ~FrameCreationEchiquier();
    bool getArticlesPiecesDeCouleur (eColorPiece aColorPiece, tlisteArticlesPieces& aListeArticles) const;

    bool RotationIsAsked () const;
    void setModePermissifEtDragOutOfScene (bool aModeDragOut, bool aModePermissif);
private:
    Ui::FrameCreationEchiquier *ui;
    ChessBoardViewWithRotation *_ChessView;
    QListePieces *_ListePiecesBlack, *_ListePiecesWhite;

private slots:
    void on_radioButtonRemplir_toggled(bool checked);
    void on_radioButtonVider_toggled(bool checked);
};

#endif // FRAMECREATIONECHIQUIER_H
