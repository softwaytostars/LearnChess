#include "Irulemovepiece.h"

#include <cmath>

#include "commandplay.h"

IRuleMovePiece::IRuleMovePiece () {

}

bool IRuleMovePiece::_ConditionMinimumRespectee (const IChessManager &aIChessManager,
                                                 bool aDoNotCheckFriendFinalSquare,
                                                 int aOldPosX, int aOldPosY,
                                                 int anewPosX, int anewPosY,
                                                 ArticlePiece& aArticle) const  {

    //si piece n'existe pas à la position d'origine, pas possible
    if (!aIChessManager.UnePieceExiste (aOldPosX, aOldPosY, aArticle)) return false;

    if (aDoNotCheckFriendFinalSquare) return true;
    ArticlePiece lArticleTo;
    bool lUnePieceAllieeDejaDessus = aIChessManager.UnePieceExiste (anewPosX, anewPosY, lArticleTo)
                                        && (lArticleTo._Color == aArticle._Color);
    //Si piece alliee déjà sur arrivee, pas possible
    if (lUnePieceAllieeDejaDessus) return false;

    return true;
}


bool IRuleMovePiece::AllowMove (const IChessManager &aIChessManager,
                                int aOldPosX, int aOldPosY,
                                int aNewPosX, int aNewPosY,
                                const CommandPlay* const aPreviousPlay,
                                bool aVerifIfKingIsCheck,
                                bool aDoNotCheckFriendFinalSquare,
                                bool ARayonX) const {

    ArticlePiece lArticleFrom;
    if ((aOldPosX == aNewPosX) && (aOldPosY == aNewPosY)) return false;//dit pas ok si piece bouge pas
    if (!_ConditionMinimumRespectee (aIChessManager,
                                     aDoNotCheckFriendFinalSquare,
                                     aOldPosX, aOldPosY, aNewPosX, aNewPosY, lArticleFrom )) return false;

    if (!_PasseCritereEvaluationRapide (lArticleFrom, aPreviousPlay, aNewPosX, aNewPosY))
        return false;

    //verifie simplement si mvt possible
    bool lMoveAutorise = _DoMovement (aIChessManager, lArticleFrom, aNewPosX, aNewPosY, 0, aPreviousPlay,
                                      aVerifIfKingIsCheck,
                                      aDoNotCheckFriendFinalSquare,
                                      ARayonX);
    return lMoveAutorise;
}

bool IRuleMovePiece::_PasseCritereEvaluationRapide (const ArticlePiece& aArticleFrom,
                                                    const CommandPlay* const aPreviousPlay,
                                                    int aNewPosX, int aNewPosY) const {

    if (aPreviousPlay && (aArticleFrom._Color == aPreviousPlay->getColor())) return true;//on teste slt pour coup suivant

    //sur echec double, seul move du roi possible
    if (aPreviousPlay && (aPreviousPlay->getPiecesGivingCheck().count() > 1) && (aArticleFrom._TypePiece != King))
        return false;

    //si echec à cause cavalier, seul move roi ou essaie tuer cavalier est possible
    if (aPreviousPlay &&
            (aPreviousPlay->getPiecesGivingCheck().count() == 1) &&
            (aPreviousPlay->getPiecesGivingCheck()[0]._TypePiece == Knight) && (aArticleFrom._TypePiece != King))
        return ( (aNewPosX == aPreviousPlay->getPiecesGivingCheck()[0]._PosX)
                 && (aNewPosY == aPreviousPlay->getPiecesGivingCheck()[0]._PosY));

    return true;
}

