#include "evaluatorposition.h"
#include <QtAlgorithms>
#include <QtCore/qmath.h>
#include <QtGlobal>
#include "ChessBoardManager/chessmanager.h"
#include "ChessBoardManager/commandplay.h"
//#include "floatfann.h"

//todo safety king + structure pions + promotion...

const int CPenalitePieceAttaqueeParPion = 50;
const int CPenalitePieceAttaquee        = 15;
const int CPenaliteDev                  = 15;
const int CPenalitePasDeRoque           = 35;
const int CPoidsMateriel                = 50;
const int CBonusControleCentre          = 10;
const int CBonusControleLoin            = 4;
const int CPoidsMobilite                = 2;
const int CPenaliteDejaJouee            = 15;
const int CBonusAutoProtect             = 10;
const int CBonusRookAlignVert           = 35;
const int CPenaliteJoueeTropTot         = 10;
const int CPoidsCaseEnnemieAttaquee     = 5;
const int CBonusPieceAvancee            = 10;
const int CPenaliseBadBishop            = 12;
const int CPenaliseVeryBadBishop        = 22;
const int CPoidsInciteDev               = 20;
const int CBonusKingEnnemyIsChecked     = 33;
const int CPenaliteKingNotSecure        = 22;
const int CBonusKingSecure              = 30;
const int CPoidsPenalitePieceClouee     = 4;
const int CBonusRookNoFriendPawns       = 30;
const int CBonusRookNoEnnemyPawns       = 20;
const int CPenaliteRookWithFriendPawns  = 15;
const int CPoidsPieceDefendueEtPasAttaquee = 2;
const int CBonusPionPasse               = 15;
const int CPoidsPionProchePromotion     = 3;
const int CBonusProtegePieceAttaquee    = 15;
const int CBonusProtegePiece            = 5;
const int CPenalitePionPasDefendu       = 10;
const int CBonusEnnemyEvalueMat         = 500;
const int CBonusKingSousHautePression   = 50;
const int CBonusSecuriteEchangePiece    = 17;
const int CPoidsRapprochementPionPasse  = 3;
const int CBonusAvancePromotion         = 50;
const int CBonusMenaceMat               = 40;
const int CPenaliteRisqueMat            = 1000;
const int CBonusEnnemyEvalueMatCertain  = 5000;
const int CPenaliteEnnemyHasPawnPassed  = 15;
const int CBonusAttackPathPassedPawnNearPromotion = 10;
const int CPoidsValeurPieceMobilite     = 15;
const int CBonusPionDeRoiProtegee       = 13;
const int CPenalitePourInciterRookAlignVertSurOpenCol = 17;
const int CBonusPourInciterRookAlignVertSurOpenCol = 14;

EvaluatorPosition::EvaluatorPosition(ChessManager &aChessManager):
    _ChessManager(aChessManager),
    _UseNN(false)
{
}

bool EstDebutPartie (int aCurrentMove) {
    return (aCurrentMove < 10);
}

bool EstMilieuPartie (int aCurrentMove) {
    return (!EstDebutPartie (aCurrentMove) && (aCurrentMove <= 70));
}

bool EstFinale (int aCurrentMove) {

    return (!EstDebutPartie(aCurrentMove) && !EstMilieuPartie(aCurrentMove));
}

bool EvaluatorPosition::EstUnePieceDeveloppee (const ArticlePiece& aPiece) const {

    return ((aPiece._TypePiece != Pawn)
            && aPiece._HasEverMoved
            && (aPiece._PosY != _ChessManager.get_PosYKingBoard(aPiece._Color)));
}

float EvaluatorPosition::EstimationPosition (int acurrentDemiMove,
                                             eColorPiece aColor,
                                             const CommandPlay &aCommandeJouee) const {

  tQMapValeursCriteres lValeursCriteres;
  float lEvaluationColor  = EstimationPosition (acurrentDemiMove, aColor, aCommandeJouee, lValeursCriteres);
  return (lEvaluationColor);
}

float EvaluatorPosition::EstimationPosition (int acurrentDemiMove,
                                             eColorPiece aColor,
                                             const CommandPlay &aCommandeJouee,
                                             tQMapValeursCriteres &aValeursCriteres) const {

    EvaluationPositionStatique (acurrentDemiMove, aColor, aCommandeJouee, aValeursCriteres);
    if (!_UseNN)
        return EstimationBasique(aValeursCriteres);
    else
        return EstimationNN(aValeursCriteres, acurrentDemiMove);

}

bool EvaluatorPosition::getListePossibleSquares (const ArticlePiece& aPiece,
                                                 const CommandPlay &aCommandeJouee,
                                                 QMap<int, tQVectorInfosSquare>& aMapPosPieceListePossibleSquares,
                                                 tQVectorInfosSquare& aListSquares) const {

    int lPos = _ChessManager.qHashKey (aPiece._PosX, aPiece._PosY);
    if (!aMapPosPieceListePossibleSquares.contains(lPos)) {
        _ChessManager.get_SquaresForMoves (aPiece,
                                           &aCommandeJouee,
                                           aListSquares,
                                           false);
        aMapPosPieceListePossibleSquares [lPos] = aListSquares;
    }
    else
        aListSquares = aMapPosPieceListePossibleSquares [lPos];

    return (aListSquares.count()>0);
}

bool EstUnePieceMobile (eTypePiece aTypePiece) {

    return (aTypePiece != Pawn);
}

class DirectionsSearch {

    protected:
        ArticlePiece _KingEnnemy;
        ArticlePiece _PieceDonnantEchec;
    public:
        DirectionsSearch (const ArticlePiece& aKingEnnemy,
                               const ArticlePiece& aPieceDonnantEchec)
            : _KingEnnemy(aKingEnnemy), _PieceDonnantEchec(aPieceDonnantEchec) {}
        virtual int getXToSearch(int aPos)=0;
        virtual int getYToSearch(int aPos)=0;
        virtual int getMinDomain()=0;
        virtual int getMaxDomain()=0;
};

class DirectionsSearchHoriz : public DirectionsSearch {

    public :
        DirectionsSearchHoriz (const ArticlePiece& aKingEnnemy,
                               const ArticlePiece& aPieceDonnantEchec)
            : DirectionsSearch(aKingEnnemy, aPieceDonnantEchec) {}

        virtual int getXToSearch(int aPos) {return aPos;}
        virtual int getYToSearch(int ) {return _KingEnnemy._PosY;}
        virtual int getMinDomain() {return (1+qMin<int> (_KingEnnemy._PosX, _PieceDonnantEchec._PosX));}
        virtual int getMaxDomain() {return (qMax<int> (_KingEnnemy._PosX, _PieceDonnantEchec._PosX)-1);}
};

class DirectionsSearchVert: public DirectionsSearch {

    public :
    DirectionsSearchVert (const ArticlePiece& aKingEnnemy,
                          const ArticlePiece& aPieceDonnantEchec)
        : DirectionsSearch(aKingEnnemy, aPieceDonnantEchec) {}

        virtual int getXToSearch(int ) {return _KingEnnemy._PosX;}
        virtual int getYToSearch(int aPos) {return aPos;}
        virtual int getMinDomain() {return (1+qMin<int> (_KingEnnemy._PosY, _PieceDonnantEchec._PosY));}
        virtual int getMaxDomain() {return (qMax<int> (_KingEnnemy._PosY, _PieceDonnantEchec._PosY)-1);}
};

class DirectionsSearchDiag: public DirectionsSearch {

    private:
        int _MinPosX,_MaxPosX, _Orient, _PosYFromMinX;
    public :
    DirectionsSearchDiag (const ArticlePiece& aKingEnnemy,
                          const ArticlePiece& aPieceDonnantEchec)
        : DirectionsSearch(aKingEnnemy, aPieceDonnantEchec) {

        _MinPosX      = qMin<int> (_KingEnnemy._PosX, _PieceDonnantEchec._PosX);
        _MaxPosX      = qMax<int> (_KingEnnemy._PosX, _PieceDonnantEchec._PosX);
        _Orient       = 0;
        _PosYFromMinX = 0;

        if (_MaxPosX == _KingEnnemy._PosX) {
            _Orient        = (_KingEnnemy._PosY - _PieceDonnantEchec._PosY)/qAbs<int>(_KingEnnemy._PosY - _PieceDonnantEchec._PosY);
            _PosYFromMinX  = _PieceDonnantEchec._PosY;
        }
        else {
            _Orient = (_PieceDonnantEchec._PosY - _KingEnnemy._PosY)/qAbs<int>(_KingEnnemy._PosY - _PieceDonnantEchec._PosY);
            _PosYFromMinX  = _KingEnnemy._PosY;
        }
    }

        virtual int getXToSearch(int aPos) {return aPos;}
        virtual int getYToSearch(int aPos) {return _PosYFromMinX + _Orient*(aPos - _MinPosX);}
        virtual int getMinDomain() {return (_MinPosX+1);}
        virtual int getMaxDomain() {return (_MaxPosX-1);}
};

