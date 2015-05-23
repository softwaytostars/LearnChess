#include "RuleMoveKing.h"
#include <vector>
#include <cmath>
#include "commandplay.h"

RuleMoveKing::RuleMoveKing() : IRuleMovePiece()
{
}

int CoeffDirecteur (int aFromX, int aFromY, int aToX, int aToY) {
    if (aToX == aFromX) return 100;
    if (aToY == aFromY) return 0;
    return (aToY - aFromY)/(aToX - aFromX);
}

bool RuleMoveKing::_DoMovement (const IChessManager &aIChessManager,
                                const ArticlePiece& aArticleFrom,
                                int aNewPosX,
                                int aNewPosY,
                                CommandPlay* aCommandPlay,
                                const CommandPlay* const aPreviousCommandPlay,
                                bool aVerifIfKingIsCheck,
                                bool aDoNotCheckFriendFinalSquare,
                                bool /*ARayonX*/) const {


       //verifie si le roi veut roquer
       sDiagnostiqueCastling lDiagnostiqueCastling;
       if (aIChessManager.CastlingIsPossibleForKing(aArticleFrom, aNewPosX, aNewPosY, lDiagnostiqueCastling)) {

           //fait le roque (deplace roi + tour opposee)
           if (aCommandPlay) {
               aCommandPlay->AddActionCastling(aArticleFrom._Color, lDiagnostiqueCastling._IsSmallCastling);
           }

       return true;
    }//if CastlingIsPossibleForKing

    tlisteArticlesPieces llistePiecesEnnemies;
    //verif ne peut-etre faite ici car boucle: roi se deplace , verifie que pas attaque par pieces opposees qui verifie si pas attaqué...
    if (aVerifIfKingIsCheck) {

        //si le roi se deplace dans la meme direction qu'il est attaqué, pas possible
        if (aPreviousCommandPlay && aPreviousCommandPlay->KingEnnemyIsChecked()) {

            int lCoeffDeplacement = CoeffDirecteur (aArticleFrom._PosX, aArticleFrom._PosY,
                                                    aNewPosX, aNewPosY);
            for (tlisteArticlesPieces::const_iterator itArticlePiece = aPreviousCommandPlay->getPiecesGivingCheck().begin();
                 itArticlePiece != aPreviousCommandPlay->getPiecesGivingCheck().end();
                 ++itArticlePiece) {

                //si roi attaque piece donnant echec, c'est ok
                if ((aNewPosX == (*itArticlePiece)._PosX) && (aNewPosY == (*itArticlePiece)._PosY))
                    continue;
                //regarde slt les pieces attaque à distance
                if (((*itArticlePiece)._TypePiece != Rook) &&
                    ((*itArticlePiece)._TypePiece != Queen) &&
                    ((*itArticlePiece)._TypePiece != Bishop))
                    continue;

                int lCoeffAttaque = CoeffDirecteur ((*itArticlePiece)._PosX, (*itArticlePiece)._PosY,
                                                        aArticleFrom._PosX, aArticleFrom._PosY);
                //meme direction deplacement que la direction ou il etait attaque, donc pas possible
                if (lCoeffDeplacement == lCoeffAttaque)
                    return false;

            }//for
        }
        //sinon verifie qu'il est pas attqué par une autre piece
        if (aIChessManager.ColorIsUnderAttackForPosition (aArticleFrom._Color,
                                                          aNewPosX, aNewPosY,
                                                          llistePiecesEnnemies)) return false;
    }

    //pour autre cas, ne se deplace que d'une case
    if (aNewPosY == aArticleFrom._PosY) {
        if (std::abs(1.0*(int(aNewPosX)-int(aArticleFrom._PosX))) > 1 ) return false;
        return _ActionSurDeplacementVertouHoriz (aIChessManager,
                                                 aArticleFrom,
                                                 edirX,
                                                 aArticleFrom._PosY,
                                                 aArticleFrom._PosX,
                                                 aNewPosX,
                                                 aCommandPlay,
                                                 aDoNotCheckFriendFinalSquare,
                                                 false);
    }else if (aNewPosX == aArticleFrom._PosX) {
        if (std::abs(1.0*(int(aNewPosY)-int(aArticleFrom._PosY))) > 1 ) return false;
        return _ActionSurDeplacementVertouHoriz (aIChessManager,
                                                 aArticleFrom,
                                                 edirY,
                                                 aArticleFrom._PosX,
                                                 aArticleFrom._PosY,
                                                 aNewPosY,
                                                 aCommandPlay,
                                                 aDoNotCheckFriendFinalSquare,
                                                 false);
    }else {
        if ( (std::abs(1.0*(int(aNewPosX)-int(aArticleFrom._PosX))) + std::abs(1.0*(int(aNewPosY)-int(aArticleFrom._PosY)))) > 2) return false;
        return _ActionSurDeplacementDiag (aIChessManager,
                                          aArticleFrom,
                                          aNewPosX,
                                          aNewPosY,
                                          aCommandPlay,
                                          aDoNotCheckFriendFinalSquare,
                                          false);
    }
}
