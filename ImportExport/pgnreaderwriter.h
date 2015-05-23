#ifndef PGNREADERWRITER_H
#define PGNREADERWRITER_H

#include <QString>
#include "TypePiece.hpp"
#include "TypeFichierPGN.h"
#include "exceptionsInfos.h"

class CommandPlay;
class IChessManager;
class ILoaderGame;
class QTextStream;

class PGNReaderWriter
{
public:
    PGNReaderWriter(const QString &aFileName);
    void OverWriteEntete (const sDataTypeEntete &aDataEntete, int aNoGame=1) const;
    bool DeleteGame(int aNoGame) const;
    void LoadGame (ILoaderGame &aLoaderGame, int aNoGame = 1 );
    void WriteGame (const QString &aFileName,
                    const sDataTypeEntete& aDataEntete,
                    const QVector<CommandPlay *> &aListCommandesPlay);
    void ReadEntete(int aNoGame=1);

    const QString& getEvent() const { return _Event;}
    const QString& getSite()  const { return _Site;}
    const QString& getDate()  const { return _Date;}
    const QString& getRound() const { return _Round;}
    const QString& getWhiteName() const { return _WhiteName;}
    const QString& getBlackName() const { return _BlackName;}
    const QString& getResultat()  const { return _Resultat;}
    const QString& getWhiteELO()  const { return _WhiteELO;}
    const QString& getBlackELO()  const { return _BlackELO;}

    int NbreGames () const;

//    void setEvent (const QString& aEvent) {_Event = aEvent;}
//    void setSite (const QString& aSite)  {_Site = aSite;}
//    void setDate (const QString& aDate) {_Date = aDate;}
//    void setRound(const QString& aRound) {_Round = aRound;}
//    void setWhiteName(const QString& aWhiteName) {_WhiteName = aWhiteName;}
//    void setBlackName(const QString& aBlackName) {_BlackName = aBlackName;}
//    void setResultat(const QString& aResultat) {_Resultat = aResultat;}
//    void setWhiteELO(const QString& aWhiteELO) {_WhiteELO = aWhiteELO;}
//    void setBlackELO(const QString& aBlackELO) {_BlackELO = aBlackELO;}

private:
    bool EstUneNotationPiece (const QChar aNote, eTypePiece& aType);
    void ParseDataDemiCoup (eColorPiece aColor,
                            const QString &aDataDemiCoup,
                            int aPosEndPreviousDemiCoup,
                            int *aPosStartNextDemiCoup,
                            const QString &aDataGameFile,
                            ILoaderGame& aLoaderGame);
    void ChercherPosEndCommentary (int *aPosStartNextDemiCoup,
                                   const QString &aDataGameFile,
                                   int aPosStartForLooking);
    void getLectureString (int aNoGame, const QString &afilter, QString& aVar);
    void WriteEntete(QTextStream &astreamfile, const sDataTypeEntete &aDataEntete) const;
    void RelancerException (CommandPlay* aCommandPlay);
    int FindPosNextMove (const QString &adataGameFile,
                                  int aNbreCoups,
                                  QString& aKeyBegin,
                                  int aLastPosition);
    bool EstUneLigneEnteteTraitee (const QString& aline) const;
    QString getDataGame(int aNoGame);
    bool TrouveDebutEnTete (int aPosDebutFile, int& aPosDebutEnTete);
    bool TrouveDelimiteur (const QString& aData, int aPosDebut, int& aPosDelim) const;
    void ChercherDernierDelimiteur (const QString& aData, int& aPosDelim) const;
    void EnleverDelimiteurSiExistant (QString& aData) const;

    QString _FileName;
    QString _DataFile;
    QString _Event;
    QString _Site;
    QString _Date;
    QString _Round;
    QString _WhiteName;
    QString _BlackName;
    QString _Resultat;
    //facultatif
    QString _WhiteELO, _BlackELO;

    QVector<QString> _CKeys;
    QString _DataGame;
};

#endif // PGNREADERWRITER_H
