#include "RuleMovePawn.h"
#include <cmath>
#include "dialogaskpiece.h"
#include "commandplay.h"

RuleMovePawn::RuleMovePawn()
{
}

bool RuleMovePawn::_DoMovement (const IChessManager &aIChessManager,
                                const ArticlePiece& aArticleFrom,
                                int aNewPosX,
                                int aNewPosY,
                                CommandPlay* aCommandPlay,
                                const CommandPlay* const aPreviousPlay,
                                bool,
                                bool aDoNotCheckFriendFinalSquare,
                                bool /*ARayonX*/) const {

    //ne se deplace qu'en avant
    if (aArticleFrom._Color == WHITE) {
        if (aNewPosY < aArticleFrom._PosY) return false;
    }
    else {
        if (aNewPosY > aArticleFrom._PosY) return false;
    }

    //et pas plus d'une case (ou 2 si premier coup)
    int lnbreCases = 1;
    if (!aArticleFrom._HasEverMoved && (aNewPosX == aArticleFrom._PosX)) {
        if ( ((aArticleFrom._Color == BLACK)  && (aArticleFrom._PosY ==7))
             || ((aArticleFrom._Color == WHITE)  && (aArticleFrom._PosY ==2)))
        lnbreCases = 2;
    }

    double ldiff = 1.0*(int (aNewPosY) - int(aArticleFrom._PosY));
    if (std::abs(ldiff) > lnbreCases ) return false;

    //verifie si case promotion
    bool lPromotion = (((aArticleFrom._Color == WHITE) && (aNewPosY == 8))
                        || ((aArticleFrom._Color == BLACK) && (aNewPosY == 1)));

    //au cas où il attaque
    if (qAbs<int> (int(aNewPosX) - int(aArticleFrom._PosX)) == 1) {
        ArticlePiece lPiece;
        //mvt autorise slt si une piece ennemie y est
        if (aIChessManager.UnePieceExiste(aNewPosX, aNewPosY, lPiece) && (lPiece._Color != aArticleFrom._Color)) {
            if (lPromotion) {
                return DoPromotion (aIChessManager, aArticleFrom, aNewPosX, aNewPosY, aCommandPlay);
            } else
                return _ActionSurDeplacementDiag (aIChessManager,
                                                  aArticleFrom,
                                                  aNewPosX, aNewPosY,
                                                  aCommandPlay,
                                                  aDoNotCheckFriendFinalSquare,
                                                  false);
        } else {
            //verifie si c'est une prise en passant
            eTypePiece lPieceFrom;
            int lPosXFrom, lPosYFrom, lPosXTo, lPosYTo;
            eColorPiece lColorPiece;
            ArticlePiece lPieceToDelete;

            //si on ne donne pas le previousmove et que acommandplay=0 (cad juste verif pour le lectureur de PGN),
            //on admet la possibilite du mvt si le pion à executer est dans les bonnes conditions positionnelle
            if ((aPreviousPlay == 0) && (aCommandPlay == 0)
                    && aIChessManager.UnePieceExiste(aNewPosX, aArticleFrom._PosY, lPieceToDelete)
                    && (lPieceToDelete._TypePiece == Pawn)
                    && (lPieceToDelete._Color != aArticleFrom._Color)
                    && !aIChessManager.UnePieceExiste(aNewPosX, aNewPosY, lPieceToDelete))
                return true;

            if (aPreviousPlay && (aPreviousPlay->getColor() != aArticleFrom._Color)
                    && aPreviousPlay->EstUnSimpleMove(lPieceFrom, lPosXFrom, lPosYFrom, lPosXTo, lPosYTo, lColorPiece)
                    && (lPieceFrom == Pawn)
                    && (lColorPiece != aArticleFrom._Color)
                    && ((std::abs(1.0*(int (lPosYTo) - int (lPosYFrom)))) == 2)
                    && (aNewPosX == lPosXTo)
                    && (lPosYTo == aArticleFrom._PosY)
                    && aIChessManager.UnePieceExiste(lPosXTo, lPosYTo, lPieceToDelete)) {
                if (aCommandPlay) {
                    aCommandPlay->AddActionPriseEnPassant(aArticleFrom, aNewPosX, aNewPosY, lPieceToDelete);
                }
                return true;

            }
            else return false;
        }
    }//if (qAbs<int> (int(aNewPosX) - int(aArticleFrom._PosX)) == 1)

    //pour autre cas, se deplace que verticalement
    if (aNewPosX != aArticleFrom._PosX) return false;

    //si piece devant le pion, impossible d'avancer
    ArticlePiece lPieceGenante;
    if (aIChessManager.UnePieceExiste(aNewPosX, aNewPosY, lPieceGenante)) return false;

    //verifie si promotion
    if (lPromotion) {
        return DoPromotion (aIChessManager, aArticleFrom, aNewPosX, aNewPosY, aCommandPlay);
    }

    //sinon bouge la piece
    return _ActionSurDeplacementVertouHoriz(aIChessManager,
                                            aArticleFrom,
                                            edirY,
                                            aArticleFrom._PosX,
                                            aArticleFrom._PosY,
                                            aNewPosY,
                                            aCommandPlay,
                                            aDoNotCheckFriendFinalSquare,
                                            false);
}

bool RuleMovePawn::AskPieceForPromotion (eColorPiece aColor, eTypePiece& aTypePieceChoisie) const {

    tQVectorTypePiece lPiecesFiltrees;
    lPiecesFiltrees.append(Pawn);//on peut promouvoir en tout sauf pion et roi
    lPiecesFiltrees.append(King);
    return DialogAskPiece::getTypePiece (aTypePieceChoisie, aColor, lPiecesFiltrees);
}

bool RuleMovePawn::DoPromotion ( const IChessManager &aIChessManager,
                                 const ArticlePiece& aArticleFrom,
                                 int aNewPosX,
                                 int aNewPosY,
                                 CommandPlay* aCommandPlay) const {

    //si pas de reponse pour type promotion, annule le coup
    if (aCommandPlay) {
        ArticlePiece lArticleTo = aArticleFrom;
        lArticleTo._PosX = aNewPosX;
        lArticleTo._PosY = aNewPosY;
        eTypePiece lGenrePromotion;
        //quand les notifications sont desactivées (pour l'algo, on considère que le coup de promotion sera la reine!)
        if (aIChessManager.NotificationsSontDesactivees() || aIChessManager.AskPiecePromotionIsForbiddenFor (aArticleFrom._Color))
             lGenrePromotion = Queen;
        else if (!AskPieceForPromotion (aArticleFrom._Color, lGenrePromotion))
            return false;

        ArticlePiece lPieceExistante;
        if (aIChessManager.UnePieceExiste(aNewPosX, aNewPosY, lPieceExistante))
            aCommandPlay->AddActionMangerPiece(aArticleFrom, lPieceExistante);
        else
            aCommandPlay->AddActionMove(aArticleFrom, aNewPosX, aNewPosY);
        //puis promotion
        aCommandPlay->AddActionPromotion(lArticleTo, lGenrePromotion);
    }
    return true;
}
