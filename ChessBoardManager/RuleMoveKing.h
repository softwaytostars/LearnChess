#ifndef RULEMOVEKING_H
#define RULEMOVEKING_H

#include "ChessBoardManager/Irulemovepiece.h"

class RuleMoveKing : public IRuleMovePiece
{
public:
    RuleMoveKing();
protected:
    virtual bool _DoMovement (const IChessManager &aIChessManager,
                              const ArticlePiece &aArticleFrom,
                              int aNewPosX,
                              int aNewPosY,
                              CommandPlay *aCommandPlay,
                              const CommandPlay* const aPreviousCommandPlay,
                              bool aVerifIfKingIsCheck,
                              bool aDoNotCheckFriendFinalSquare,
                              bool /*ARayonX*/) const;
    bool getDirections(QVector<eTypeDirection>& aDirections) const { aDirections.push_back(edirDiag);
                                                                     aDirections.push_back(edirX);
                                                                     aDirections.push_back(edirY);
                                                                     return true;}

};

#endif // RULEMOVEKING_H
