#ifndef RULEMOVEROOK_H
#define RULEMOVEROOK_H

#include "ChessBoardManager/Irulemovepiece.h"


class RuleMoveRook : public IRuleMovePiece
{
public:
    RuleMoveRook();
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
    bool getDirections(QVector<eTypeDirection>& aDirections) const { aDirections.push_back(edirX);
                                                                     aDirections.push_back(edirY);
                                                                     return true;}
};

#endif // RULEMOVEROOK_H
