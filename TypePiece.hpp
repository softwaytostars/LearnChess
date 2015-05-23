#ifndef TYPEPIECE_HPP
#define TYPEPIECE_HPP

#include <QVector>

enum eTypePiece {
    Queen=0,
    Bishop=1,
    Rook=2,
    Pawn=3,
    Knight=4,
    King=5
};

enum eColorPiece {
    BLACK=0,
    WHITE=1
};

typedef QVector<eTypePiece> tQVectorTypePiece;
std::ostream& operator<<(std::ostream& output, const eTypePiece& a);
typedef QVector<eColorPiece> tQVectorColorPiece;


#endif // TYPEPIECE_HPP
