#ifndef TYPEPREFERENCES_H
#define TYPEPREFERENCES_H
#include <QMap>

enum ePreferenceStyle {
    eStyleDefault,
    eStyleKid
};

enum ePreferencesPieces {
    ePiecesMerida=0,
    ePiecesBerlin=1,
    ePiecesKid=2,
    ePiecesGabriel=3,
    ePiecesEnsifer=4,
    ePiecesAustria=5
};

typedef QMap<ePreferenceStyle,   QString> tMapPrefStyleAppName;
typedef QMap<ePreferencesPieces, QString> tMapPrefPieceName;

#endif // TYPEPREFERENCES_H
