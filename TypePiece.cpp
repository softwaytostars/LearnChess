#include "TypePiece.hpp"

std::ostream& operator<<(std::ostream& output, const eTypePiece& a) {
    switch (a) {
        case King   : output << "King";break;
        case Queen  : output << "Queen";break;
        case Bishop : output << "Bishop";break;
        case Rook   : output << "Rook";break;
        case Pawn   : output << "Pawn";break;
        case Knight : output << "Knight";break;
    }
    return output;
}
