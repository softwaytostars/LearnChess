#ifndef TYPEDEMO_HPP
#define TYPEDEMO_HPP

#include <vector>
#include "TypePiece.hpp"

//contient info pour demo où se trouve la pièce au temps relatif
struct sMoveDemo {
    float _tempsRelatif;
    int _posX;
    int _posY;
    sMoveDemo () : _tempsRelatif(0.0), _posX(0), _posY(0) {}
    sMoveDemo (const sMoveDemo& aCopy) : _tempsRelatif(aCopy._tempsRelatif),
        _posX(aCopy._posX),
        _posY(aCopy._posY)
    {
    }
};

enum eTypeInitiation {
    eInitierPiecesMoves,
    eInitierSpecialMoves,
    eInitierPatternsMat,
    eInitierTactiques
};

enum eTypeDemoSpecialMove {
    ePetitRoqueMove,
    eGrandRoqueMove,
    ePriseEnPassantMove,
    ePromotionMove
};

enum eTypeDemoPatternMat {
    ePatternMatCouloir,
    ePatternMatLolli,
    ePatternMatEtouffe,
    ePatternMatEpaulette,
    ePatternMatArabes,
    ePatternMatGreco,
    ePatternMatBoden,
    ePatternMatAnastasie,
    ePatternMatLegal
};

enum eTypeDemoTactics {
    eTacticDoubleAttack,
    eTacticDiscoveredAttack,
    eTacticPinAndSkewer,
    eTacticRemovingGuard,
    eTacticOverLoading,
    eTacticDeflection
};



typedef std::vector<sMoveDemo> tListMovesdemo;

enum eTypeDemo {
    eSimuMove,
    eTrainingMove
};

#endif // TYPEDEMO_HPP
