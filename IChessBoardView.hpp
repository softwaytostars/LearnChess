#ifndef ICHESSBOARDVIEW_HPP
#define ICHESSBOARDVIEW_HPP

#include "TypePiece.hpp"
#include "Data/ArticlePiece.hpp"

class DescriptorDemo;
class ObserverChessManager;
class IObserverAlgo;
class IControleur;

class IChessBoardView {
public:
    virtual void setDemo (const DescriptorDemo& aDemo, bool aAllowInteractionChessBoard) = 0;
    virtual void setFreeUseChessBoard (bool aDragOutPossible, bool aModePermissif) = 0;
    virtual void SetModeRotation (bool aModeRotation)=0;
    virtual void setModeInfoAllowedMoves (bool aInfoAllowedMoves)=0;
    virtual void FillBoard ()=0;
    virtual void EmptyBoard ()=0;
    virtual bool getArticlesPiecesDeCouleur (eColorPiece aColorPiece,
                                             tlisteArticlesPieces& aListeArticles) const=0;
    virtual void RegisterObserver (ObserverChessManager &aObserver)=0;
    virtual void setTitle(const QString&)=0;
    virtual void DrawComments (const QString& aDescriptionMove, const QString& aCommentary)=0;
    virtual void DoOnCheckMate ()=0;
    virtual void RegisterView (IChessBoardView& aView)=0;
    virtual void ChangeCommentary (const QString& aCommentary) const=0;
    virtual IControleur& getControleur() const=0;
    virtual void DoOnModePlayAgainstComputer(eColorPiece) {}
};

#endif // ICHESSBOARDVIEW_HPP
