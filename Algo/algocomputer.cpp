#include "algocomputer.h"
#include <QtAlgorithms>
#include <QtCore/qmath.h>
#include <QMap>
#include <QtGlobal>
#include <QTime>
#include <QFile>

#include "ChessBoardManager/chessmanager.h"
#include "ChessBoardManager/commandplay.h"
#include "Types/TypeInfosAlgo.h"
#include "Algo/IObserverAlgo.h"

#ifdef _DEBUG
#include <QDebug>
#endif

//#include "Algo/creatordatann.h"

const float CPasDeCoupsPossibles = -900000.;//echec et mat
const int   CNumerateurMargeErrEquivMove = 0;

AlgoComputer::AlgoComputer(ChessManager& aChessmanager, eColorPiece aColorComputer):
    _ChessManager (aChessmanager),
    _ColorComputer (aColorComputer),
    _Depth(2),//mettre un nbre pair pour finir par le meilleur coup algo
    _EvaluatorPosition  (EvaluatorPosition(_ChessManager) ),
    _StopComputation(false),
    _resultGame (eNoResult)
{
    //Pour faire le training NN
//    CreatorDataNN lCreator;
//    lCreator.CreateTrainingFile();
    _EvaluatorPosition.setUseNN(QFile::exists("TrainingNN.net"));
}

eResultAlgo AlgoComputer::FindMoveToPlay (int acurrentDemiMove,
                                   const CommandPlay *const aPreviousCommandPlay,
                                   sMove &aMoveSelected) {

    QVector<sMove> lMovesEquivalent;

//    if (ChoiceMoveInOpeningsLibrary (acurrentDemiMove, aPreviousCommandPlay, aMoveSelected))
//        return true;
//    else MoveCanBeComputed (acurrentDemiMove, aPreviousCommandPlay, aMoveSelected))

//    if (_UseOpening && (lOpenings.count() > 0) && (acurrentDemiMove <= 12)) {

//        QVector<Opening*> lOpenings;

//        if (!aPreviousCommandPlay) {
//            _UseOpening   = lOpenings [(lOpenings.count()-1)*qrand ()/RAND_MAX]->getMove (acurrentDemiMove,
//                                                                                          aMoveSelected);
//        }else if (aPreviousCommandPlay->GetInfosPieceDeplacee ()) {
//            _UseOpening = false;
//            lMovesEquivalent.clear();
//            for (QVector<Opening*>::const_iterator itOpening = lOpenings.begin();
//                itOpening != lOpenings.end();
//                ++itOpening) {
//                if ((*itOpening)
//                        && ((*itOpening)->getMove (acurrentDemiMove-1, lMoveSelected)
//                        && (lMoveSelected == lPreviousMove)
//                        && ((*itOpening)->getMove (acurrentDemiMove, lMoveSelected)))) {
//                    lMovesEquivalent.push_back(lMoveSelected);
//                }

//            }
//            _UseOpening = lMovesEquivalent.count() > 0;
//            if (_UseOpening) {
//                qsrand (QTime::currentTime().second());
//                aMoveSelected = lMovesEquivalent [(lMovesEquivalent.count()-1)*qrand ()/RAND_MAX];
//            }

//        }
//    }


    //desactiver Notifications
    // etpermet eviter verif tour joueur
    _ChessManager.setModePermissif(true);
    _ChessManager.DesActiverNotification (true);

    //recherche du meilleur coup
    float lValeur = NegaMax (acurrentDemiMove,
                             _Depth,
                             CPasDeCoupsPossibles,
                            -CPasDeCoupsPossibles,
                            aPreviousCommandPlay,
                            aMoveSelected,
                            lMovesEquivalent);

    _ChessManager.setModePermissif(false);
    _ChessManager.DesActiverNotification (false);

    QVector<sMove> lTrueMovesEquivalent;
    for (int i=0; i<lMovesEquivalent.count(); i++) {
        if ((aMoveSelected._Poids - lMovesEquivalent [i]._Poids) <= (CNumerateurMargeErrEquivMove/100)*aMoveSelected._Poids)//laisse une marge d'erreur de 1% pour permettre choisir coup different
          lTrueMovesEquivalent.push_back(lMovesEquivalent [i]);
    }

    if (lTrueMovesEquivalent.count() > 1) {
        //permet de choisir aleatoirement une reponse equivalente
        qsrand (QTime::currentTime().second());
        int lIndex = qrand () % lTrueMovesEquivalent.count();
        aMoveSelected = lTrueMovesEquivalent [qMin<int>((lTrueMovesEquivalent.count()-1),
                                                        lIndex)];
    }

    if (lValeur == CPasDeCoupsPossibles) {
        return eComputerHasNoMove;
    }
    if (lValeur == -getTheWorstScore (_Depth - 1))//si prochain coup est calculé impossible
        return eEnnemyHasNoMove;

    return eCONTINUE;

}


