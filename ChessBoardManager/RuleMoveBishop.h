#ifndef RULEMOVEBISHOP_H
#define RULEMOVEBISHOP_H

#include "ChessBoardManager/Irulemovepiece.h"

class RuleMoveBishop : public IRuleMovePiece
{
public:
    RuleMoveBishop();
protected:
    virtual bool _DoMovement (const IChessManager &aIChessManager,
                              const ArticlePiece &aArticleFrom,
                              int aNewPosX,
                              int aNewPosY,
                              CommandPlay *aCommandPlay,
                              const CommandPlay *const,
                              bool,
                              bool aDoNotCheckFriendFinalSquare,
                              bool ARayonX) const;
    bool getDirections(QVector<eTypeDirection>& aDirections) const { aDirections.push_back(edirDiag);  return true;}
};

#endif // RULEMOVEBISHOP_H
