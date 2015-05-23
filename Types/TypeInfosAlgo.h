#ifndef TYPEINFOSALGO_H
#define TYPEINFOSALGO_H

#include "TypePiece.hpp"


struct sMove {
    eTypePiece _TypePiece;
    int _OldPosX, _OldPosY, _newPosX, _newPosY;
    float _Poids;//poids associé au mouvement de la piece
    bool _GivesCheck;//info supp qui dit si le coup donne échec au roi ennemi
    sMove (eTypePiece aTypePiece,
           int aOldPosX, int aOldPosY,
           int anewPosX, int anewPosY,
           float aPoids,
           bool aGivesCheck) :
        _TypePiece(aTypePiece),
        _OldPosX(aOldPosX), _OldPosY(aOldPosY),
        _newPosX(anewPosX), _newPosY(anewPosY),
        _Poids (aPoids),
        _GivesCheck(aGivesCheck)
    {}
    sMove (const sMove &aMove) :
        _TypePiece(aMove._TypePiece),
        _OldPosX(aMove._OldPosX), _OldPosY(aMove._OldPosY),
        _newPosX(aMove._newPosX), _newPosY(aMove._newPosY),
        _Poids (aMove._Poids),
        _GivesCheck (aMove._GivesCheck)
    {}
    sMove () :
        _TypePiece(Pawn),
        _OldPosX(0), _OldPosY(0),
        _newPosX(0), _newPosY(0),
        _Poids (-1.0),
        _GivesCheck (false)
    {}
};
struct sMoveComparer {
    bool operator() (const sMove& aMove1, const sMove&  aMove2) {
        return (aMove1._Poids > aMove2._Poids);
    }
};

typedef QVector<sMove> tsMoveVector;

enum eResultAlgo {
    eComputerHasNoMove,
    eCONTINUE,
    eEnnemyHasNoMove
};



#endif // TYPEINFOSALGO_H
