#ifndef OBSERVERCHESSMANAGER_H
#define OBSERVERCHESSMANAGER_H

#include "TypePiece.hpp"

class CommandPlay;

class ObserverChessManager {
public:
    virtual void DoOnAjouterPiece(eTypePiece aTypePiece, eColorPiece aColor, int aPosX, int aPosY) = 0;
    virtual void DoOnResetPieces () = 0;
    virtual void DoOnModificationPlacePiece (eTypePiece aTypePiece, eColorPiece aColor,
                                             int aOldPosX, int aOldPosY,
                                             int anewPosX, int anewPosY) =0;
    virtual void DoOnKillPiece (eTypePiece aGenrePiece, eColorPiece aColorPiece,
                                int aPosX, int aPosY) = 0;
    virtual void DoOnPlay (CommandPlay* aCommandPlay) =0;
};

#endif // OBSERVERCHESSMANAGER_H
