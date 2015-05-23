#ifndef ALGOCOMPUTER_H
#define ALGOCOMPUTER_H

#include "TypePiece.hpp"
#include "Data/ArticlePiece.hpp"
#include "Types/TypeInfosMoves.h"
#include "Types/TypeInfosGame.h"
#include "evaluatorposition.h"

class ChessManager;
class CommandPlay;
class IObserverAlgo;

class AlgoComputer
{
public:
    AlgoComputer(ChessManager& aChessmanager, eColorPiece aColorComputer);
    eResultAlgo FindMoveToPlay(int acurrentDemiMove,
                         const CommandPlay *const aPreviousCommandPlay,
                         sMove &aMoveSelected);

    eColorPiece get_ColorComputer() const {return _ColorComputer;}
    void setColorComputer (eColorPiece aColorComputer) {_ColorComputer = aColorComputer;}
    void RegisterObserver (IObserverAlgo* aObserverAlgo)  {if (aObserverAlgo) {_ListObservers.push_back(aObserverAlgo);}}
    void NotifierBeginComputation () const;
    void NotifierEndComputation () const;

    void setAlgoLose ();
    void setAlgoWins ();
    void setEqualityResult ();

    eResultGame get_resultGame() const {return _resultGame;}

    void SetStopComputation (bool aStop) {_StopComputation = aStop;}

private:
    ChessManager&  _ChessManager;
    eColorPiece _ColorComputer;
    int _Depth;
    EvaluatorPosition _EvaluatorPosition;
    QVector<IObserverAlgo*> _ListObservers;
    bool _StopComputation;
    eResultGame _resultGame;

    /**
     * @brief Algorithme de recherche du meilleur coup
     *
     * @param acurrentDemiMove
     * @param aDepth
     * @param aAlphaPruning
     * @param aBetaPruning
     * @param aPreviousCommandPlay
     * @param aMoveSelected
     * @return int
     */
    int NegaMax (int acurrentDemiMove,
                 int aDepth,
                 float aAlphaPruning, float aBetaPruning,
                 const CommandPlay *const aPreviousCommandPlay,
                 sMove &aMoveSelected,
                 QVector<sMove>& aMovesEquivalent);
    bool CanPrune (float aMoveScore,
                   sMove aMove,
                   float & abestmove_score,
                   float & aAlphaPruning,
                   float & aBetaPruning,
                   sMove & aMoveSelected,
                   QVector<sMove> &aMovesEquivalent);
    float getTheWorstScore (int aDepth) const;
    float TryMoveEnProfondeur (const sMove& aMove,
                               int acurrentDemiMove,
                               int aDepth,
                               float aAlphaPruning,
                               float aBetaPruning,
                               const CommandPlay* const aPreviousCommandPlay);
    void setResultGame (eResultGame aresultGame);
    void NotifierAlgoIsCheckMate () const;
    void NotifierAlgoIsPat () const;
    void NotifierAlgoIsVictorious () const;
    bool ConditionArret (int aDepth, float aBestMoveScore) const;

};

#endif // ALGOCOMPUTER_H