bool EvaluatorPosition::IlExistePiecesAInterposerProtegees (const CommandPlay &aPreviousPlay,
                                                            const ArticlePiece& aKingEnnemy,
                                                            const ArticlePiece &aPieceDonnantEchec) const {

    switch (aPieceDonnantEchec._TypePiece) {

        case Knight : return false; //exception du Cavalier car il saute
        case Pawn   : return false; //exception du Pion car attaque directe
        default     : break;

    }

    tlisteArticlesPieces lPiecesEnnemy;
    if (!_ChessManager.getArticlesPiecesDeCouleur (aKingEnnemy._Color,
                                                   lPiecesEnnemy)) return false;
    DirectionsSearch *lSearch =0;

    if (aKingEnnemy._PosY == aPieceDonnantEchec._PosY)
        lSearch = new DirectionsSearchHoriz (aKingEnnemy, aPieceDonnantEchec);
    else if (aKingEnnemy._PosX == aPieceDonnantEchec._PosX)
        lSearch = new DirectionsSearchVert (aKingEnnemy, aPieceDonnantEchec);
    else
        lSearch = new DirectionsSearchDiag (aKingEnnemy, aPieceDonnantEchec);

    //parcourt les cases entre les pieces et voit si on a une piece qui peut s'interposer tout en Ètant protÈgÈe

    for (int lPos = lSearch->getMinDomain(); lPos <= lSearch->getMinDomain(); lPos++) {

        for (int i=0; i < lPiecesEnnemy.count();i++) {

            tlisteArticlesPieces lPiecesAttaquantsCase;
            int lPosX = lSearch->getXToSearch (lPos);
            int lPosY = lSearch->getYToSearch (lPos);

            if (_ChessManager.AllowedMove (lPiecesEnnemy[i]._PosX, lPiecesEnnemy[i]._PosY,
                                           lPosX, lPosY,
                                           &aPreviousPlay,
                                           true,
                                           false)
                    && _ChessManager.ColorIsUnderAttackForPosition(_ChessManager.get_ColorEnnemy(aKingEnnemy._Color),
                                                                   lPosX,
                                                                   lPosY,
                                                                   lPiecesAttaquantsCase)
                    && (!lPiecesAttaquantsCase.contains(lPiecesEnnemy[i]) || (lPiecesAttaquantsCase.count()>=2)))

                return true;


        }//for (int i=0; i < lPiecesEnnemy.count();i++)
    }//for (int lPos = lSearch->getMinDomain()

    delete lSearch;
    return false;

}

bool EvaluatorPosition::AMoveLeadToMate (const CommandPlay &aEnnemyCommandMove, int aIter) const {

    tlisteArticlesPieces lPiecesFriends, lPieces;

    eColorPiece lColorOpposed = _ChessManager.get_ColorEnnemy(aEnnemyCommandMove.getColor());
    //liste les pieces considerees pour donner echec au roi
    _ChessManager.findPieces(lColorOpposed, Rook, lPiecesFriends);
    if (_ChessManager.findPieces(lColorOpposed, Queen, lPieces))
        lPiecesFriends += lPieces;
    if (_ChessManager.findPieces(lColorOpposed, Knight, lPieces))
        lPiecesFriends += lPieces;
    if (_ChessManager.findPieces(lColorOpposed, Bishop, lPieces))
        lPiecesFriends += lPieces;

    if (lPiecesFriends.count() == 0) return false;

    bool lLeadToMate = false;

    //parcours les pieces de l'ordi qui peuvent faire echec
    for (int i = 0; i < lPiecesFriends.count(); i++) {

        tQVectorInfosSquare lListSquaresPiece;
        _ChessManager.get_SquaresForMoves (lPiecesFriends[i],
                                           &aEnnemyCommandMove,
                                           lListSquaresPiece,
                                           false);
        for (tQVectorInfosSquare::const_iterator itSquarePiece = lListSquaresPiece.begin();
             itSquarePiece != lListSquaresPiece.end();
             ++itSquarePiece) {

                CommandPlay *lCommandPlayPiece = 0;
                if (_ChessManager.Play(lPiecesFriends[i]._PosX, lPiecesFriends[i]._PosY,
                                      (*itSquarePiece)._PosX, (*itSquarePiece)._PosY,
                                      lCommandPlayPiece, &aEnnemyCommandMove)) {

                    if (lCommandPlayPiece->KingEnnemyIsChecked()) {

                        lLeadToMate = EnnemyWillBeMated (*lCommandPlayPiece,
                                                         aIter+1);
                    }

                    lCommandPlayPiece->UnExecute();
                    if (lLeadToMate) break;//on a trouvÈ une position qui fait le mat, on sort
                }
                if (lLeadToMate) break;//on a trouvÈ une piece qui fait le mat, on sort
           }//for (tQVectorInfosSquare::const_iterator itSquarePiece


            if (lLeadToMate) break;//un mvt de piece conduit ‡ mat, pas besoin d'aller chercher plus loin
        }//for lPiecesFriends.count()

    return lLeadToMate;

}

bool EvaluatorPosition::AllMovesFromKingLeadToMate (const CommandPlay &aCommandeJouee,
                                                    const ArticlePiece& aKingEnnemy,
                                                    const tQVectorInfosSquare& aListSquaresKing,
                                                    int aIter) const {

    if (aIter > 2) return false;//garde fou

    //parcours les moves possibles de roi. Si pour chacun d'entre eux il est possible de
    //faire encore echec avec une piece sans que piece menacee, sans interposition protegee et sans moves possibles alors mat!
    for (tQVectorInfosSquare::const_iterator itSquareKing = aListSquaresKing.begin();
         itSquareKing != aListSquaresKing.end();
         ++itSquareKing) {

        CommandPlay *lCommandPlayKing        = 0;

        if (_ChessManager.Play(aKingEnnemy._PosX, aKingEnnemy._PosY,
                               (*itSquareKing)._PosX, (*itSquareKing)._PosY,
                               lCommandPlayKing, &aCommandeJouee)) {

            bool lLeadToMate = AMoveLeadToMate (*lCommandPlayKing, aIter);
            lCommandPlayKing->UnExecute();
            if (!lLeadToMate) return false;//un mvt de Roi permet de pas faire mat, on sort
        }

    }//for

    return true;
}

bool EvaluatorPosition::EnnemyWillBeMated (const CommandPlay &aCommandeJouee,
                                           int aIter) const {

    //si coup ordi a mis en echec l'ennemi
    if (!aCommandeJouee.KingEnnemyIsChecked()) return false;

    ArticlePiece lKingEnnemy;
    eColorPiece lColorEnnemy = _ChessManager.get_ColorEnnemy(aCommandeJouee.getColor());
    if (!_ChessManager.getKing(lColorEnnemy, lKingEnnemy)) return false;

    //a) si echec double et que roi peut pas bouger c'est un mat!
    if ((aCommandeJouee.getPiecesGivingCheck().count()) >= 2) {

        tQVectorInfosSquare lListSquaresKing;
        if (!_ChessManager.get_SquaresForMoves (lKingEnnemy,
                                                   &aCommandeJouee,
                                                   lListSquaresKing,
                                                   false)) {

            return true;

        } else {

            return (AllMovesFromKingLeadToMate (aCommandeJouee, lKingEnnemy, lListSquaresKing, aIter));
        }

    }//if (aCommandeJouee.getPiecesGivingCheck() >=2)
    //b) Si une seule piece donnant echec
    else {

        tlisteArticlesPieces lPiecesAttaquantsCase;
        if (!_ChessManager.ColorIsUnderAttackForPosition(aCommandeJouee.getColor(),
                                                         aCommandeJouee.getPiecesGivingCheck()[0]._PosX,
                                                         aCommandeJouee.getPiecesGivingCheck()[0]._PosY,
                                                         lPiecesAttaquantsCase)) {


            ArticlePiece lKingEnnemy;
            if (!_ChessManager.getKing(_ChessManager.get_ColorEnnemy(aCommandeJouee.getColor()), lKingEnnemy)) return false;

          if (IlExistePiecesAInterposerProtegees (aCommandeJouee,
                                                  lKingEnnemy,
                                                  aCommandeJouee.getPiecesGivingCheck()[0]))
                return false;

            tQVectorInfosSquare lListSquaresKing;
            //si pas de moves possibles du roi et pas de piece ‡ interposer et protegee
            if (_ChessManager.get_SquaresForMoves (lKingEnnemy,
                                                   &aCommandeJouee,
                                                   lListSquaresKing,
                                                   false)) {

                //essai chaque move possible du roi ennemi
                //si pour chacun d'entre eux, une des pieces donne echec et mat alors excellent coup
                return (AllMovesFromKingLeadToMate (aCommandeJouee, lKingEnnemy, lListSquaresKing, aIter));
            }
            else  {
                return true;
            }


        }//si piece donnant echec pas menacee
        else {
            //tente le sacrifice (envisage le cas que d'un seul attaquant)
            if (lPiecesAttaquantsCase.count() == 1) {

                //test le move de l'attaquant
                CommandPlay *lCommandEnnemy = 0;
                if (_ChessManager.Play(lPiecesAttaquantsCase[0]._PosX, lPiecesAttaquantsCase[0]._PosY,
                                       aCommandeJouee.getPiecesGivingCheck()[0]._PosX, aCommandeJouee.getPiecesGivingCheck()[0]._PosY,
                                       lCommandEnnemy, &aCommandeJouee)) {

                    bool lLeadToMate = AMoveLeadToMate (*lCommandEnnemy, aIter);
                    lCommandEnnemy->UnExecute();
                    return lLeadToMate;

                }

            }

        }
    }

    return false;

}

bool EvaluatorPosition::EnnemyCanMateAfterMove (const CommandPlay &aCommandeJouee) const {

    bool lCanMate = AMoveLeadToMate (aCommandeJouee, 0);
    return lCanMate;
}

