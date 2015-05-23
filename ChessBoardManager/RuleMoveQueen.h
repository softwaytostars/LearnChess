#ifndef RULEMOVEQUEEN_H
#define RULEMOVEQUEEN_H

#include "ChessBoardManager/Irulemovepiece.h"

class RuleMoveQueen : public IRuleMovePiece
{
public:
    RuleMoveQueen();
protected:
    virtual bool _DoMovement (const IChessManager &aIChessManager,
                              const ArticlePiece &aArticleFrom,
                              int aNewPosX,
                              int aNewPosY,
                              CommandPlay *aCommandPlay,
                              const CommandPlay* const,
                              bool,
                              bool aDoNotCheckFriendFinalSquare,
                              bool ARayonX) const;
    bool getDirections(QVector<eTypeDirection>& aDirections) const { aDirections.push_back(edirDiag);
                                                                     aDirections.push_back(edirX);
                                                                     aDirections.push_back(edirY);
                                                                     return true;}
};

#endif // RULEMOVEQUEEN_H
