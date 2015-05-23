#include "ThreadAlgo.h"
#include "algocomputer.h"

ThreadAlgo::ThreadAlgo(AlgoComputer& aAlgoComputer,
                       int aCurrentDemiMove,
                       const CommandPlay* const aPreviousCommandPlay): QThread(),
    _AlgoComputer(aAlgoComputer),
    _CurrentDemiMove(aCurrentDemiMove),
    _PreviousCommandPlay(aPreviousCommandPlay)
{
    //pour s'autodetruire
 //   QObject::connect (this, SIGNAL (finished()), this, SLOT(deleteLater()));
}

ThreadAlgo::~ThreadAlgo () {
}


void ThreadAlgo::run() {

    sMove lMoveSelected;
    emit StartComputation();

    eResultAlgo lresultAlgo = _AlgoComputer.FindMoveToPlay(_CurrentDemiMove, _PreviousCommandPlay, lMoveSelected);
    switch (lresultAlgo) {
        case eCONTINUE : {emit GiveMoveToPlay (lMoveSelected._OldPosX, lMoveSelected._OldPosY,
                                              lMoveSelected._newPosX, lMoveSelected._newPosY);
                          break;
                          }
        case eComputerHasNoMove   : {emit ComputerHasNoMove () ; break;}
        case eEnnemyHasNoMove     : {emit EnnemyHasNoMoveAfter(lMoveSelected._OldPosX, lMoveSelected._OldPosY,
                                                              lMoveSelected._newPosX, lMoveSelected._newPosY,
                                                              lMoveSelected._GivesCheck);break;}
        default        : break;
    }

    _AlgoComputer.SetStopComputation(false);

}

bool ThreadAlgo::StopComputation() {

    this->sleep(5);
    _AlgoComputer.SetStopComputation(true);
    //attend au maximum 1 minute
    return this->wait(60000);//attend la fin de l'execution
}
