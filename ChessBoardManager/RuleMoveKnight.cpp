#include "RuleMoveKnight.h"
#include <cmath>
#include "commandplay.h"

RuleMoveKnight::RuleMoveKnight() : IRuleMovePiece()
{
}

bool RuleMoveKnight::_DoMovement (const IChessManager &aIChessManager,
                                  const ArticlePiece& aArticleFrom,
                                  int aNewPosX,
                                  int aNewPosY,
                                  CommandPlay* aCommandPlay,
                                  const CommandPlay* const,
                                  bool,
                                  bool aDoNotCheckFriendFinalSquare,
                                  bool /*ARayonX*/)const {
   //le cavalier peut sauter les pieces donc simple verif si position finale contient pas une
   //piece de sa couleur
    ArticlePiece lArticleTo;
    bool lGoodMove = ((std::abs(1.0*(int(aNewPosX)-int(aArticleFrom._PosX))) == 1) && (std::abs(1.0*(int(aNewPosY)-int(aArticleFrom._PosY))) == 2))
                        || ((std::abs(1.0*(int(aNewPosX)-int(aArticleFrom._PosX))) == 2) && (std::abs(1.0*(int(aNewPosY)-int(aArticleFrom._PosY))) == 1));
    if (!lGoodMove) return false;

    if (aIChessManager.UnePieceExiste(aNewPosX, aNewPosY, lArticleTo)) {
        if (lArticleTo._Color == aArticleFrom._Color) return aDoNotCheckFriendFinalSquare;
        if (aCommandPlay)
            aCommandPlay->AddActionMangerPiece(aArticleFrom, lArticleTo);
        return true;
    }else {
        if (aCommandPlay)
            aCommandPlay->AddActionMove(aArticleFrom, aNewPosX, aNewPosY);
        return true;
    }
}

void RuleMoveKnight::getSquaresKnightFromPos (int aFromPosX, int aFromPosY, tVecPairPos& agetSquares) {

    agetSquares.clear();

    int lNewPosX, lNewPosY;

    //8 possibilites
    lNewPosX = aFromPosX+1;
    lNewPosY = aFromPosY+2;
    if ((lNewPosX <= 8) && (lNewPosY <=8))
        agetSquares.push_back(QPair <int, int> (lNewPosX, lNewPosY));

    lNewPosX = aFromPosX-1;
    lNewPosY = aFromPosY+2;
    if ((lNewPosX >= 1) && (lNewPosY <=8))
       agetSquares.push_back(QPair <int, int> (lNewPosX, lNewPosY));

    lNewPosX = aFromPosX-2;
    lNewPosY = aFromPosY+1;
    if ((lNewPosX >= 1) && (lNewPosY <=8))
        agetSquares.push_back(QPair <int, int> (lNewPosX, lNewPosY));

    lNewPosX = aFromPosX-2;
    lNewPosY = aFromPosY-1;
    if ((lNewPosX >= 1) && (lNewPosY >= 1))
        agetSquares.push_back(QPair <int, int> (lNewPosX, lNewPosY));

    lNewPosX = aFromPosX-1;
    lNewPosY = aFromPosY-2;

    if ((lNewPosX >= 1) && (lNewPosY >= 1))
        agetSquares.push_back(QPair <int, int> (lNewPosX, lNewPosY));

    lNewPosX = aFromPosX+1;
    lNewPosY = aFromPosY-2;
    if ((lNewPosX <= 8) && (lNewPosY >= 1))
        agetSquares.push_back(QPair <int, int> (lNewPosX, lNewPosY));

    lNewPosX = aFromPosX+2;
    lNewPosY = aFromPosY-1;
    if ((lNewPosX <= 8) && (lNewPosY >= 1))
        agetSquares.push_back(QPair <int, int> (lNewPosX, lNewPosY));

    lNewPosX = aFromPosX+2;
    lNewPosY = aFromPosY+1;
    if ((lNewPosX <= 8) && (lNewPosY <= 8))
        agetSquares.push_back(QPair <int, int> (lNewPosX, lNewPosY));

}


bool RuleMoveKnight::get_SquaresForMoves (const IChessManager &aIChessManager,
                                          const ArticlePiece& aArticleFrom,
                                          const CommandPlay* const aPreviousPlay,
                                          tQVectorInfosSquare& aListMoves,
                                          bool aCalculDefenseursAttaquants,
                                          bool /*aRayonX*/) const {

    aListMoves.clear();

    tVecPairPos lSquaresKnight;
    getSquaresKnightFromPos(aArticleFrom._PosX, aArticleFrom._PosY, lSquaresKnight);

    for (int i=0; i < lSquaresKnight.count(); i++) {

        QPair <int, int> lSquare = lSquaresKnight [i];
        AddPossibleMoves(aIChessManager,
                         aArticleFrom,
                         lSquare.first, lSquare.second,
                         aPreviousPlay,
                         aListMoves,
                         aCalculDefenseursAttaquants,
                         false);
    }

    return (aListMoves.count()>0);
}