void EvaluatorPosition::EvaluationPositionStatique (int acurrentDemiMove,
                                                    eColorPiece aColor,
                                                    const CommandPlay &aCommandeJouee,
                                                    tQMapValeursCriteres &aValeursCriteres) const {

    aValeursCriteres.clear();

    float lMaterielPiece     = 0;
    float lPoidsMobilite       = 0;
    float lPoidsDeveloppement  = 0;
    float lPoidsControleCase   = 0;
    float lPoidsSafetyPieces   = 0;
    float lPoidsAgressivite    = 0;
    float lPoidsControleCentre = 0;
    float lValeurPieceMobilite = 0;

    bool lEstDebutPartie  = EstDebutPartie(acurrentDemiMove);
    bool lEstMilieuPartie = EstMilieuPartie(acurrentDemiMove);


    QMap<int, tPieceEvalueeVector> lMapPosDefenseurs, lMapPosAttaquants;
    QMap<int, tQVectorInfosSquare> lMapPosPieceListePossibleSquares;
    tlisteArticlesPieces lListePieces;


    //‡ faire qu'aprËs dÈbut partie pour pas perdre de temps (peu de chance que mat possible)
    if (!lEstDebutPartie) {

        //si avec ce coup, on est certain de mater, on sort
        if (EnnemyWillBeMated (aCommandeJouee)) {

            aValeursCriteres.insert(eAgressivitePieces, CBonusEnnemyEvalueMatCertain);
            return;
        }
        if (EnnemyCanMateAfterMove (aCommandeJouee)) {

            aValeursCriteres.insert(eSafetyPieces, -CBonusEnnemyEvalueMatCertain);
            return;
        }

        //cherche si pions ennemis se trouvent sur la derniere rangee avant promotion
        //auquel cas penalite si aucune piece sur la derniere rangee ou si aucun attaquant sur la case
        eColorPiece lColorEnnemy = _ChessManager.get_ColorEnnemy(aColor);
        tlisteArticlesPieces lPawnsNearPromotion;

        if (_ChessManager.findPieces(lColorEnnemy, Pawn, lPawnsNearPromotion)) {

            tlisteArticlesPieces lPiecesFriend;
            int lPosYPromotion = _ChessManager.get_PosYKingBoard(aColor);

            for (int i=0; i < lPawnsNearPromotion.count(); i++) {

                if ((qAbs<int> (lPawnsNearPromotion[i]._PosY - lPosYPromotion)) == 1) {

                    lPiecesFriend.clear();
                    ArticlePiece lPiece;
                    if (!_ChessManager.UnePieceExiste(lPawnsNearPromotion[i]._PosX,
                                                      lPosYPromotion,
                                                      lPiece)
                        && !_ChessManager.ColorIsUnderAttackForPosition(lColorEnnemy,
                                                                     lPawnsNearPromotion[i]._PosX,
                                                                     lPosYPromotion,
                                                                     lPiecesFriend))
                        lPoidsControleCase -= CPoidsMateriel*CalculerValeurMaterielPiece(ArticlePiece (-1, Queen, WHITE, 0, 0, true), acurrentDemiMove);

                }

            }
        }



        //evalue possibilite mat du couloir
        //si on risque le mat, sortir tout de suite avec mauvais score!
//        if (CouleurPeutMenacerMatDuCouloir (lColorEnnemy, aCommandeJouee)) {

//            aValeursCriteres.insert(eSafetyPieces, -CPenaliteRisqueMat);
//            return;
//        }
//        else if (CouleurPeutMenacerMatDuCouloir (aColor, aCommandeJouee))
//            lPoidsAgressivite += CBonusMenaceMat;


    }//if (!lEstDebutPartie)


    int lNbrePiecesDeveloppeesAuDebutPartie = 0;

    //boucle sur toutes les pieces pour la couleur
    if (_ChessManager.getArticlesPiecesDeCouleur(aColor, lListePieces)) {
        for (tlisteArticlesPieces::const_iterator it = lListePieces.begin();
             it!= lListePieces.end();
             ++it) {

            if (EstUnePieceDeveloppee (*it))
                lNbrePiecesDeveloppeesAuDebutPartie += 1;

            //calcul poids materiel dans evaluation totale
            float lValeurPiece      = CalculerValeurMaterielPiece ((*it), acurrentDemiMove);
            lMaterielPiece       += lValeurPiece;
            //on assimile la valeur mobilite piece ‡ la valeur du materiel
            if (EstUnePieceMobile ((*it)._TypePiece))
                lValeurPieceMobilite += lValeurPiece;

            //evalue la mobilite des pieces
            lPoidsMobilite       += EvaluerMobiliteStatiquePiece (*it,
                                                                  acurrentDemiMove,
                                                                  aCommandeJouee,
                                                                  lMapPosPieceListePossibleSquares);

            //evalue le developpement de la position
            lPoidsDeveloppement  += EvaluerDeveloppementStatiquePiece (*it, acurrentDemiMove);

            //evalue le controle des cases
            //mais compte pas les points defense tant que pas d'attaquants et que tout d√©but partie
            CalculerDefenseursAttaquantsDeCase (aColor,
                                                (*it)._PosX, (*it)._PosY,
                                                acurrentDemiMove,
                                                lMapPosDefenseurs, lMapPosAttaquants);

            //evalue la securite de la piece (meme si controle case mais attaquee)
            bool lEchangeEstFavorable = false;
            lPoidsSafetyPieces   += EvaluerSafetyPieceStatique (acurrentDemiMove,
                                                                (*it),
                                                                aCommandeJouee,
                                                                lMapPosDefenseurs, lMapPosAttaquants,
                                                                lMapPosPieceListePossibleSquares,
                                                                lEchangeEstFavorable);

            //evalue l'agressivite de la position de la piece sur ses deplacements possibles
            lPoidsAgressivite    += EvaluerAgressiviteStatiquePiece (*it, acurrentDemiMove,
                                                                     aCommandeJouee,
                                                                     lMapPosDefenseurs, lMapPosAttaquants,
                                                                     lEchangeEstFavorable,
                                                                     lMapPosPieceListePossibleSquares);

        }//for
    }//if


    //criteres generaux
    // structure pions...

    //controle maximum du centre (d4,e4, d5, e5) et le + loin possible
    //TODO : plutot ajout si lPoidsControleCase > 0 et estcentre alors bonus
    if (!EstFinale(acurrentDemiMove)) {

        lPoidsControleCentre += getPoidsControleCentre (aColor, _ChessManager.GetXPos('D'), 4, acurrentDemiMove, lMapPosDefenseurs, lMapPosAttaquants);
        lPoidsControleCentre += getPoidsControleCentre (aColor, _ChessManager.GetXPos('E'), 4, acurrentDemiMove, lMapPosDefenseurs, lMapPosAttaquants);
        lPoidsControleCentre += getPoidsControleCentre (aColor, _ChessManager.GetXPos('D'), 5, acurrentDemiMove, lMapPosDefenseurs, lMapPosAttaquants);
        lPoidsControleCentre += getPoidsControleCentre (aColor, _ChessManager.GetXPos('E'), 5, acurrentDemiMove, lMapPosDefenseurs, lMapPosAttaquants);
//        if (aColor == WHITE) {
            lPoidsControleCentre += getPoidsControleCentre (aColor, _ChessManager.GetXPos('D'), 6, acurrentDemiMove, lMapPosDefenseurs, lMapPosAttaquants);
            lPoidsControleCentre += getPoidsControleCentre (aColor, _ChessManager.GetXPos('E'), 6, acurrentDemiMove, lMapPosDefenseurs, lMapPosAttaquants);
//        }
//        else {
            lPoidsControleCentre += getPoidsControleCentre (aColor, _ChessManager.GetXPos('D'), 3, acurrentDemiMove, lMapPosDefenseurs, lMapPosAttaquants);
            lPoidsControleCentre += getPoidsControleCentre (aColor, _ChessManager.GetXPos('E'), 3, acurrentDemiMove, lMapPosDefenseurs, lMapPosAttaquants);
//        }

    }

    int lNbrePiecesDeveloppeesEnnemyAuDebutPartie = 0;

    //parcourt pieces ennemies
    if (_ChessManager.getArticlesPiecesDeCouleur(_ChessManager.get_ColorEnnemy(aColor), lListePieces)) {
        for (tlisteArticlesPieces::const_iterator it = lListePieces.begin();
             it!= lListePieces.end();
             ++it) {

            if (EstUnePieceDeveloppee (*it))
                lNbrePiecesDeveloppeesEnnemyAuDebutPartie += 1;

            float lValeurPiece = CalculerValeurMaterielPiece ((*it), acurrentDemiMove);
            //calcul poids materiel dans evaluation totale
            lMaterielPiece  -= lValeurPiece;
            //on assimile la valeur mobilite piece ‡ la valeur du materiel
            if (EstUnePieceMobile ((*it)._TypePiece))
                lValeurPieceMobilite -= lValeurPiece;
        }
    }

    //compare developpement pieces debut partie
    if (lEstDebutPartie || lEstMilieuPartie) {
        lPoidsDeveloppement += CPoidsInciteDev *(lNbrePiecesDeveloppeesAuDebutPartie - lNbrePiecesDeveloppeesEnnemyAuDebutPartie);
    }

    if (!EstFinale(acurrentDemiMove))
        lPoidsMobilite += lValeurPieceMobilite* CPoidsValeurPieceMobilite;

    aValeursCriteres.insert(ePoidsMateriel, lMaterielPiece*CPoidsMateriel);
    aValeursCriteres.insert(ePoidsMobilite, lPoidsMobilite);
    aValeursCriteres.insert(ePoidsDev, lPoidsDeveloppement);
    aValeursCriteres.insert(ePoidsControleCases, lPoidsControleCase);
    aValeursCriteres.insert(eSafetyPieces, lPoidsSafetyPieces);
    aValeursCriteres.insert(eAgressivitePieces, lPoidsAgressivite);
    aValeursCriteres.insert(ePoidsControleCentre, lPoidsControleCentre);
}


float EvaluatorPosition::CalculerValeurMaterielPiece (const ArticlePiece& aPiece, int aCurrentDemiMove) const {

    switch (aPiece._TypePiece) {
        case Rook   : return 7;
        case Knight : return 4;
        case Bishop : return 4;
        case Queen  : return 12;
        case Pawn   : {
                       float lValeur = 0.5;
                       const int CDeltaBord = 2;

                       if (!EstDebutPartie(aCurrentDemiMove)) {
                         int lDistance = DistancePawnFromPromotion(aPiece);
                         if (lDistance <= CDeltaBord) {
                             float lPoidsValeur = 3.0;
                             if (EstFinale(aCurrentDemiMove))
                                lPoidsValeur = 5.0;
                            lValeur = lPoidsValeur*(1 -((1/8)*DistancePawnFromPromotion(aPiece)));
                         }
                       }
                        return lValeur;
                      }
        default     : return 0;
    }
}

