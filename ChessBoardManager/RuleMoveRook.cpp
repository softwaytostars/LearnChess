#include "RuleMoveRook.h"

RuleMoveRook::RuleMoveRook() : IRuleMovePiece()
{
}

bool RuleMoveRook::_DoMovement (const IChessManager &aIChessManager,
                                const ArticlePiece& aArticleFrom,
                                int aNewPosX,
                                int aNewPosY,
                                CommandPlay* aCommandPlay,
                                const CommandPlay* const,
                                bool,
                                bool aDoNotCheckFriendFinalSquare,
                                bool ARayonX) const{

    //se deplace verticalement ou horizontalement
    if ( (aNewPosX != aArticleFrom._PosX) && (aNewPosY != aArticleFrom._PosY) ) return false;

    //si piece meme couleur rencontree sur chemin, s'arrete avant la piece
    //si piece differente couleur en chemin, s'arrete ici et la mange

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
    }
    else {
       return (_ActionSurDeplacementVertouHoriz (aIChessManager,
                                                 aArticleFrom,
                                                 edirY,
                                                 aArticleFrom._PosX,
                                                 aArticleFrom._PosY,
                                                 aNewPosY,
                                                 aCommandPlay,
                                                 aDoNotCheckFriendFinalSquare,
                                                 ARayonX));
    }

}
