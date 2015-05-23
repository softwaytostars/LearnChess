#ifndef RULEMOVEKNIGHT_H
#define RULEMOVEKNIGHT_H

#include "ChessBoardManager/Irulemovepiece.h"

class RuleMoveKnight : public IRuleMovePiece
{
public:
    RuleMoveKnight();
    virtual bool get_SquaresForMoves (const IChessManager &aIChessManager,
                                      const ArticlePiece& aArticleFrom,
                                      const CommandPlay* const aPreviousPlay,
                                      tQVectorInfosSquare& aListMoves,
                                      bool aCalculDefenseursAttaquants=true,
                                      bool /*aRayonX*/=false) const;
    static void getSquaresKnightFromPos (int aFromPosX, int aFromPosY, tVecPairPos& agetSquares);
protected:
    virtual bool _DoMovement (const IChessManager &aIChessManager,
                              const ArticlePiece &aArticleFrom,
                              int aNewPosX,
                              int aNewPosY,
                              CommandPlay *aCommandPlay,
                              const CommandPlay* const,
                              bool,
                              bool aDoNotCheckFriendFinalSquare,
                              bool /*ARayonX*/) const;
private:

};

#endif // RULEMOVEKNIGHT_H
