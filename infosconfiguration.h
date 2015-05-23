#ifndef INFOSCONFIGURATION_H
#define INFOSCONFIGURATION_H

#include <QString>
#include "TypeDemo.hpp"
#include "Types/TypePreferences.h"
#include "Types/TypeLangage.h"

class InfosConfiguration
{
public:
    static InfosConfiguration* instance ();
    static void kill ();

    void setLastFileNameForDemo (const QString& aFileName, eTypeInitiation aTypeInitiation);
    QString getLastFileNameForDemo(eTypeInitiation aTypeInitiation) const;

    void setLastFileNameForTrainingMove (const QString& aFileName, eTypeInitiation aTypeInitiation);
    QString getLastFileNameForTrainingMove(eTypeInitiation aTypeInitiation) const;

    void setLastDirForPGNFiles (const QString& aDir) {_LastDirForPGNFiles = aDir;}
    const QString& getLastDirForPGNFiles() const {return _LastDirForPGNFiles;}

    void setLastTypePieces (ePreferencesPieces aPrefPieces) {_PreferencesPieces = (int) aPrefPieces;}
    ePreferencesPieces getLastTypePieces() const { return (ePreferencesPieces) _PreferencesPieces;}

    void setLastStyleApp (ePreferenceStyle aPrefStyle) {_PreferencesStyle = (int) aPrefStyle;}
    ePreferenceStyle getLastStyleApp() const { return (ePreferenceStyle) _PreferencesStyle;}

    void setLanguage (eLangue aLangue) {_PreferencesLanguage = (int) aLangue;}
    QString getLastLanguageAppToStr() const;
    const tMapPrefLangueName& get_MapPrefLangueName () {return _MapLangueToStr;}

    void setModeConfigurateur (bool aModeConfigurateur) {_ModeConfigurateur = aModeConfigurateur;}
    bool getModeConfigurateur () const {return _ModeConfigurateur;}


private:
    InfosConfiguration();
    static InfosConfiguration* _instance;
    QString _LastFileNameForDemoPiecesMove; /**< Nom fichier pour la demo des Pieces moves */
    QString _FileConfigName;
    QString _LastFileNameForTrainingPiecesMove;/**< Nom fichier pour le training des Pieces moves */
    QString _LastDirForPGNFiles;
    QString _LastFileNameForDemoSpecialMoves;
    QString _LastFileNameForTrainingSpecialMoves;
    QString _LastFileNameForDemoPatternsMat;
    QString _LastFileNameForTrainingPatternsMat;
    QString _LastFileNameForDemoTactics;
    QString _LastFileNameForTrainingTactics;
    int _PreferencesPieces;
    int _PreferencesStyle;
    int _PreferencesLanguage;
    tMapPrefLangueName _MapLangueToStr; /**< map entre le enum langage et sa valeur string */
    bool _ModeConfigurateur;//sera pas sauve dans fichier


    void ReadString (std::ifstream &afilestream, QString &aValue);
    void WriteString (std::ofstream& afilestream, const QString& aValue);

    void ReadInt (std::ifstream &afilestream, int &aValue);
    void WriteInt (std::ofstream& afilestream, int aValue);
};



#endif // INFOSCONFIGURATION_H