bool IRuleMovePiece::DoPlay (IChessManager &aIChessManager,
                             int aOldPosX, int aOldPosY,
                             int aNewPosX, int aNewPosY,
                             CommandPlay*& aCommandPlay,
                             const CommandPlay* const aPreviousPlay) {

    ArticlePiece lArticleFrom;
    aCommandPlay = 0;
    bool lDoNotCheckFriendFinalSquare = false;
    if (!_ConditionMinimumRespectee (aIChessManager,
                                     lDoNotCheckFriendFinalSquare,
                                     aOldPosX, aOldPosY, aNewPosX, aNewPosY, lArticleFrom )) return false;
    ArticlePiece lArticleTo;
    if (aIChessManager.UnePieceExiste(aNewPosX, aNewPosY, lArticleTo) && (lArticleTo._TypePiece == King))
        return false;//pas le droit de tuer le roi!!!

    if (!_PasseCritereEvaluationRapide (lArticleFrom, aPreviousPlay, aNewPosX, aNewPosY))
        return false;
    //sur echec multiple, seul un move de roi va etre possible

    CommandPlay* lCommandPlay = new CommandPlay (aIChessManager);

    //verifie simplement si mvt possible
    bool lMoveAutorise = _DoMovement (aIChessManager,
                                      lArticleFrom,
                                      aNewPosX, aNewPosY,
                                      lCommandPlay,
                                      aPreviousPlay,
                                      false,//car sera verifie dans la simu suivante (sur le modifierplace)
                                      lDoNotCheckFriendFinalSquare,//car le jeu autorise pas de jouer sur case amie
                                      false /*car pas de rayon X lors du jeu*/);

    if (lMoveAutorise) {
    //verifie si une fois le coup fait, le roi est en echec
        bool lNotificationsDesactivees = aIChessManager.NotificationsSontDesactivees();
        aIChessManager.DesActiverNotification(true);
        lMoveAutorise = lMoveAutorise
                        && lCommandPlay->Execute(false);
        //le roi ne doit pas etre attaqué suite au move
        lMoveAutorise = lMoveAutorise
                        && !aIChessManager.KingIsAttacked (lArticleFrom._Color);

        lCommandPlay->UnExecute();
        aIChessManager.DesActiverNotification(lNotificationsDesactivees);
    }

    if (!lMoveAutorise) {
        delete lCommandPlay;
        aCommandPlay = 0;
    }else {
        aCommandPlay = lCommandPlay;
    }
    return lMoveAutorise;
}

bool IRuleMovePiece::_ActionSurDeplacementDiag (const IChessManager &aIChessManager,
                                                const ArticlePiece& aArticleFrom,
                                                int aNewPosX,
                                                int aNewPosY,
                                                CommandPlay* aCommandPlay,
                                                bool aDoNotCheckFriendFinalSquare,
                                                bool aRayonX) const
{

    //se deplace diagonalement donc la abs(pente) est de 1
    int ldiff1 = (aNewPosY - aArticleFrom._PosY);
    int ldiff2 = (aNewPosX-aArticleFrom._PosX);
    if (ldiff2 == 0) return false;
    float lPente = (1.0*ldiff1/ldiff2);
    if (std::abs(lPente) != 1.0) return false;
    int lDiff = aNewPosX - aArticleFrom._PosX;
    int lDirectionH = lDiff/std::abs(1.0*lDiff);
    //si piece differente couleur sur parcours, s'y arrete et la mange

    ArticlePiece lArticleTo;
    for (int lPosX = aArticleFrom._PosX+lDirectionH; lPosX != aNewPosX+lDirectionH; lPosX+=lDirectionH) {
        int lPosY = lPente*(lPosX-aArticleFrom._PosX)+aArticleFrom._PosY;
        if (aIChessManager.UnePieceExiste(lPosX, lPosY, lArticleTo)){

            //pour case finale, refuse si meme couleurs slt si pas aDoNotCheckFriendFinalSquare
            if ((lPosX == aNewPosX) && (lPosY == aNewPosY) && (lArticleTo._Color == aArticleFrom._Color))
                return aDoNotCheckFriendFinalSquare;

            //si piece meme couleur sur parcours, pas possible
            if (lArticleTo._Color == aArticleFrom._Color) return false;
            //si une piece ennemie est en travers du chemin, on dit qu'il est possible seulement si c'est le bout du chemin (position finale).
            if ((lPosX == aNewPosX) && (lPosY == aNewPosY)) {
                if (aCommandPlay) aCommandPlay->AddActionMangerPiece(aArticleFrom, lArticleTo);
                return true;
            } else return aRayonX;
        }
    }

    if (aCommandPlay) aCommandPlay->AddActionMove(aArticleFrom, aNewPosX, aNewPosY);
    return true;
}