void AlgoComputer::NotifierBeginComputation () const {

    for (QVector<IObserverAlgo*>::const_iterator it =_ListObservers.begin();
         it != _ListObservers.end();
         ++it) {
        if (*it)
            (*it)->DoOnStartPlay ();
    }
}

void AlgoComputer::NotifierEndComputation () const {

    for (QVector<IObserverAlgo*>::const_iterator it =_ListObservers.begin();
         it != _ListObservers.end();
         ++it) {
        if (*it)
            (*it)->DoOnEndPlay ();
    }
}

void AlgoComputer::NotifierAlgoIsCheckMate () const {

    for (QVector<IObserverAlgo*>::const_iterator it =_ListObservers.begin();
         it != _ListObservers.end();
         ++it) {
        if (*it)
            (*it)->DoOnCheckMate ();
    }
}

void AlgoComputer::NotifierAlgoIsPat () const {

    for (QVector<IObserverAlgo*>::const_iterator it =_ListObservers.begin();
         it != _ListObservers.end();
         ++it) {
        if (*it)
            (*it)->DoOnPat ();
    }
}

void AlgoComputer::NotifierAlgoIsVictorious () const {

    for (QVector<IObserverAlgo*>::const_iterator it =_ListObservers.begin();
         it != _ListObservers.end();
         ++it) {
        if (*it)
            (*it)->DoOnAlgoVictorious ();
    }
}

float AlgoComputer::getTheWorstScore (int aDepth) const {

    return (CPasDeCoupsPossibles + qMax <int> ((_Depth - aDepth), 0));
}