int EvaluatorPosition::EvaluerMobiliteStatiquePiece (const ArticlePiece& aPiece,
                                                     int aCurrentDemiMove,
                                                     const CommandPlay &aCommandeJouee,
                                                     QMap<int, tQVectorInfosSquare>& aMapPosPieceListePossibleSquares) const {

    int lPoids = 0;
    switch (aPiece._TypePiece) {
        case Rook   : {//Rook tres mobile si colonne ouverte
                        tlisteArticlesPieces llistePawns;
                        if (EstDebutPartie(aCurrentDemiMove) || EstMilieuPartie(aCurrentDemiMove)) {
                            bool lWithFriendPawnsSameColumn = false;
                            _ChessManager.findPieces(aPiece._Color, Pawn, llistePawns);

                            for (tlisteArticlesPieces::const_iterator it = llistePawns.begin();
                                 it != llistePawns.end();
                                 ++it) {

                                lWithFriendPawnsSameColumn = ((*it)._PosX == aPiece._PosX);
                                if (lWithFriendPawnsSameColumn) break;
                            }
                            if (lWithFriendPawnsSameColumn)
                              lPoids -= CBonusRookNoFriendPawns;//penalite inverse au bonus si pion devant

                            //si  pas de pions amis, compte bonus si pas non plus pions ennemis
                            if (!lWithFriendPawnsSameColumn) {

                                bool lWithEnnemyPawnsSameColumn = false;
                                _ChessManager.findPieces(_ChessManager.get_ColorEnnemy(aPiece._Color), Pawn, llistePawns);
                                for (tlisteArticlesPieces::const_iterator it = llistePawns.begin();
                                     it != llistePawns.end();
                                     ++it) {
                                    lWithEnnemyPawnsSameColumn = ((*it)._PosX == aPiece._PosX);
                                    if (lWithEnnemyPawnsSameColumn) break;
                                }
                                if (!lWithEnnemyPawnsSameColumn) {
                                  lPoids += CBonusRookNoEnnemyPawns + CBonusRookNoFriendPawns;
                                  //Si colonne ouverte incite une des 2 tours ‡ changer de rang si les 2 sont alignÈs horiz
                                  bool lAxeHoriz = false;
                                  if (_ChessManager.RooksAreAutoProtected(aPiece._Color, lAxeHoriz)) {

                                      if (lAxeHoriz)
                                         lPoids -= CPenalitePourInciterRookAlignVertSurOpenCol;
                                      else
                                         lPoids += CBonusPourInciterRookAlignVertSurOpenCol;
                                  }
                                }
                            }

                        }
                        break;
                      }
        case Knight : {
                            lPoids += CalculPoidsDistanceAuCentre (aPiece._PosX, aPiece._PosY);
                            break;
                        }
        case Bishop : {
                       //penalise pion mauvais
                        lPoids -= PenalisationBadBishop (aPiece._Color, aPiece._PosX, aPiece._PosY);
                        break;
                        }
        case Queen :
        case King  :
        case Pawn  : break;//pas de poids particuliers
    }

    tQVectorInfosSquare lListSquares;
    //plus la piece a de possibilites de mouvements, mieux c'est
    if ((aPiece._TypePiece != Pawn)
            && getListePossibleSquares (aPiece,
                                        aCommandeJouee,
                                        aMapPosPieceListePossibleSquares,
                                        lListSquares))
        lPoids += CPoidsMobilite*lListSquares.count ();

    return lPoids;
}

int EvaluatorPosition::EvaluerDeveloppementStatiquePiece (const ArticlePiece& aPiece,
                                                          int aCurrentDemiMove) const {

    int lPoids = 0;

    switch (aPiece._TypePiece) {
        //rook : favorise tours connectees
        case Rook: {bool lAxeHoriz = false;
                       if (_ChessManager.RooksAreAutoProtected (aPiece._Color, lAxeHoriz)) {
                         lPoids += CBonusAutoProtect;
                         if (!lAxeHoriz) lPoids += CBonusRookAlignVert;
                       }
                       break;
                    }

        //pieces legeres : p√©nalise positions o√π pas encore boug√©es!
        case Knight :
        case Bishop : {
                         if (EstDebutPartie(aCurrentDemiMove) || EstMilieuPartie(aCurrentDemiMove)) {
                              if ( ((aPiece._Color == WHITE) && (aPiece._PosY == 1))
                                   || ((aPiece._Color == BLACK) && (aPiece._PosY == 8)))
                                lPoids -= CPenaliteDev;
                          }
                          break;
                       }
        //si roi toujours pas roqu√©, p√©nalise
        case King : {
                       if (EstDebutPartie(aCurrentDemiMove) || EstMilieuPartie(aCurrentDemiMove)){
                             if (!_ChessManager.KingIsCastled(aPiece._Color)) {
                                 lPoids -= CPenalitePasDeRoque;
                             }
                             else
                                 lPoids += CBonusKingSecure;
                        }
                        break;
                    }
        //p√©nalit√© si dame bouge avant les 6 premiers demi-coups
        case Queen  : if ((aCurrentDemiMove <= 6) && aPiece._HasEverMoved)
                        lPoids -= CPenaliteJoueeTropTot;
                      break;//pas de dev trop tot pour la reine

        case Pawn   : //incite le developpement des pions centraux au d√©but de partie
                      if (EstDebutPartie (aCurrentDemiMove)
                              && aPiece._HasEverMoved
                              && ((aPiece._PosX == _ChessManager.GetXPos(('D'))) || (aPiece._PosX == _ChessManager.GetXPos(('E')))))
                          lPoids += CPoidsInciteDev;
                      break;

        default : break;
    }

    return lPoids;
}

bool EvaluatorPosition::FourchetteDePionPossible (const ArticlePiece& aPawn) const {

    int lOrient = 1;
    if (aPawn._Color == BLACK)
       lOrient = -1;

   ArticlePiece lPiece;
   bool lFourchette = _ChessManager.UnePieceExiste(aPawn._PosX+1, aPawn._PosY+lOrient, lPiece)
                       && (lPiece._Color != aPawn._Color)
                       && _ChessManager.UnePieceExiste(aPawn._PosX-1, aPawn._PosY+lOrient, lPiece)
                       && (lPiece._Color != aPawn._Color);
   return lFourchette;

}

