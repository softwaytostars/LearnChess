#ifndef THREADALGO_H
#define THREADALGO_H

#include <QThread>

class AlgoComputer;
class CommandPlay;

class ThreadAlgo : public QThread
{
    Q_OBJECT
public:
    ThreadAlgo(AlgoComputer &aAlgoComputer,
               int aCurrentDemiMove,
               const CommandPlay* const aPreviousCommandPlay);
    ~ThreadAlgo();
    bool StopComputation();
signals:
    void StartComputation ();
    void GiveMoveToPlay (int aOldPosX, int aOldPosY, int aNewPosX, int aNewPosY);
    void ComputerHasNoMove ();
    void EnnemyHasNoMoveAfter (int aOldPosX, int aOldPosY, int aNewPosX, int aNewPosY, bool aGivesCheck);

protected:
    virtual void run();
private:
    AlgoComputer& _AlgoComputer;
    int _CurrentDemiMove;
    const CommandPlay*  _PreviousCommandPlay;
};

#endif // THREADALGO_H