int AlgoComputer::NegaMax (int acurrentDemiMove, int aDepth, float aAlphaPruning, float aBetaPruning,
                           const CommandPlay* const aPreviousCommandPlay,
                           sMove &aMoveSelected,
                           QVector<sMove> &aMovesEquivalent) {

   try {
        float lbestmove_score = getTheWorstScore (aDepth);

        //pour eviter evaluation si demande arret et que niveau superieur
        if (ConditionArret(aDepth, lbestmove_score))
            return lbestmove_score;

        if (acurrentDemiMove < 4)
           aDepth = 0;

        aMovesEquivalent.clear();
         QVector<sMove> lListeMoves;
         tlisteArticlesPieces lListePieces;
         ArticlePiece lPiece;
         tQVectorInfosSquare lListSquares;
         float lPoidsPieceSurCase = 0;
         eColorPiece lColorInConsideration = _ColorComputer;
         if (aPreviousCommandPlay) {
             lColorInConsideration = _ChessManager.get_ColorEnnemy (aPreviousCommandPlay->getColor());
         }

         CommandPlay *lCommandPlay = 0;
         /****liste tous les coups possibles pour toutes les pieces en leur donnant une pondération (pour les trier par ordre de préférence afin que le pruning soit plus efficace)****/
         if (_ChessManager.getArticlesPiecesDeCouleur(lColorInConsideration, lListePieces)) {

             for (tlisteArticlesPieces::const_iterator it = lListePieces.begin();
                  it!= lListePieces.end();
                  ++it) {
                 //cherche les moves possibles pour la piece
                 lPiece = *it;
                 //au debut de partie, ne considère pas les tours ni les pions b et g pour accélerer calcul
                 if ( (acurrentDemiMove <= 10) && (aDepth > 0)) {
                    if (lPiece._TypePiece == Rook) continue;
                    if (lPiece._TypePiece == Pawn) {
                        if ( (lPiece._PosX == 2) || (lPiece._PosX == 7)) continue;
                    }
                 }
                 if (_ChessManager.get_SquaresForMoves (lPiece,
                                         aPreviousCommandPlay,
                                         lListSquares,
                                         false)) {

                     for (tQVectorInfosSquare::const_iterator itInfoSquare = lListSquares.begin();
                          itInfoSquare != lListSquares.end();
                          ++itInfoSquare) {


                         if (_ChessManager.Play(lPiece._PosX, lPiece._PosY,
                                                (*itInfoSquare)._PosX, (*itInfoSquare)._PosY,
                                                lCommandPlay, aPreviousCommandPlay) && lCommandPlay) {

//#ifdef _DEBUG
//    if (lColorInConsideration == WHITE)
//        qDebug() <<"WHITE";
//    else
//        qDebug()<<"BLACK";
//    qDebug()<<" aDepth = "<<aDepth;
//    qDebug()<<"MOVE ETUDIE "<<lPiece._TypePiece<<" "<<_ChessManager.GetNomXPos(lPiece._PosX)<<lPiece._PosY<< " TO ";
//    qDebug()<<_ChessManager.GetNomXPos((*itInfoSquare)._PosX)<< (*itInfoSquare)._PosY;
//#endif

                                 lPoidsPieceSurCase = _EvaluatorPosition.EstimationPosition(acurrentDemiMove+1,
                                                                                            lColorInConsideration,
                                                                                            *lCommandPlay);
//#ifdef _DEBUG
//    qDebug()<<"LMOVE_SCORE="<<lPoidsPieceSurCase;
//#endif

                                 sMove lMove = sMove (lPiece._TypePiece,
                                                      lPiece._PosX, lPiece._PosY,
                                                      (*itInfoSquare)._PosX, (*itInfoSquare)._PosY,
                                                      lPoidsPieceSurCase,
                                                      lCommandPlay->KingEnnemyIsChecked());
                                 lListeMoves.push_back(lMove);

                                 lCommandPlay->UnExecute();
                                 delete lCommandPlay;
                                 lCommandPlay = 0;

                                 //mis ici pour accelerer calcul (pas de tri ni re-parcours)
                                 if (aDepth == 0) {

                                     if (CanPrune (lPoidsPieceSurCase,
                                                   lMove,
                                                   lbestmove_score,
                                                   aAlphaPruning,
                                                   aBetaPruning,
                                                   aMoveSelected,
                                                   aMovesEquivalent))
                                     {
                                        return aAlphaPruning;
                                     }


                                 }//if (aDepth == 0)

                                 //arrete le calcul que pour niveaux suivants sinon risque message de mat ou stalemate!
                                 if (ConditionArret(aDepth, lbestmove_score))
                                     return lbestmove_score;


                             }//if play

                     }//for lListSquares

                 }//if get square moves
             }//for (tlisteArticlesPieces::const_iterator
         }//if (_ChessManager.getArticlesPiecesDeCouleur


         //pas besoin du tri ni de reparcourir et de ressayer les coups si adepth=0
         if (aDepth == 0) return lbestmove_score;


         /*****si on cherche la reponse ennemie, on va trier les coups pour plus d'efficacite****/
         /****tri les coups par leur poids****/
         qSort (lListeMoves.begin(), lListeMoves.end(), sMoveComparer());


         int lnbreMovesTestes = 0;
         const float CEcartAuMaxAccepte = 0.1;

         /****parcourt les move possibles et les essaie****/
         for (QVector<sMove>::const_iterator it = lListeMoves.begin();
              it != lListeMoves.end();
              ++it) {

             sMove lMove = *it;


             //ne pas considerer les coups trop faibles
             if (lbestmove_score > getTheWorstScore(aDepth)) {

                //ne pas considerer les coups trop faibles
                if ((lMove._Poids < 0) && (lListeMoves[0]._Poids > 70))
                     break;

                if ((lnbreMovesTestes >= 4)
                     && (qAbs<float> (lListeMoves[0]._Poids-lMove._Poids) > qAbs<int> (CEcartAuMaxAccepte*lListeMoves[0]._Poids)))
                    break;


                 //si profondeur demandee superieur à 2, alors ne teste que les 1 meilleurs premiers coups
//                 if (((_Depth - aDepth) > 2) && (lnbreMovesTestes > 0))
//                     break;
             }


             lnbreMovesTestes++;

//             //debug
//             if (aDepth==2) {
//                 if ((lnbreMovesTestes !=1)
//                         &&  (lnbreMovesTestes !=6)) continue;
//             }
//             if (aDepth==1) {
//                 if (lnbreMovesTestes > 1) continue;
//             }

             float lmove_score = TryMoveEnProfondeur (lMove,
                                                      acurrentDemiMove,
                                                      aDepth,
                                                      aAlphaPruning,
                                                      aBetaPruning,
                                                      aPreviousCommandPlay);
             if (CanPrune (lmove_score,
                           lMove,
                           lbestmove_score,
                           aAlphaPruning,
                           aBetaPruning,
                           aMoveSelected,
                           aMovesEquivalent))
             {
                return aAlphaPruning;
             }


             //si on a trouvé un coup qui fait mat l'adversaire, plus besoin de chercher + loin...
             if (lmove_score >= -getTheWorstScore(aDepth))
                 return lmove_score;

             if (ConditionArret(aDepth, lbestmove_score))
                 return lbestmove_score;

         }//for lMove

         //si apres notre selection de move, on a trouvé aucun coup qui nous evite d'etre mat tout de suite alors on regarde les coups suivants
         if ( (lbestmove_score == getTheWorstScore(aDepth)) && (lnbreMovesTestes < lListeMoves.count())) {

             for (int i= lnbreMovesTestes; i < lListeMoves.count();i++) {

                 float lmove_score = TryMoveEnProfondeur (lListeMoves [i],
                                                          acurrentDemiMove,
                                                          aDepth,
                                                          aAlphaPruning,
                                                          aBetaPruning,
                                                          aPreviousCommandPlay);
                 if (CanPrune (lmove_score,
                               lListeMoves [i],
                               lbestmove_score,
                               aAlphaPruning,
                               aBetaPruning,
                               aMoveSelected,
                               aMovesEquivalent))
                 {
                    return aAlphaPruning;
                 }


                 //on s'arrete des qu'on a trouvé un coup qui evite le mat
                 if (lmove_score > getTheWorstScore(aDepth))
                     return lmove_score;

                 if (ConditionArret(aDepth, lbestmove_score))
                     return lbestmove_score;

             }//for

         }//if ( (lmove_score == getTheWorstScore(aDepth)) && (lnbreMovesTestes < lListeMoves.count()))

         return lbestmove_score;
    }//try
    catch (...) {
        return getTheWorstScore(aDepth);
    }
}