float EvaluatorPosition::EvaluerSafetyPieceStatique (int aCurrentDemiMove,
                                                   const ArticlePiece& aPiece,
                                                   const CommandPlay &aCommandeJouee,
                                                   const QMap<int, tPieceEvalueeVector> &aMapPiecesDefenseurs,
                                                   const QMap<int, tPieceEvalueeVector> &aMapPiecesAttaquants,
                                                   QMap<int, tQVectorInfosSquare> &aMapPosPieceListePossibleSquares,
                                                   bool &aEchangeEstFavorable) const {

    float lPoids = 0;

    int lPos = _ChessManager.qHashKey(aPiece._PosX, aPiece._PosY);
    tPieceEvalueeVector lPiecesDefenseurs = aMapPiecesDefenseurs [lPos];
    tPieceEvalueeVector lPiecesAttaquants = aMapPiecesAttaquants [lPos];

    float lvaleurPiece = CalculerValeurMaterielPiece(aPiece, aCurrentDemiMove);

    bool lKingEnnemyIsChecked = aCommandeJouee.KingEnnemyIsChecked();

    float lPoidsEchange     = 0;
    float lPoidsEchangeMini = -1;
    //evalue l'avantage ou desavantage de l'echange
    if ( ((aPiece._TypePiece != Pawn) || FourchetteDePionPossible (aPiece) ) &&
          (aPiece._TypePiece != King)
            && (lPiecesAttaquants.count() > 0)//et qu'il a des attaquants
            && (!lKingEnnemyIsChecked //et qu'on ne met pas l'ennemi en echec par le dernier coup (car ennemy ne peut tuer notre piece sauf si echec unique et que c'est la piece consideree qui donne l'echec)
                || ((aCommandeJouee.getPiecesGivingCheck().count() == 1) && (aCommandeJouee.getPiecesGivingCheck() [0] == aPiece)))) {

        //pour evaluer echange, tri par ordre croissant de valeur
        qSort (lPiecesDefenseurs.begin(), lPiecesDefenseurs.end(), sPieceEvalueeValeursCroissantes());
        qSort (lPiecesAttaquants.begin(), lPiecesAttaquants.end(), sPieceEvalueeValeursCroissantes());

        //si le plus faible des attaquants est un pion (fourchette du pion)
        if (lPiecesAttaquants [0]._Piece._TypePiece == Pawn) {

            QVector< QPair <int, int> > lPairs;
            lPairs.push_back( QPair <int, int> (lPiecesAttaquants [0]._Piece._PosX -1, lPiecesAttaquants [0]._Piece._PosY +1));
            lPairs.push_back( QPair <int, int> (lPiecesAttaquants [0]._Piece._PosX -1, lPiecesAttaquants [0]._Piece._PosY -1));
            lPairs.push_back( QPair <int, int> (lPiecesAttaquants [0]._Piece._PosX +1, lPiecesAttaquants [0]._Piece._PosY +1));
            lPairs.push_back( QPair <int, int> (lPiecesAttaquants [0]._Piece._PosX +1, lPiecesAttaquants [0]._Piece._PosY -1));
            ArticlePiece lAutrePiece;
            for (int i=0; i < lPairs.count(); i++) {

                QPair <int, int> lPair = lPairs [i];

                if (! ((lPair.first == aPiece._PosX) &&
                        (lPair.second == aPiece._PosY)) &&
                    _ChessManager.PieceAttacksThePosition(lPiecesAttaquants [0]._Piece, lPair.first, lPair.second) &&
                    _ChessManager.UnePieceExiste(lPair.first, lPair.second, lAutrePiece) &&
                    (lAutrePiece._Color == aPiece._Color) &&
                    (lAutrePiece._TypePiece != Pawn) ) {

                    float lValeurAutrePiece = CalculerValeurMaterielPiece(lAutrePiece, aCurrentDemiMove);
                    //lui enleve la piece car toutes les chances de la perdre
                    lPoids -= 0.5*CPoidsMateriel * qMin <float> (lValeurAutrePiece, lvaleurPiece);
                    break;
                }
            }


        }



        if (lPiecesDefenseurs.count() >= lPiecesAttaquants.count()) {
            for (int i=0; i < lPiecesAttaquants.count(); i++) {
                if (_ChessManager.SimulationPlayOk(lPiecesAttaquants [i]._Piece._PosX,
                                                   lPiecesAttaquants [i]._Piece._PosY,
                                                   aPiece._PosX, aPiece._PosY)) {
                    if (i==0) lPoidsEchange -= lvaleurPiece;
                    lPoidsEchange += CalculerValeurMaterielPiece (lPiecesAttaquants [i]._Piece, aCurrentDemiMove);
                    if (lPoidsEchange < 0) break;//l'ennemi arretera l'echange des que c'est favorable pour lui
                    if (i < (lPiecesAttaquants.count()-1))//le dernier defenseur reste
                        lPoidsEchange -= CalculerValeurMaterielPiece (lPiecesDefenseurs [i]._Piece, aCurrentDemiMove);
                    if ((lPoidsEchange > 0) && (lPoidsEchangeMini < 0))
                       lPoidsEchangeMini = lPoidsEchange;
                }
            }//for
        } else {
            for (int i=0; i < lPiecesAttaquants.count(); i++) {
                if (_ChessManager.SimulationPlayOk(lPiecesAttaquants [i]._Piece._PosX,
                                                   lPiecesAttaquants [i]._Piece._PosY,
                                                   aPiece._PosX, aPiece._PosY)) {
                    if (i==0) lPoidsEchange -= lvaleurPiece;
                    if (i >= lPiecesDefenseurs.count()) break;
                    lPoidsEchange += CalculerValeurMaterielPiece (lPiecesAttaquants [i]._Piece, aCurrentDemiMove);
                    lPoidsEchange -= CalculerValeurMaterielPiece (lPiecesDefenseurs [i]._Piece, aCurrentDemiMove);
                    if (lPoidsEchange < 0) break;//l'ennemi arretera l'echange des que c'est favorable pour lui
                }
            }//for
        }
    }//(aPiece._TypePiece != Pawn) || FourchetteDePionPossible (aPiece) ) &&....
    else {
        if (aPiece._TypePiece == Pawn) {

            ArticlePiece lKing;
            //Si il s'agit d'un pion qui est bien protege et qui est proche du Roi, on met un bonus
            if ((lPiecesAttaquants.count() > 0) &&
                    ((lPiecesDefenseurs.count() - lPiecesAttaquants.count()) > 0) &&
                    _ChessManager.getKing(aPiece._Color, lKing)) {

                float ldistance2 = qSqrt (qPow ((qAbs<int> (aPiece._PosX - lKing._PosX)), 2) +
                                                 qPow ((qAbs<int> (aPiece._PosY - lKing._PosY)), 2));
                if (ldistance2 < 2)
                    lPoids += CBonusPionDeRoiProtegee;

            }
        }
    }//else

    if ((lPoidsEchangeMini > 0) && ( lPoidsEchange > 0))//car ennemi s'arretera d'attaquer d√®s que son bilan negatif (donne pas trop de poids donc au coup joue!)
        lPoids += qMin<float> (lPoidsEchangeMini*CPoidsMateriel, CBonusSecuriteEchangePiece);
    else
        lPoids += qMin<float> (lPoidsEchange*CPoidsMateriel, CBonusSecuriteEchangePiece);


    aEchangeEstFavorable = (lPoidsEchangeMini > 0) || ( lPoidsEchange > 0);


    //√  ne faire qu'apr√®s debut partie
    if (!EstDebutPartie(aCurrentDemiMove)) {

        if ((aPiece._TypePiece == Pawn) && (lPiecesDefenseurs.count() == 0)) {
            lPoids -= CPenalitePionPasDefendu;
            if (lPiecesAttaquants.count()> 0) {
              lPoids -= CPenalitePionPasDefendu;
              if (EstFinale(aCurrentDemiMove))
                lPoids -= CPenalitePionPasDefendu;
            }
        }

        //encourage mettre piece sur case pas attaquee et defendue
        if (aPiece._TypePiece != King) {
            if ( (lPiecesAttaquants.count() == 0) //si pas d'attaquants
                 && (lPiecesDefenseurs.count() > 0)) {
                lPoids += lvaleurPiece*CPoidsPieceDefendueEtPasAttaquee;
            }
        }
    }else {

        switch (aPiece._TypePiece) {

            case Pawn : { if ((lPiecesDefenseurs.count() == 0) &&//pion pas defendu
                              (lPiecesAttaquants.count()> 0) && //pion attaque
                              ((aPiece._PosX == 4) || (aPiece._PosX == 5) ))//pion central
                            lPoids -= 2*CPenalitePionPasDefendu;
                          break;
                        }
            default   : break;
        }
    }



    //securite du roi
    if (aPiece._TypePiece == King) {

        //penalite si roi attaqu√© par des pieces
        lPoids -= (lPiecesAttaquants.count())*CPenaliteKingNotSecure;

        if (EstDebutPartie (aCurrentDemiMove) || EstMilieuPartie(aCurrentDemiMove)) {

            int lPosSecondLine = 2;
            if (aPiece._Color == BLACK) {
               lPosSecondLine = 7;
            }

            lPoids -= qAbs<int> (aPiece._PosY- _ChessManager.get_PosYKingBoard(aPiece._Color))*CPenaliteKingNotSecure;

            //barriere de pions
            ArticlePiece lPion;
            if (aPiece._PosX <= _ChessManager.GetXPos('C')) {

                bool lPionAExiste = _ChessManager.UnePieceExiste(1, lPosSecondLine, lPion)
                        && (lPion._TypePiece == Pawn)
                        && (lPion._Color == aPiece._Color);
                bool lPionBExiste = _ChessManager.UnePieceExiste(2, lPosSecondLine, lPion)
                        && (lPion._TypePiece == Pawn)
                        && (lPion._Color == aPiece._Color);
                bool lPionCExiste = _ChessManager.UnePieceExiste(3, lPosSecondLine, lPion)
                        && (lPion._TypePiece == Pawn)
                        && (lPion._Color == aPiece._Color);
                if (!lPionAExiste || !lPionBExiste || !lPionCExiste)
                   lPoids -= CPenaliteKingNotSecure;
                else
                   lPoids += CBonusKingSecure;
            }
            if (aPiece._PosX >= _ChessManager.GetXPos('G')) {

                bool lPionGExiste = _ChessManager.UnePieceExiste(7, lPosSecondLine, lPion)
                        && (lPion._TypePiece == Pawn)
                        && (lPion._Color == aPiece._Color);

                bool lPionHExiste = _ChessManager.UnePieceExiste(8, lPosSecondLine, lPion)
                        && (lPion._TypePiece == Pawn)
                        && (lPion._Color == aPiece._Color);
                if (!lPionGExiste || !lPionHExiste)
                   lPoids -= CPenaliteKingNotSecure;
                else
                   lPoids += CBonusKingSecure;
            }

            //si le roi pas encore roqu√© et qu'il ne peut pas car son path est attaqu√©, p√©nalit√©!
            if (!_ChessManager.KingIsCastled(aPiece._Color)) {
                int lPosY = 1;
                if (aPiece._Color == BLACK)
                   lPosY = 8;
                sDiagnostiqueCastling lDiagCastling;
                if (!_ChessManager.CastlingIsPossibleForKing (aPiece,
                                                              _ChessManager.getInfosSmallCastling()._PosKingDest,
                                                              lPosY,
                                                              lDiagCastling) && lDiagCastling._PathIsAttacked)
                   lPoids -= CPenaliteKingNotSecure;
                if (!_ChessManager.CastlingIsPossibleForKing (aPiece,
                                                              _ChessManager.getInfosBigCastling()._PosKingDest,
                                                              lPosY,
                                                              lDiagCastling) && lDiagCastling._PathIsAttacked)
                   lPoids -= CPenaliteKingNotSecure;


            }
        }//if debut partie ou milieu partie

    }

    //penalite si piece clouee
    bool lPieceClouee = false;
    if ((aPiece._TypePiece != King) && (lPiecesAttaquants.count()>0)) {

        ArticlePiece lPieceEnnemy;
        lPieceClouee = _ChessManager.PieceEstClouee (aPiece, lPieceEnnemy);
    }
    if (lPieceClouee)
        lPoids -= CPoidsPenalitePieceClouee*CalculerValeurMaterielPiece(aPiece, aCurrentDemiMove);

    return lPoids;
}