bool IRuleMovePiece::_ActionSurDeplacementVertouHoriz (const IChessManager &aIChessManager,
                                                       const ArticlePiece &aArticleFrom,
                                                       eTypeDirection aTypeDir,
                                                       int aInvariant,
                                                       int aDebut,
                                                       int aFin,
                                                       CommandPlay* aCommandPlay,
                                                       bool aDoNotCheckFriendFinalSquare,
                                                       bool ARayonX) const {

    ArticlePiece lArticleTo;

    if (aFin == aDebut) return true;

    //dans quel sens on va
    int ldiff = aFin - aDebut;
    int lDirection = ldiff/std::abs(1.0*ldiff);

    //boucle sur les positions du parcours
    for (int lPos = aDebut+lDirection; lPos != (aFin+lDirection); lPos+=lDirection) {

        bool lPieceExistante;
        if (aTypeDir == edirX) {
            lPieceExistante = aIChessManager.UnePieceExiste(lPos, aInvariant, lArticleTo);
        }else
            lPieceExistante = aIChessManager.UnePieceExiste(aInvariant, lPos, lArticleTo);

        if (lPieceExistante) {

            //pour case finale, refuse si meme couleurs slt si pas aDoNotCheckFriendFinalSquare
            if ((lPos == aFin) && (lArticleTo._Color == aArticleFrom._Color))
                return aDoNotCheckFriendFinalSquare;

            if (aArticleFrom._Color == lArticleTo._Color) {
                return false;//on peut pas traverser la piece
            }else
            {
                //le pion est la seule exception pour laquelle on ne prend pas sur le deplacement vertical
                if (aArticleFrom._TypePiece == Pawn) return false;
                //move possible slt si piece est en position demandee
                if (lPos == aFin) {
                    //piece differente couleur, on la mange et on s'y met
                    if (aCommandPlay)
                        aCommandPlay->AddActionMangerPiece(aArticleFrom, lArticleTo);
                    return true;
                }
                else return ARayonX;
            }
        }//if (lPieceExistante)
    }//for

    //à la toute fin, si pas de piece intermediaire, la met où on veut
    if (aCommandPlay) {
        if (aTypeDir == edirX) {
            aCommandPlay->AddActionMove(aArticleFrom, aFin, aInvariant);
        }else
            aCommandPlay->AddActionMove (aArticleFrom, aInvariant, aFin);
    }

    return true;
}