float AlgoComputer::TryMoveEnProfondeur (const sMove& aMove,
                                         int acurrentDemiMove,
                                         int aDepth,
                                         float aAlphaPruning,
                                         float aBetaPruning,
                                         const CommandPlay* const aPreviousCommandPlay) {

    CommandPlay *lCommandPlay = 0;
    float lmove_score = getTheWorstScore(aDepth);

    sMove lMoveSelected;
    QVector<sMove> lMovesEquivalent;

    //do the move
    if (_ChessManager.Play(aMove._OldPosX, aMove._OldPosY,
                           aMove._newPosX, aMove._newPosY,
                           lCommandPlay, aPreviousCommandPlay) && lCommandPlay) {


//#ifdef _DEBUG
//    qDebug()<<" aDepth = "<<aDepth;
//    qDebug()<<"MOVE EFFECTUE "<<" "<<_ChessManager.GetNomXPos(aMove._OldPosX)<<aMove._OldPosY<< " TO ";
//    qDebug()<<_ChessManager.GetNomXPos(aMove._newPosX)<< aMove._newPosY;
//    qDebug()<<"POIDS="<<aMove._Poids;
//#endif


        int lAjoutProdondeur = 0;
//                 if ((aDepth == 0) && lCommandPlay->KingEnnemyIsChecked())
//                   lAjoutProdondeur = 0;//va plus en profondeur (2demi coups)pour pas louper de coup tactique

        acurrentDemiMove++;
        if ((aDepth+lAjoutProdondeur) == 0) {
            lmove_score = aMove._Poids;

        }
        else
            lmove_score = -NegaMax (acurrentDemiMove,
                                    lAjoutProdondeur+aDepth-1,
                                    -aBetaPruning,
                                    -aAlphaPruning,
                                    lCommandPlay,
                                    lMoveSelected,
                                    lMovesEquivalent);

//#ifdef _DEBUG
//    qDebug()<<" aDepth = "<<aDepth;
//    qDebug()<<"COUPS CHOISI ENNEMY"<<" "<<_ChessManager.GetNomXPos(lMoveSelected._OldPosX)<<lMoveSelected._OldPosY<< " TO ";
//    qDebug()<<_ChessManager.GetNomXPos(lMoveSelected._newPosX)<< lMoveSelected._newPosY;
//    qDebug()<<"POIDS="<<lMoveSelected._Poids;
//#endif




        //undo the move
        lCommandPlay->UnExecute();
        delete lCommandPlay;
        lCommandPlay = 0;
        acurrentDemiMove--;

    }

    return lmove_score;

}

