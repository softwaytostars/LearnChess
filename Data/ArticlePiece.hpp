#ifndef ARTICLEPIECE_HPP
#define ARTICLEPIECE_HPP

#include "TypePiece.hpp"
#include <QPair>

struct ArticlePiece {

    int _Id; /**< identifiant */
    eTypePiece  _TypePiece;  /**< Type de la pièce */
    eColorPiece _Color; /**< Couleur de la piece */
    int _PosX; /**< Position X sur l'echiquier, de 1 à 8 */
    int _PosY; /**< Position Y sur l'echiquier, de 1 à 8 */
    bool _HasEverMoved; /**< Donne l'état de la pièce si elle a bougé ou pas */

    ArticlePiece () : _Id (-1),
        _TypePiece(Pawn),
        _Color (WHITE),
        _PosX (1),
        _PosY (1),
        _HasEverMoved(false){}
    ArticlePiece (int aId, eTypePiece aType, eColorPiece aColor, int aPosX, int aPosY, bool aHasEverMoved):
        _Id(aId),
        _TypePiece(aType),
        _Color (aColor),
        _PosX (aPosX),
        _PosY (aPosY),
        _HasEverMoved(aHasEverMoved){}
    ArticlePiece (const ArticlePiece& a): _Id(a._Id),
        _TypePiece(a._TypePiece),
        _Color (a._Color),
        _PosX (a._PosX),
        _PosY (a._PosY),
        _HasEverMoved(a._HasEverMoved){
    }

    inline bool EstALaPosition (int aPosX, int aPosY) { return ((_PosX==aPosX) && (_PosY==aPosY));}

    int ColorToInt () const {
        return ColorToInt (_Color);
    }
    static int ColorToInt (const eColorPiece& aColor) {
        if (aColor == WHITE) return 0;
        else return 1;
    }
    int TypePieceToInt () const {
        return TypePieceToInt (_TypePiece);
    }
    static int TypePieceToInt (const eTypePiece& aType) {
        return ((int) (aType));
    }


    inline bool operator == (const ArticlePiece& aPiece) const {
        return  ((_TypePiece == aPiece._TypePiece)
                 && (_PosX == aPiece._PosX)
                 && (_PosY == aPiece._PosY)
                 && (_Color == aPiece._Color));
    }
    inline bool operator != (const ArticlePiece& aPiece) const {
        return  !(this->operator ==(aPiece));
    }
};


//definitions de types à partir de la structure
typedef QVector<ArticlePiece> tlisteArticlesPieces;


#endif // ARTICLEPIECE_HPP