bool IRuleMovePiece::get_SquaresForMoves (const IChessManager &aIChessManager,
                                          const ArticlePiece& aArticleFrom,
                                          const CommandPlay* const aPreviousPlay,
                                          tQVectorInfosSquare& aListMoves,
                                          bool aCalculDefenseursAttaquants,
                                          bool ARayonX) const {

    aListMoves.clear();

    QVector<eTypeDirection> lDirections;

    if (!getDirections(lDirections)) return false;

    int lnewPosX, lnewPosY;

    int lNbreEssaisMax = 8;
    int lNbreEssais    = 0;
    if (aArticleFrom._TypePiece == Pawn)
        lNbreEssaisMax = 2;//pour le debut
    if (aArticleFrom._TypePiece == King)
        lNbreEssaisMax = 2;//pour le roque

    for (int i = 0; i < lDirections.count(); i++) {

        lnewPosX = aArticleFrom._PosX;
        lnewPosY = aArticleFrom._PosY;
        switch (lDirections[i]) {
              case edirDiag : {
                        lNbreEssais =0;
                        while ((lnewPosX < 8) && (lnewPosY < 8) && (lNbreEssais <= lNbreEssaisMax)) {
                                lnewPosX += 1;
                                lnewPosY += 1;
                                lNbreEssais++;
                                AddPossibleMoves (aIChessManager,
                                                  aArticleFrom,
                                                  lnewPosX, lnewPosY,
                                                  aPreviousPlay,
                                                  aListMoves,
                                                  aCalculDefenseursAttaquants,
                                                  ARayonX);
                            }
                        lnewPosX = aArticleFrom._PosX;
                        lnewPosY = aArticleFrom._PosY;
                        lNbreEssais =0;
                        while ((lnewPosX > 1) && (lnewPosY > 1) && (lNbreEssais <= lNbreEssaisMax)) {
                                lnewPosX -= 1;
                                lnewPosY -= 1;
                                lNbreEssais++;
                                AddPossibleMoves (aIChessManager,
                                                  aArticleFrom,
                                                  lnewPosX, lnewPosY,
                                                  aPreviousPlay,
                                                  aListMoves,
                                                  aCalculDefenseursAttaquants,
                                                  ARayonX);
                            }
                        lnewPosX = aArticleFrom._PosX;
                        lnewPosY = aArticleFrom._PosY;
                        lNbreEssais =0;
                        while ((lnewPosX > 1) && (lnewPosY < 8) && (lNbreEssais <= lNbreEssaisMax)) {
                                lnewPosX -= 1;
                                lnewPosY += 1;
                                lNbreEssais++;
                                AddPossibleMoves (aIChessManager,
                                                  aArticleFrom,
                                                  lnewPosX, lnewPosY,
                                                  aPreviousPlay,
                                                  aListMoves,
                                                  aCalculDefenseursAttaquants,
                                                  ARayonX);
                            }
                        lnewPosX = aArticleFrom._PosX;
                        lnewPosY = aArticleFrom._PosY;
                        lNbreEssais =0;
                        while ((lnewPosX < 8) && (lnewPosY > 1) && (lNbreEssais <= lNbreEssaisMax)) {
                                lnewPosX += 1;
                                lnewPosY -= 1;
                                lNbreEssais++;
                                AddPossibleMoves (aIChessManager,
                                                  aArticleFrom,
                                                  lnewPosX, lnewPosY,
                                                  aPreviousPlay,
                                                  aListMoves,
                                                  aCalculDefenseursAttaquants,
                                                  ARayonX);
                            }
                        break;
                    }//case edirDiag

                case edirX : {
                                lNbreEssais =0;
                                while ((lnewPosX < 8) && (lNbreEssais <= lNbreEssaisMax)) {
                                        lnewPosX += 1;
                                        lNbreEssais++;
                                        AddPossibleMoves (aIChessManager,
                                                          aArticleFrom,
                                                          lnewPosX, lnewPosY,
                                                          aPreviousPlay,
                                                          aListMoves,
                                                          aCalculDefenseursAttaquants,
                                                          ARayonX);
                                    }
                                lnewPosX = aArticleFrom._PosX;
                                lNbreEssais =0;
                                while ((lnewPosX > 1) && (lNbreEssais <= lNbreEssaisMax)) {
                                        lnewPosX -= 1;
                                        lNbreEssais++;
                                        AddPossibleMoves (aIChessManager,
                                                          aArticleFrom,
                                                          lnewPosX, lnewPosY,
                                                          aPreviousPlay,
                                                          aListMoves,
                                                          aCalculDefenseursAttaquants,
                                                          ARayonX);
                                    }
                                break;
                                }//case edirX

                case edirY : {
                                lNbreEssais =0;
                                while ((lnewPosY < 8) && (lNbreEssais <= lNbreEssaisMax)) {
                                       lnewPosY += 1;
                                       lNbreEssais++;
                                       AddPossibleMoves (aIChessManager,
                                                         aArticleFrom,
                                                         lnewPosX, lnewPosY,
                                                         aPreviousPlay,
                                                         aListMoves,
                                                         aCalculDefenseursAttaquants,
                                                         ARayonX);
                                   }
                               lnewPosY = aArticleFrom._PosY;
                               lNbreEssais =0;
                               while ((lnewPosY > 1) && (lNbreEssais <= lNbreEssaisMax)) {
                                       lnewPosY -= 1;
                                       lNbreEssais++;
                                       AddPossibleMoves (aIChessManager,
                                                         aArticleFrom,
                                                         lnewPosX, lnewPosY,
                                                         aPreviousPlay,
                                                         aListMoves,
                                                         aCalculDefenseursAttaquants,
                                                         ARayonX);
                                   }
                                break;
                               }//case edirY

        }
    }

    return (aListMoves.count() > 0);
}

void IRuleMovePiece::AddPossibleMoves (const IChessManager &aIChessManager,
                                       const ArticlePiece& aArticleFrom,
                                       int anewPosX, int anewPosY,
                                       const CommandPlay* const aPreviousPlay,
                                       tQVectorInfosSquare& aListMoves,
                                       bool aCalculDefenseursAttaquants,
                                       bool ARayonX) const {


    tlisteArticlesPieces lPiecesDefenseurs, lPiecesAttaquants;
    eColorPiece lColorEnnemy = aIChessManager.get_ColorEnnemy(aArticleFrom._Color);

    if (AllowMove (aIChessManager,
                   aArticleFrom._PosX,
                   aArticleFrom._PosY,
                   anewPosX,
                   anewPosY,
                   aPreviousPlay,
                   true,//verif king is checked (aArticleFrom._TypePiece==King)
                   false,//do no check friend final square
                   ARayonX)) {

        if (aCalculDefenseursAttaquants) {
            aIChessManager.ColorIsUnderAttackForPosition(lColorEnnemy,
                                                  anewPosX, anewPosY,
                                                  lPiecesDefenseurs);
            aIChessManager.ColorIsUnderAttackForPosition(aArticleFrom._Color,
                                              anewPosX, anewPosY,
                                              lPiecesAttaquants);
        }

        aListMoves.push_back(sInfosSquare (anewPosX, anewPosY,
                                           lPiecesDefenseurs, lPiecesAttaquants));
    }
}