//piece en direction du roi, une piece ennemie peut-etre attaqu√©e, on va le + loin possible sur echiquier
int EvaluatorPosition::EvaluerAgressiviteStatiquePiece (const ArticlePiece& aPiece,
                                                        int aCurrentDemiMove,
                                                        const CommandPlay &aCommandeJouee,
                                                        const QMap<int, tPieceEvalueeVector> &aMapPiecesDefenseurs,
                                                        const QMap<int, tPieceEvalueeVector> &aMapPiecesAttaquants,
                                                        const bool aEchangeEstFavorable,
                                                        QMap<int, tQVectorInfosSquare> &aMapPosPieceListePossibleSquares) const {

    int lPoids = 0;

    /*** regarde si dans les cases accessibles se trouvent des ennemis***/
    /*** on privilegie l'agression √  condition que la piece soit prot√©g√©e!! ***/
    tQVectorInfosSquare lListSquares;
    ArticlePiece lPiece;//get_squaresForAttack

    int lPos = _ChessManager.qHashKey(aPiece._PosX, aPiece._PosY);
    tPieceEvalueeVector lPiecesDefenseurs = aMapPiecesDefenseurs [lPos];
    tPieceEvalueeVector lPiecesAttaquants = aMapPiecesAttaquants [lPos];
    eColorPiece lColorEnnemy = _ChessManager.get_ColorEnnemy(aPiece._Color);

    float lValeurPiece = CalculerValeurMaterielPiece(aPiece, aCurrentDemiMove);
    qSort (lPiecesAttaquants.begin(), lPiecesAttaquants.end(), sPieceEvalueeValeursCroissantes());
    //si pas d'attaquants ‡ la piece ou alors que le plus faible d'entre eux ait encore une valeur plus grande ‡ notre piece
    if ( (lPiecesAttaquants.count() == 0) || aEchangeEstFavorable) {

        if (getListePossibleSquares (aPiece,
                                     aCommandeJouee,
                                     aMapPosPieceListePossibleSquares,
                                     lListSquares)) {

            QVector<float> lListValeursAttaque;
            bool lAttaqueKingEnnemy   = false;

            tlisteArticlesPieces lPiecesAttaquantsCase;
            for (tQVectorInfosSquare::const_iterator itSquare = lListSquares.begin();
                 itSquare != lListSquares.end();
                 ++itSquare) {

                lPiecesAttaquantsCase.clear();
                _ChessManager.ColorIsUnderAttackForPosition(aPiece._Color,
                                                              (*itSquare)._PosX, (*itSquare)._PosY,
                                                              lPiecesAttaquantsCase);

                //todo faire une get_SquaresForAttacks
                bool lUnePieceExiste = _ChessManager.UnePieceExiste((*itSquare)._PosX, (*itSquare)._PosY, lPiece);
                bool lAttack         = lUnePieceExiste && (lPiece._Color != aPiece._Color);
                bool lProtege        = lUnePieceExiste && (lPiece._Color == aPiece._Color);

                //si on attaque un ennemi et qu'il est pas d√©fendu, encourage le coup!
                if (lAttack) {

                    if (lPiece._TypePiece != King) {

                        float lValeurPieceAttaquee = CalculerValeurMaterielPiece (lPiece, aCurrentDemiMove);

                        if( (lPiecesAttaquantsCase.count() == 0) || (lValeurPieceAttaquee>=lValeurPiece))
                        {
                            if (lPiecesAttaquantsCase.count() == 0)
                                lListValeursAttaque.push_back(lValeurPieceAttaquee);
                            else
                                lListValeursAttaque.push_back(lValeurPieceAttaquee-lValeurPiece);
                        }
                        else lPoids += CPoidsCaseEnnemieAttaquee;

                    }//if (lPiece._TypePiece != King)
                    else
                      lAttaqueKingEnnemy = true;
                }//if (lAttack)

                //ajoute bonus pour chaque piece protegee gr√¢ce √  celle-ci
                if (lProtege && (lPiece._TypePiece != King)) {
                    if (lPiecesAttaquantsCase.count() > 0)
                        lPoids += CBonusProtegePieceAttaquee;
                    else
                        lPoids += CBonusProtegePiece;
                }//if (lProtege)

            }//for itsquare

            qSort (lListValeursAttaque.begin(), lListValeursAttaque.end(), qGreater<float>());
            if (lAttaqueKingEnnemy && (lListValeursAttaque.count() >=1))
                lPoids += CPoidsMateriel*lListValeursAttaque [0];
            else if (lListValeursAttaque.count()>=2) {
                lPoids += CPoidsMateriel*lListValeursAttaque [1];
            }
            //else if (lListValeursAttaque.count()>=1) et peut pas bouger...

        }//if getsquare
    }

    //si pas d'echange possible et que roi mis en echec, alors jeu agressif!
    bool lSeulAttaquantEstRoiEnnemi = (lPiecesAttaquants.count() == 1) && (lPiecesAttaquants[0]._Piece._TypePiece == King);
    if ((aCommandeJouee.getPiecesGivingCheck().count() > 0)
            && (aCommandeJouee.getPiecesGivingCheck().contains(aPiece))
            && ( (lPiecesAttaquants.count() == 0) || (lSeulAttaquantEstRoiEnnemi && !getListePossibleSquares (lPiecesAttaquants[0]._Piece,
                                                                                                              aCommandeJouee,
                                                                                                              aMapPosPieceListePossibleSquares,
                                                                                                              lListSquares) ))) {

        lPoids += CBonusKingEnnemyIsChecked;

        ArticlePiece lKingEnnemy;
        if (lSeulAttaquantEstRoiEnnemi) lKingEnnemy = lPiecesAttaquants[0]._Piece;
        else {
            _ChessManager.getKing(lColorEnnemy, lKingEnnemy);
            getListePossibleSquares (lKingEnnemy,
                                     aCommandeJouee,
                                     aMapPosPieceListePossibleSquares,
                                     lListSquares);
        }

        //si le roi est en echec et qu'il ne peut plus bouger, evalue la possibilite de mat
        if (lListSquares.count() == 0) {

            bool lEnnemyEvalueMat = (aCommandeJouee.getPiecesGivingCheck().count() > 1)
                                        || (aPiece._TypePiece == Knight);
            if (!lEnnemyEvalueMat) {
                float ldistance = qSqrt (qPow ((qAbs<int> (aPiece._PosX - lKingEnnemy._PosX)), 2) +
                                             qPow ((qAbs<int> (aPiece._PosY - lKingEnnemy._PosY)), 2));
                lEnnemyEvalueMat =  (ldistance < 2);

            }

            if (lEnnemyEvalueMat)
               lPoids += CBonusEnnemyEvalueMat;
            else lPoids += CBonusKingSousHautePression;


        }
    }

    //agressivite mais prudence au d√©but
    if (!EstDebutPartie(aCurrentDemiMove) || (aPiece._TypePiece == Pawn)) {
        if ( (aPiece._Color == WHITE) && (aPiece._PosY >= 4))
           lPoids += CBonusPieceAvancee;
        else if ( (aPiece._Color == BLACK) && (aPiece._PosY <= 5))
           lPoids += CBonusPieceAvancee;
    }


    tlisteArticlesPieces llisteFriendPawns, llistePawnsEnnemies;
    _ChessManager.findPieces(aPiece._Color, Pawn, llisteFriendPawns);
    _ChessManager.findPieces(lColorEnnemy, Pawn, llistePawnsEnnemies);

    //si pion ennemi pass√© et proche de la promotion, il faut l'arreter!
    //if (EstFinale(aCurrentDemiMove) && (aPiece._TypePiece == King)) {
    if (!EstDebutPartie(aCurrentDemiMove)) {
        tlisteArticlesPieces lListeEnnemyPassedPawns;
        for (tlisteArticlesPieces::const_iterator itPawnEnnemy = llistePawnsEnnemies.begin();
             itPawnEnnemy != llistePawnsEnnemies.end();
             ++itPawnEnnemy) {
            if (PawnIsPassed ((*itPawnEnnemy), llisteFriendPawns))
                lListeEnnemyPassedPawns.push_back(*itPawnEnnemy);
        }//for
        if (lListeEnnemyPassedPawns.count() > 0) {

            //pour inciter ‡ tuer les pions passÈs proche de la promotion
            int lDistanceMini = 8;
            int lIndexPawn = 0;
            int lIndexPawnPassedLePlusProche = 0;
            for (tlisteArticlesPieces::const_iterator itPawnEnnemy= lListeEnnemyPassedPawns.begin();
                 itPawnEnnemy != lListeEnnemyPassedPawns.end();
                 ++itPawnEnnemy) {

                int lDistance = DistancePawnFromPromotion (*itPawnEnnemy);
                if ( lDistance <= 3) {

                    lPoids -= CPenaliteEnnemyHasPawnPassed;
                    if (lDistance < lDistanceMini) {
                        lDistanceMini = lDistance;
                        lIndexPawnPassedLePlusProche = lIndexPawn;

                    }
                }//if ( lDistance <= 3)
                lIndexPawn++;
            }//for

            if (lDistanceMini < 8) {

                switch (aPiece._TypePiece) {
                    case King: {
                        if (EstFinale(aCurrentDemiMove)) {
                            //encourage rapprochement du roi du pion pour le tuer (slt en finale)
                            float ldistance = qSqrt (qPow((aPiece._PosX - lListeEnnemyPassedPawns[lIndexPawnPassedLePlusProche]._PosX), 2) + qPow((aPiece._PosY - lListeEnnemyPassedPawns[lIndexPawnPassedLePlusProche]._PosY), 2));
                            lPoids += CPoidsRapprochementPionPasse*CalculerValeurMaterielPiece(lListeEnnemyPassedPawns[lIndexPawnPassedLePlusProche], aCurrentDemiMove)/ldistance;
                        }
                        break;
                    }//case King
                    default : {//il s'agit d'attaquer la position devant le pion ou de le bloquer
                                tlisteArticlesPieces lPiecesContrePathPawn;
                                int lPosYLim = _ChessManager.get_PosYKingBoard(aPiece._Color);
                                int lInc = 1;
                                if (aPiece._Color == WHITE)
                                    lInc = -1;
                                int lPosY    = lListeEnnemyPassedPawns[lIndexPawnPassedLePlusProche]._PosY;

                                while (lPosY != lPosYLim) {
                                    if (_ChessManager.ColorIsUnderAttackForPosition(lListeEnnemyPassedPawns[lIndexPawnPassedLePlusProche]._Color,
                                                                                    lListeEnnemyPassedPawns[lIndexPawnPassedLePlusProche]._PosX,
                                                                                    lPosY+lInc,
                                                                                    lPiecesContrePathPawn))

                                        lPoids += CBonusAttackPathPassedPawnNearPromotion;

                                    lPosY += lInc;
                                }

                                break;
                               }//default
                }//switch (aPiece._TypePiece)

             }//if (lDistanceMini < 8) {
        }//if (lListeEnnemyPassedPawns.count() > 0)

    }//if (EstFinale(aCurrentDemiMove) && (aPiece._TypePiece == King))

    //si pion passe et que proche promotion alors met le paquet
    if (!EstDebutPartie(aCurrentDemiMove) && (aPiece._TypePiece == Pawn)) {

        if (PawnIsPassed (aPiece, llistePawnsEnnemies)) {
            if ((lPiecesDefenseurs.count() - lPiecesAttaquants.count()) > 0) {

                lPoids += CBonusPionPasse;
                if (lValeurPiece > 1) lPoids += CPoidsPionProchePromotion*lValeurPiece;
                if (EstFinale(aCurrentDemiMove))
                    lPoids += -((CBonusAvancePromotion/8)*DistancePawnFromPromotion(aPiece))+CBonusAvancePromotion;

            }//si defense > attaque
        }//if PawnIsPassed

    }//if (!EstDebutPartie(aCurrentDemiMove) && (aPiece._TypePiece == Pawn))


    //verifie si enfilade
    if ( (lPiecesAttaquants.count() == 0) || aEchangeEstFavorable)
        lPoids += PoidsSiEnfilade (aPiece, aCurrentDemiMove, aCommandeJouee, aMapPiecesDefenseurs);

    return lPoids;

}


