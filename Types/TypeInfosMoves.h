#ifndef TYPEINFOSMOVES_H
#define TYPEINFOSMOVES_H

#include <QVector>
#include "Data/ArticlePiece.hpp"

struct sInfosSquare {
    int _PosX, _PosY;
    tlisteArticlesPieces _listePiecesDefenseurs, _listePiecesAttaquants;
    sInfosSquare (int aPosX, int aPosY,
                  const tlisteArticlesPieces& alistePiecesDefenseurs,
                  const tlisteArticlesPieces& alistePiecesAttaquants):
        _PosX (aPosX), _PosY (aPosY),
        _listePiecesDefenseurs (alistePiecesDefenseurs),
        _listePiecesAttaquants (alistePiecesAttaquants)
    {}
    sInfosSquare(){}
};


struct sInfosCastling {
    int _PosKingToCastle, _PosKingDest, _PosRookToCastle, _PosRookDest;
    sInfosCastling (int aPosKingToCastle, int aPosKingDest, int aPosRookToCastle, int aPosRookDest):
        _PosKingToCastle(aPosKingToCastle), _PosKingDest(aPosKingDest),
        _PosRookToCastle(aPosRookToCastle), _PosRookDest (aPosRookDest)
    {
    }

    sInfosCastling (const sInfosCastling& aInfos) {
        _PosKingToCastle = aInfos._PosKingToCastle;
        _PosKingDest     = aInfos._PosKingDest;
        _PosRookToCastle = aInfos._PosKingToCastle;
        _PosRookDest     = aInfos._PosRookDest;
    }
};

struct sDiagnostiqueCastling {
    bool _IsSmallCastling;
    bool _PathIsAttacked;
    sDiagnostiqueCastling ():
        _IsSmallCastling (false),
        _PathIsAttacked  (false)

    {
    }
};


typedef QVector<sInfosSquare> tQVectorInfosSquare;
typedef QVector< QPair<int, int> > tVecPairPos;

#endif // TYPEINFOSMOVES_H