bool AlgoComputer::CanPrune (float aMoveScore,
                             sMove aMove,
                             float & abestmove_score,
                             float & aAlphaPruning,
                             float & aBetaPruning,
                             sMove & aMoveSelected,
                             QVector<sMove> &aMovesEquivalent) {


    aMove._Poids = aMoveScore;/* car un mouvement de poids plus faible peut avoir un meilleur score. Ceci est une astuce pour choix final*/

    if (aMoveScore > abestmove_score) {
        abestmove_score = aMoveScore;
        aMoveSelected   = aMove;
    }


    //if (aMoveScore == abestmove_score)
    if (qAbs<float> (aMoveScore- abestmove_score) <= (CNumerateurMargeErrEquivMove/100)*abestmove_score)
        aMovesEquivalent.push_back(aMove);

    //alpha-beta pruning pour ne pas chercher les autres moves dès qu'il y en a un qui diminuerait notre resultat
    if (aMoveScore >= aAlphaPruning)
        aAlphaPruning = aMoveScore;

    return (aAlphaPruning >= aBetaPruning);
}


void AlgoComputer::setAlgoLose () {

    if (_ColorComputer == WHITE)
        setResultGame (eBlackWins);
    else
        setResultGame (eWhiteWins);
    NotifierAlgoIsCheckMate();
}

void AlgoComputer::setAlgoWins () {

    if (_ColorComputer == WHITE)
        setResultGame (eWhiteWins);
    else
        setResultGame (eBlackWins);
    NotifierAlgoIsVictorious();

}

void AlgoComputer::setEqualityResult () {

    setResultGame (eNulle);
    NotifierAlgoIsPat();
}

void AlgoComputer::setResultGame(eResultGame aresultGame) {

    _resultGame = aresultGame;
    NotifierEndComputation();

}

//arrte seulement pour les niveaux superieurs sauf si on a trouve un score evitant message mat ou stalemate!
bool AlgoComputer::ConditionArret (int aDepth, float aBestMoveScore) const {

    return (_StopComputation && (((_Depth-aDepth)>=1) || (aBestMoveScore != getTheWorstScore(aDepth))));
}








