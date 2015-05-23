#ifndef IOBSERVERALGO_H
#define IOBSERVERALGO_H

class IObserverAlgo {
public:
    virtual void DoOnStartPlay()=0;
    virtual void DoOnEndPlay()=0;
    virtual void DoOnCheckMate()=0;
    virtual void DoOnPat()=0;
    virtual void DoOnAlgoVictorious ()=0;
};

#endif // IOBSERVERALGO_H
