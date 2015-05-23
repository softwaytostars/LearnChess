#ifndef TYPEFICHIERPGN_H
#define TYPEFICHIERPGN_H

#include <QString>

struct sDataTypeEntete {
    QString _Event;
    QString _Site;
    QString _Round;
    QString _Date;
    QString _White;
    QString _Black;
    QString _WhiteElo;
    QString _BlackElo;
    QString _Resultat;
    sDataTypeEntete () :
        _Event(""), _Site(""), _Round(""),
        _Date(""), _White(""), _Black(""),
        _WhiteElo(""), _BlackElo(""), _Resultat("")
    {
    }

    sDataTypeEntete(const QString& aEvent,
                    const QString& aSite,
                    const QString& aRound,
                    const QString& aDate,
                    const QString& aWhite,
                    const QString& aBlack,
                    const QString& aWhiteElo,
                    const QString& aBlackElo,
                    const QString& aResultat):
        _Event(aEvent), _Site(aSite), _Round(aRound),
        _Date(aDate), _White(aWhite), _Black(aBlack),
        _WhiteElo(aWhiteElo), _BlackElo(aBlackElo), _Resultat(aResultat)
    {
    }
    sDataTypeEntete(const sDataTypeEntete& aEntete):
        _Event(aEntete._Event), _Site(aEntete._Site), _Round(aEntete._Round),
        _Date(aEntete._Date), _White(aEntete._White), _Black(aEntete._Black),
        _WhiteElo(aEntete._WhiteElo), _BlackElo(aEntete._BlackElo), _Resultat(aEntete._Resultat)
    {
    }

    inline bool operator == (const sDataTypeEntete& aEntete) const {
        return  ((aEntete._Event == _Event)
                 && (aEntete._Site == _Site)
                 && (aEntete._Round == _Round)
                 && (aEntete._Date == _Date)
                 && (aEntete._White == _White)
                 && (aEntete._Black == _Black)
                 && (aEntete._WhiteElo == _WhiteElo)
                 && (aEntete._BlackElo == _BlackElo)
                 && (aEntete._Resultat == _Resultat));
    }
};

#endif // TYPEFICHIERPGN_H
