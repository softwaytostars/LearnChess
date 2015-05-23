#include "infosconfiguration.h"
#include <QDir>
#include <QCoreApplication>
#include <fstream>
#include <QLocale>

// Initialisation du singleton à 0
InfosConfiguration* InfosConfiguration::_instance = 0;

InfosConfiguration* InfosConfiguration::instance () {
    if (!_instance) {
        _instance   = new InfosConfiguration();

        std::ifstream myfile;

        myfile.open (_instance->_FileConfigName.toStdString().c_str(), std::ios_base::in | std::ios_base::binary);
        if (myfile.is_open()) {
            //lit _LastFileNameForDemo
            _instance->ReadString (myfile, _instance->_LastFileNameForDemoPiecesMove);
            _instance->ReadString (myfile, _instance->_LastFileNameForTrainingPiecesMove);
            _instance->ReadString (myfile, _instance->_LastFileNameForDemoSpecialMoves);
            _instance->ReadString (myfile, _instance->_LastFileNameForTrainingSpecialMoves);
            _instance->ReadString (myfile, _instance->_LastFileNameForDemoPatternsMat);
            _instance->ReadString (myfile, _instance->_LastFileNameForTrainingPatternsMat);
            _instance->ReadString (myfile, _instance->_LastFileNameForDemoTactics);
            _instance->ReadString (myfile, _instance->_LastFileNameForTrainingTactics);
            _instance->ReadString (myfile, _instance->_LastDirForPGNFiles);
            _instance->ReadInt    (myfile, _instance->_PreferencesPieces);
            _instance->ReadInt    (myfile, _instance->_PreferencesStyle);
            _instance->ReadInt    (myfile, _instance->_PreferencesLanguage);
            myfile.close();
        }

        //à la premiere execution, on enregistre le repertoires des fichiers PGN comme étant PGNFilesForGames
        //dans le répertoire où se trouve l'executable (si il existe)
        if (_instance->getLastDirForPGNFiles().size() == 0)
            _instance->setLastDirForPGNFiles(QCoreApplication::applicationDirPath()+"/PGNFilesForGames");

    }
    return _instance;
}

void InfosConfiguration::kill () {

    if (_instance) {
        //d'abord sauve les infos
        std::ofstream myfile;
        myfile.open (_instance->_FileConfigName.toStdString().c_str(), std::ios::out | std::ios::binary);
        if (myfile.is_open()) {

            _instance->WriteString (myfile, _instance->_LastFileNameForDemoPiecesMove);
            _instance->WriteString (myfile, _instance->_LastFileNameForTrainingPiecesMove);
            _instance->WriteString (myfile, _instance->_LastFileNameForDemoSpecialMoves);
            _instance->WriteString (myfile, _instance->_LastFileNameForTrainingSpecialMoves);
            _instance->WriteString (myfile, _instance->_LastFileNameForDemoPatternsMat);
            _instance->WriteString (myfile, _instance->_LastFileNameForTrainingPatternsMat);
            _instance->WriteString (myfile, _instance->_LastFileNameForDemoTactics);
            _instance->WriteString (myfile, _instance->_LastFileNameForTrainingTactics);
            _instance->WriteString (myfile, _instance->_LastDirForPGNFiles);
            _instance->WriteInt    (myfile, _instance->_PreferencesPieces);
            _instance->WriteInt    (myfile, _instance->_PreferencesStyle);
            _instance->WriteInt    (myfile, _instance->_PreferencesLanguage);
        }
        delete _instance;
        _instance = 0;
    }
}

InfosConfiguration::InfosConfiguration():
    _LastFileNameForDemoPiecesMove (""),
    _FileConfigName (QCoreApplication::applicationFilePath()+".conf"),
    _LastFileNameForTrainingPiecesMove(""),
    _LastDirForPGNFiles (""),
    _LastFileNameForDemoSpecialMoves(""),
    _LastFileNameForTrainingSpecialMoves (""),
    _LastFileNameForDemoPatternsMat (""),
    _LastFileNameForTrainingPatternsMat (""),
    _LastFileNameForDemoTactics (""),
    _LastFileNameForTrainingTactics (""),
    _PreferencesPieces((int)ePiecesMerida),
    _PreferencesStyle ((int)eStyleDefault),
    _PreferencesLanguage((int)eLangueEN),
    _ModeConfigurateur(false)