//ne calcule l'enfilade que pour les fous ou dame (mais avec echec sur Roi) dans meme direction d'attaque (sinon attaque double)
int EvaluatorPosition::PoidsSiEnfilade (const ArticlePiece& aPiece,
                                        int aCurrentDemiMove,
                                        const CommandPlay &aCommandeJouee,
                                        const QMap<int, tPieceEvalueeVector> &aMapPiecesDefenseurs) const {

    tPieceEvalueeVector lPiecesEnfileesDevant, lPiecesEnfileesDerriere;

    if ((aPiece._TypePiece != Bishop) && (aPiece._TypePiece != Queen)) return 0;

    int lPoids = 0;
    tQVectorInfosSquare lListSquares;
    bool lCalculAttDef = false;
    bool lRayonX       = true;

    if (_ChessManager.get_SquaresForMoves (aPiece,
                                           &aCommandeJouee,
                                           lListSquares,
                                           lCalculAttDef,
                                           lRayonX)) {

        float  lValeurPiece = CalculerValeurMaterielPiece(aPiece, aCurrentDemiMove);
        int  lNbreEnnemis = 0;
        bool lDirectionDevant = false;
        switch (aPiece._TypePiece) {

            case Bishop : { //va compter toutes les pieces attaquees de valeur > Fou
                            if (lListSquares.count() > 1) {
//a) si Fou met en echec Roi et attaque piece pas defendue : 1) si piece est reine, il faut que Fou soit protege
                                                          // 2) si piece est Fou, ne pas compter
//b) si Fou attaque 2 pieces
                                for (tQVectorInfosSquare::const_iterator itSquare = lListSquares.begin();
                                     itSquare != lListSquares.end();
                                     ++itSquare) {

                                    lDirectionDevant = (*itSquare)._PosX > aPiece._PosX;

                                    ArticlePiece lPiece;
                                    if (_ChessManager.UnePieceExiste((*itSquare)._PosX, (*itSquare)._PosY, lPiece)
                                            && (lPiece._Color != aPiece._Color)) {

                                        lNbreEnnemis++;
                                        float lValPieceEnnemy = CalculerValeurMaterielPiece(lPiece, aCurrentDemiMove);

                                        tlisteArticlesPieces lPiecesDef;
                                        if ((lPiece._TypePiece == King)
                                                    || (lValeurPiece < lValPieceEnnemy)
                                                    || ( (lPiece._TypePiece != Bishop) &&
                                                         !_ChessManager.ColorIsUnderAttackForPosition(aPiece._Color,
                                                                                     lPiece._PosX, lPiece._PosY,
                                                                                     lPiecesDef))) {

                                              if (lDirectionDevant)
                                                  lPiecesEnfileesDevant.push_back(sPieceEvaluee (lPiece, lValPieceEnnemy));
                                              else
                                                  lPiecesEnfileesDerriere.push_back(sPieceEvaluee (lPiece, lValPieceEnnemy));

                                         }
                                    }//if (_ChessManager.UnePieceExiste

                                }//for


                                //il faut analyser les pieces enfilees et leur ordre
                                //si Roi d'abord, OK
                                //Si Reine d'abord, il faut que Fou soit protege et on va considerer que si Roi derriere
                                //sinon Ok en condierant la valeur min (on considere que sera pas protege par l'autre...)
                                if (lPiecesEnfileesDevant.count() > 1)
                                    lPoids += CalculPoidsSurPiecesEnfilees (lPiecesEnfileesDevant, aPiece, lValeurPiece, aMapPiecesDefenseurs);
                                if (lPiecesEnfileesDerriere.count() > 1)
                                    lPoids += CalculPoidsSurPiecesEnfilees (lPiecesEnfileesDerriere, aPiece, lValeurPiece, aMapPiecesDefenseurs);

                            }//if (lListSquares.count() > 1)
                            break;
                          }
            case Queen  : {
                            break;
                          }
            default : return 0;
        }//switch

    }//if (_ChessManager.get_SquaresForMoves

    return lPoids;
}

int EvaluatorPosition::CalculPoidsSurPiecesEnfilees (const tPieceEvalueeVector& aPiecesEnfilees,
                                                     const ArticlePiece& aPiece,
                                                     const int aValeurPiece,
                                                     const QMap<int, tPieceEvalueeVector> &aMapPiecesDefenseurs) const {

    if (aPiecesEnfilees.count() < 2) return 0;

    if (aPiecesEnfilees [0]._Piece._TypePiece == Queen) {

        if (aPiecesEnfilees [1]._Piece._TypePiece != King) return 0;
        int lPos = _ChessManager.qHashKey(aPiece._PosX, aPiece._PosY);
        tPieceEvalueeVector lPiecesDefenseurs = aMapPiecesDefenseurs [lPos];
        if (lPiecesDefenseurs.count() < 1) return 0;
        return CPoidsMateriel*aPiecesEnfilees [0]._ValeurPiece;

    } else if (aPiecesEnfilees [0]._Piece._TypePiece == King) {

        //n'evalue prise que si pieces de valeur superieuyr car gain assure (sinon il faudrait evaluer possibilite de defense de la piece ennemie...)
        if (aPiecesEnfilees [1]._ValeurPiece > aValeurPiece)
            return aPiecesEnfilees [1]._ValeurPiece*CPoidsMateriel;
        else return 0;

    } else {
//pions todo
        if (aPiecesEnfilees [1]._Piece._TypePiece != King) {
            int lMinValeurEnnemiPris = qMin<int> (aPiecesEnfilees [0]._ValeurPiece,
                                                  aPiecesEnfilees [1]._ValeurPiece);
            if (lMinValeurEnnemiPris > aValeurPiece)
                return lMinValeurEnnemiPris*CPoidsMateriel;
            else return 0;
        }
        else {
            if (aPiecesEnfilees [0]._ValeurPiece > aValeurPiece)
                return CPoidsMateriel*aPiecesEnfilees [0]._ValeurPiece;
            else return 0;
        }
    }

    return 0;
}

int EvaluatorPosition::DistancePawnFromPromotion (const ArticlePiece& aPawn) const {

    return qAbs <int> (aPawn._PosY-_ChessManager.get_PosYKingBoard(_ChessManager.get_ColorEnnemy(aPawn._Color)));
}

bool EvaluatorPosition::PawnIsPassed (const ArticlePiece& aPawn, const tlisteArticlesPieces& alistePawnsEnnemies) const {

    if (aPawn._TypePiece != Pawn) return false;

    bool LPawnIsPassed = true;
    if (alistePawnsEnnemies.count() > 0) {
        for (tlisteArticlesPieces::const_iterator itPawnEnnemy = alistePawnsEnnemies.begin();
             itPawnEnnemy != alistePawnsEnnemies.end();
             ++itPawnEnnemy) {
            if (((*itPawnEnnemy)._PosX == (aPawn._PosX+1)) || ((*itPawnEnnemy)._PosX == (aPawn._PosX-1))) {
                if ( ((aPawn._Color == WHITE) && (aPawn._PosY < (*itPawnEnnemy)._PosY))
                     || ((aPawn._Color == BLACK) && (aPawn._PosY > (*itPawnEnnemy)._PosY))) {
                  LPawnIsPassed = false;
                  break;
                }
            }
        }//for llistePawnsEnnemies
    }//if (alistePawnsEnnemies.count() > 0)

    return LPawnIsPassed;
}

int EvaluatorPosition::getPoidsControleCentre (eColorPiece aColor,
                                           int aPosX, int aPosY,
                                               int aCurrentDemiMove,
                                           QMap<int, tPieceEvalueeVector> &aMapPosDefenseurs,
                                           QMap<int, tPieceEvalueeVector> &aMapPosAttaquants) const {

    int lBonusPourLoin = 0;
    if ((aColor == WHITE) && (aPosY > 4))
        lBonusPourLoin = CBonusControleLoin;
    else if ((aColor == BLACK) && (aPosY < 5))
        lBonusPourLoin = CBonusControleLoin;

    CalculerDefenseursAttaquantsDeCase (aColor, aPosX, aPosY, aCurrentDemiMove, aMapPosDefenseurs, aMapPosAttaquants);

    int lPos   = _ChessManager.qHashKey (aPosX, aPosY);
    int lPoids = aMapPosDefenseurs[lPos].count() - aMapPosAttaquants.count();

    //si on possede la case du centre bonus, sinon malus
    if (lPoids > 0) {
        lPoids  = CBonusControleCentre + lBonusPourLoin;
    }
    else {
        lPoids  = -CBonusControleCentre;
    }
    return lPoids;
}

