#ifndef CHESSBOARDVIEWWITHROTATION_H
#define CHESSBOARDVIEWWITHROTATION_H

#include <QFrame>
#include "IChessBoardView.hpp"
#include "ClientGraphique/IUserTraductions.h"

namespace Ui {
    class ChessBoardViewWithRotation;
}


/**
 * @brief Classe d'encapsulation de ChessBoardView pour la decorer d'une checkbox
 *
 */
class ChessBoardViewWithRotation : public QFrame,
                                   public IChessBoardView,
                                   public IUserTraductions
{
    Q_OBJECT
    
public:
    explicit ChessBoardViewWithRotation(QWidget *aChessBoardView, QWidget *parent = 0);
    ~ChessBoardViewWithRotation();
    //IChessBoardView (sera une simple délégation car pas de comportement modifié)
    virtual void setDemo (const DescriptorDemo& aDemo, bool aAllowInteractionChessBoard);
    virtual void setFreeUseChessBoard (bool aDragOutPossible, bool aModePermissif);
    virtual void SetModeRotation (bool aModeRotation);
    virtual void setModeInfoAllowedMoves (bool aInfoAllowedMoves);
    virtual void FillBoard ();
    virtual void EmptyBoard ();
    virtual bool getArticlesPiecesDeCouleur (eColorPiece aColorPiece,
                                             tlisteArticlesPieces &aListeArticles) const;
    virtual void RegisterObserver (ObserverChessManager &aObserver);
    virtual void setTitle(const QString& aTitle);
    virtual void DrawComments (const QString& aDescriptionMove, const QString& aCommentary);
    virtual void DoOnCheckMate (){}
    virtual void RegisterView (IChessBoardView& aView);
    virtual void ChangeCommentary (const QString& aCommentary) const;
    virtual IControleur& getControleur() const;
    virtual void DoOnModePlayAgainstComputer(eColorPiece aColorComputer);
    //IUserTraductions
    virtual void Translate();

    bool RotationIsAsked() const;
    void setUseCommentaries (bool aWithCommentaries);
    void setUseInfosAllowMoves (bool aUseInfosAllowMoves);
    void setPermettreRotation (bool aPermettre);

private slots:
    void on_checkBoxRotate_clicked(bool checked);
    void on_checkBoxComments_toggled(bool checked);
    void on_textComments_textChanged();
    void on_checkBoxInfoAllowedMoves_clicked(bool checked);

private:
    Ui::ChessBoardViewWithRotation *ui;
    QWidget *_ChessBoardView;
};

#endif // CHESSBOARDVIEWWITHROTATION_H