{

    QString locale = QLocale::system().name();
    if (locale == "en")
        _PreferencesLanguage = eLangueEN;
    if (locale == "fr")
        _PreferencesLanguage = eLangueFR;

    _MapLangueToStr [eLangueEN] = "en";
    _MapLangueToStr [eLangueFR] = "fr";
}

void InfosConfiguration::setLastFileNameForDemo (const QString& aFileName, eTypeInitiation aTypeInitiation) {

    switch (aTypeInitiation) {
        case eInitierPiecesMoves  : _LastFileNameForDemoPiecesMove = aFileName; break;
        case eInitierSpecialMoves : _LastFileNameForDemoSpecialMoves = aFileName; break;
        case eInitierPatternsMat  : _LastFileNameForDemoPatternsMat = aFileName; break;
        case eInitierTactiques    : _LastFileNameForDemoTactics = aFileName; break;
    }
}

QString InfosConfiguration::getLastFileNameForDemo(eTypeInitiation aTypeInitiation) const {

    switch (aTypeInitiation) {
        case eInitierPiecesMoves  : return _LastFileNameForDemoPiecesMove;
        case eInitierSpecialMoves : return _LastFileNameForDemoSpecialMoves;
        case eInitierPatternsMat  : return _LastFileNameForDemoPatternsMat;
        case eInitierTactiques    : return _LastFileNameForDemoTactics;
        default : return QString("");
    }
}

void InfosConfiguration::setLastFileNameForTrainingMove (const QString& aFileName, eTypeInitiation aTypeInitiation) {

    switch (aTypeInitiation) {
        case eInitierPiecesMoves  : _LastFileNameForTrainingPiecesMove   = aFileName; break;
        case eInitierSpecialMoves : _LastFileNameForTrainingSpecialMoves = aFileName; break;
        case eInitierPatternsMat  : _LastFileNameForTrainingPatternsMat  = aFileName; break;
        case eInitierTactiques    : _LastFileNameForTrainingTactics      = aFileName; break;
    }
}

QString InfosConfiguration::getLastFileNameForTrainingMove(eTypeInitiation aTypeInitiation) const {

    switch (aTypeInitiation) {
        case eInitierPiecesMoves  : return _LastFileNameForTrainingPiecesMove;
        case eInitierSpecialMoves : return _LastFileNameForTrainingSpecialMoves;
        case eInitierPatternsMat  : return _LastFileNameForTrainingPatternsMat;
        case eInitierTactiques    : return _LastFileNameForTrainingTactics;
        default : return QString("");
    }

}

QString InfosConfiguration::getLastLanguageAppToStr() const {

    eLangue lLangue = (eLangue) _PreferencesLanguage;
    if (_MapLangueToStr.contains(lLangue))
        return _MapLangueToStr [lLangue];
    else
        return "en";
}


void InfosConfiguration::ReadString (std::ifstream& afilestream, QString& aValue) {

    std::string::size_type sz;

    afilestream.read(reinterpret_cast<char*>(&sz), sizeof(std::string::size_type));
    if (sz > 0) {
        aValue.resize(sz);
        char *memblock = new char [sz+2];
        afilestream.read(memblock, sz+1);
        aValue = QString(memblock);
        delete [] memblock;
        memblock = 0;
    }
}

void InfosConfiguration::WriteString (std::ofstream& afilestream, const QString& aValue) {

    std::string::size_type sz = aValue.toStdString().size();
    afilestream.write(reinterpret_cast<char*>(&sz), sizeof(std::string::size_type));
    if (sz > 0)
        afilestream.write(aValue.toStdString().c_str(), sz+1);
}

void InfosConfiguration::ReadInt (std::ifstream &afilestream, int &aValue) {

    int lValue;
    afilestream.read(reinterpret_cast<char*>(&lValue), sizeof(int));
    aValue = lValue;
}

void InfosConfiguration::WriteInt (std::ofstream& afilestream, int aValue) {

    afilestream.write (reinterpret_cast<char*>(&aValue), sizeof(int));
}