void EvaluatorPosition::CalculerDefenseursAttaquantsDeCase (eColorPiece aColor,
                                                           int aPosX, int aPosY,
                                                            int aCurrentDemiMove,
                                                           QMap<int, tPieceEvalueeVector> &aMapPosDefenseurs,
                                                           QMap<int, tPieceEvalueeVector> &aMapPosAttaquants) const {

    tlisteArticlesPieces lPiecesDefenseurs, lPiecesAttaquants;
    tPieceEvalueeVector lDefenseursEvalues, lAttaquantsEvalues;

    int lPos = _ChessManager.qHashKey (aPosX, aPosY);

    if (!aMapPosDefenseurs.contains(lPos)) {
        lPiecesDefenseurs.clear();
        _ChessManager.ColorIsUnderAttackForPosition(_ChessManager.get_ColorEnnemy(aColor),
                                            aPosX, aPosY,
                                            lPiecesDefenseurs);
        for (tlisteArticlesPieces::const_iterator it = lPiecesDefenseurs.begin();
             it != lPiecesDefenseurs.end();
             ++it)
            lDefenseursEvalues.push_back(sPieceEvaluee (*it, this->CalculerValeurMaterielPiece((*it), aCurrentDemiMove)));
        aMapPosDefenseurs [lPos] = lDefenseursEvalues;

    }
    if (!aMapPosAttaquants.contains(lPos)) {
        lPiecesAttaquants.clear();
        _ChessManager.ColorIsUnderAttackForPosition(aColor,
                                            aPosX, aPosY,
                                            lPiecesAttaquants);
        for (tlisteArticlesPieces::const_iterator it = lPiecesAttaquants.begin();
             it != lPiecesAttaquants.end();
             ++it)
            lAttaquantsEvalues.push_back(sPieceEvaluee (*it, this->CalculerValeurMaterielPiece((*it), aCurrentDemiMove)));
        aMapPosAttaquants [lPos] = lAttaquantsEvalues;

    }
}

qreal EvaluatorPosition::CalculPoidsDistanceAuCentre (int anewPosX, int anewPosY) const {

    const qreal CPoidsProcheCentre = 5.0;
    return (CPoidsProcheCentre*(1 - (qSqrt (qPow ((1.*anewPosX - 4.5), 2)+ qPow ((1.*anewPosY - 4.5), 2))/qSqrt ( 2*qPow ((1.0-4.5), 2)))));
}

int EvaluatorPosition::PenalisationBadBishop (eColorPiece aColor, int aPosX, int aPosY) const {

    int lPenalisation = 0;

    int lPosX, lPosY;
    ArticlePiece lPiece;

    if (aColor == WHITE) {

        lPosX = aPosX;
        lPosY = aPosY;
        while ( (lPosX < 8) && (lPosY < 8)) {
            lPosX++;
            lPosY++;
            if (_ChessManager.UnePieceExiste(lPosX, lPosY, lPiece)
                    && (lPiece._Color == aColor)
                    && (lPiece._TypePiece == Pawn)) {
                lPenalisation += CPenaliseBadBishop;
            }
        }

        lPosX = aPosX;
        lPosY = aPosY;
        while ( (lPosX > 1) && (lPosY < 8)) {
            lPosX--;
            lPosY++;
            if (_ChessManager.UnePieceExiste(lPosX, lPosY, lPiece)
                    && (lPiece._Color == aColor)
                    && (lPiece._TypePiece == Pawn)) {
                lPenalisation += CPenaliseBadBishop;
            }
        }
    }//if (aPiece._Color == WHITE)
    else {

        lPosX = aPosX;
        lPosY = aPosY;
        while ( (lPosX > 1) && (lPosY > 1)) {
            lPosX--;
            lPosY--;
            if (_ChessManager.UnePieceExiste(lPosX, lPosY, lPiece)
                    && (lPiece._Color == aColor)
                    && (lPiece._TypePiece == Pawn)) {
                lPenalisation += CPenaliseBadBishop;
            }
        }

        lPosX = aPosX;
        lPosY = aPosY;
        while ( (lPosX < 8) && (lPosY > 1)) {
            lPosX++;
            lPosY--;
            if (_ChessManager.UnePieceExiste(lPosX, lPosY, lPiece)
                    && (lPiece._Color == aColor)
                    && (lPiece._TypePiece == Pawn)) {
                lPenalisation += CPenaliseBadBishop;
            }
        }

    }//else

    return lPenalisation;
}

float EvaluatorPosition::EstimationBasique (const tQMapValeursCriteres& aValeursCriteres) const {

    float lscore = 0;
    for (tQMapValeursCriteres::const_iterator it=aValeursCriteres.begin();
         it != aValeursCriteres.end();
         ++it) {
        lscore += it.value();
    }
    return lscore;//qMax<int>(0, lscore);
}

float EvaluatorPosition::EstimationNN (const tQMapValeursCriteres& aValeursCriteres,
                                       int acurrentDemiMove) const {

//    fann_type *calc_out;
//    fann_type input[aValeursCriteres.count()];
//    struct fann *ann = fann_create_from_file("TrainingNN.net");
//    int i =0;
//    for (tQMapValeursCriteres::const_iterator it=aValeursCriteres.begin();
//         it != aValeursCriteres.end();
//         ++it) {
//        input[i++] = it.value();
//    }
//    input[i] = acurrentDemiMove;

//    calc_out = fann_run(ann, input);
//    float loutput = calc_out [0];
//    fann_destroy(ann);
    float loutput = 0;
    return loutput;
}

bool EvaluatorPosition::CouleurPeutMenacerMatDuCouloir (eColorPiece aColor,
                                                       const CommandPlay& aLastCommand) const {

    ArticlePiece lPieceKingEnnemy;
    eColorPiece lColorEnnemy  = _ChessManager.get_ColorEnnemy(aColor);
    CommandPlay* lCommandPlay = 0;
    const CommandPlay* lLastCommandPlay =0;
    tQVectorInfosSquare lListSquares;
    QVector<CommandPlay*> lCommands;

    bool lMenaceMat = false;
    if (_ChessManager.getKing(lColorEnnemy, lPieceKingEnnemy))
    {
        //evalue possibilite mat du couloir
        if (lPieceKingEnnemy._PosY == _ChessManager.get_PosYKingBoard(lColorEnnemy)) {

            tlisteArticlesPieces lPiecesFriends, lQueens, lPiecesEnnemies;

            //liste les tours et dames qui peuvent donner echec au roi
            _ChessManager.findPieces(aColor, Rook, lPiecesFriends);
            if (_ChessManager.findPieces(aColor, Queen, lQueens))
                lPiecesFriends += lQueens;

               for (int lPosX = 1; lPosX <= 8; lPosX++) {

                  lMenaceMat= false;
                  lCommands.clear();
                  lLastCommandPlay = &aLastCommand;

                  for (int i = 0; i < lPiecesFriends.count(); i++) {

                    if ((lPiecesFriends [i]._TypePiece == Rook) && (lPiecesFriends [i]._PosX != lPosX)) continue;

                    if ( (lPosX != lPieceKingEnnemy._PosX)
                         && _ChessManager.Play(lPiecesFriends[i]._PosX, lPiecesFriends[i]._PosY,
                                               lPosX, lPieceKingEnnemy._PosY,
                                               lCommandPlay, lLastCommandPlay)
                         && lCommandPlay){

                        lCommands.push_back(lCommandPlay);
                        lLastCommandPlay = lCommandPlay;

                        //si roi ennemi en echec et pas de moves possibles
                        if (lCommandPlay->KingEnnemyIsChecked()
                            && !_ChessManager.get_SquaresForMoves (lPieceKingEnnemy,
                                                                lCommandPlay,
                                                                lListSquares,
                                                                false)) {
                            //chercher pieces ennemies pouvant soit attaquer la piece, soit se mettre devant en etant protegee
                            lPiecesEnnemies.clear();
                            CommandPlay* lCommandPlayEnnemy = 0;
                            if (_ChessManager.ColorIsUnderAttackForPosition(aColor, lPosX, lPieceKingEnnemy._PosY, lPiecesEnnemies)
                                    && _ChessManager.Play(lPiecesEnnemies[0]._PosX, lPiecesEnnemies[0]._PosY,
                                                          lPosX, lPieceKingEnnemy._PosY,
                                                          lCommandPlayEnnemy, lLastCommandPlay)){
                                lCommands.push_back(lCommandPlayEnnemy);
                                lLastCommandPlay = lCommandPlayEnnemy;
                            }
                            else {
                                lMenaceMat = true;
                                for (int lPos=1; lPos <= 8; lPos++) {

                                    //enleve les positions X sur lesquelles il est intuile de chercher
                                    if (lPos == lPosX) continue;//deja teste
                                    if ( (lPosX < lPieceKingEnnemy._PosX)
                                           && ((lPos < lPosX) || (lPos >= lPieceKingEnnemy._PosX))) continue;
                                    if ( (lPosX > lPieceKingEnnemy._PosX)
                                           && ((lPos > lPosX) || (lPos <= lPieceKingEnnemy._PosX))) continue;

                                    if (_ChessManager.ColorIsUnderAttackForPosition(aColor, lPos, lPieceKingEnnemy._PosY, lPiecesEnnemies)
                                        && (lPiecesEnnemies.count() > 1)
                                        && _ChessManager.Play(lPiecesEnnemies[0]._PosX, lPiecesEnnemies[0]._PosY,
                                                               lPosX, lPieceKingEnnemy._PosY,
                                                               lCommandPlayEnnemy, lLastCommandPlay)) {
                                        lCommands.push_back(lCommandPlayEnnemy);
                                        lLastCommandPlay = lCommandPlayEnnemy;
                                        lMenaceMat = false;
                                        break;
                                    }
                                }//for (int lPos=1; lPos <= 8; lPos++)
                            }//else
                        }//if (lCommandPlay->KingEnnemyIsChecked()...
                    }//if ( (lPosX != lPieceKingEnnemy._PosX)...

                    if (lMenaceMat) break;
                }//for (int i = 0; i < lPiecesFriends.count(); i++)

                //fait un rollback des moves
                QVector<CommandPlay*>::const_iterator it = lCommands.end();
                while (it != lCommands.begin()) {
                    --it;
                    if (*it) {
                        (*it)->UnExecute ();
                        delete *it;
                    }
                }

                if (lMenaceMat) break;
            }//for (int lPosX = 1; lPosX <= 8; lPosX++)

        }//if (lPieceKingEnnemy._PosY == _ChessManager.get_PosYKingBoard(lColorEnnemy))

    }//if (_ChessManager.getKing(lColorEnnemy, lPieceKingEnnemy))

    return lMenaceMat;
}



