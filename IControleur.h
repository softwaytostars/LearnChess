#ifndef ICONTROLEUR_H
#define ICONTROLEUR_H

#include "TypePiece.hpp"

class IObserverAlgo;

class IControleur {
public:
    virtual void setModePlayAgainstComputer (IObserverAlgo& aObserverAlgo,
                                             eColorPiece aColorComputer,
                                             bool aModeComputerAgainstComputer=false)=0;
    virtual bool StopAllComputations ()=0;
    virtual void AllowPlayFirstBlack (bool aAllow)=0;
    virtual void AllowShowAnySquareForMovePiece (bool aAllow)=0;
    virtual void AllowUserInteraction (bool aAllow)=0;
};

#endif // ICONTROLEUR_H
