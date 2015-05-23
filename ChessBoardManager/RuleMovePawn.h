#ifndef RULEMOVEPAWN_H
#define RULEMOVEPAWN_H

#include "ChessBoardManager/Irulemovepiece.h"

class RuleMovePawn : public IRuleMovePiece
{
public:
    RuleMovePawn();
protected:
    virtual bool _DoMovement (const IChessManager &aIChessManager,
                              const ArticlePiece &aArticleFrom,
                              int aNewPosX,
                              int aNewPosY,
                              CommandPlay *aCommandPlay,
                              const CommandPlay *const aPreviousPlay,
                              bool,
                              bool aDoNotCheckFriendFinalSquare,
                              bool /*ARayonX*/) const;
    bool getDirections(QVector<eTypeDirection>& aDirections) const { aDirections.push_back(edirDiag);
                                                                     aDirections.push_back(edirY);
                                                                     return true;}
private:
    bool AskPieceForPromotion (eColorPiece aColor, eTypePiece& aTypePieceChoisie) const;
    bool DoPromotion ( const IChessManager &aIChessManager,
                       const ArticlePiece &aArticleFrom,
                       int aNewPosX,
                       int aNewPosY,
                       CommandPlay *aCommandPlay) const;
};

#endif // RULEMOVEPAWN_H
