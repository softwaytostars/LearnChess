#include "RuleMoveQueen.h"

RuleMoveQueen::RuleMoveQueen() : IRuleMovePiece()
{
}

bool RuleMoveQueen::_DoMovement (const IChessManager &aIChessManager,
                                 const ArticlePiece& aArticleFrom,
                                 int aNewPosX,
                                 int aNewPosY,
                                 CommandPlay* aCommandPlay,
                                 const CommandPlay* const,
                                 bool,
                                 bool aDoNotCheckFriendFinalSquare,
                                 bool ARayonX) const{

    //se deplace diagonalement ou verticalement ou horizontalement
    if (aNewPosY == aArticleFrom._PosY) {
        return _ActionSurDeplacementVertouHoriz (aIChessManager,
                                                 aArticleFrom,
                                                 edirX,
                                                 aArticleFrom._PosY,
                                                 aArticleFrom._PosX,
                                                 aNewPosX,
                                                 aCommandPlay,
                                                 aDoNotCheckFriendFinalSquare,
                                                 ARayonX);
    }else if (aNewPosX == aArticleFrom._PosX) {
        return _ActionSurDeplacementVertouHoriz (aIChessManager,
                                                 aArticleFrom,
                                                 edirY,
                                                 aArticleFrom._PosX,
                                                 aArticleFrom._PosY,
                                                 aNewPosY,
                                                 aCommandPlay,
                                                 aDoNotCheckFriendFinalSquare,
                                                 ARayonX);
    }else {
        return _ActionSurDeplacementDiag (aIChessManager,
                                          aArticleFrom,
                                          aNewPosX,
                                          aNewPosY,
                                          aCommandPlay,
                                          aDoNotCheckFriendFinalSquare,
                                          ARayonX);
    }
}
