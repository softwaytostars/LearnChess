#include "pgnreaderwriter.h"
#include <fstream>
#include <QFile>
#include <QTextStream>
#include "ChessBoardManager/IChessManager.h"
#include "ChessBoardManager/commandplay.h"
#include "ChessBoardManager/ILoaderGame.h"
#include <cctype>
#include "infosgraphiques.h"

#ifdef _DEBUG
#include <QDebug>
#endif


const QString CKeyEvent     = "Event";
const QString CKeySite      = "Site";
const QString CKeyDate      = "Date";
const QString CKeyRound     = "Round";
const QString CKeyWhite     = "White";
const QString CKeyBlack     = "Black";
const QString CKeyResult    = "Result";
const QString CKeyWhiteElo  = "WhiteElo";
const QString CKeyBlackElo  = "BlackElo";



PGNReaderWriter::PGNReaderWriter(const QString& aFileName):
    _FileName(aFileName),
    _DataFile(""),
    _Event (""),
    _Site (""),
    _Date (""),
    _Round (""),
    _WhiteName (""),
    _BlackName (""),
    _Resultat (""),
    _WhiteELO(""),
    _BlackELO(""),
    _DataGame ("")
{
    std::ifstream myfile;
    myfile.open (aFileName.toStdString().c_str(), std::ios::in);
    if (!myfile.is_open()) return;


    _CKeys.push_back(CKeyEvent);
    _CKeys.push_back(CKeySite);
    _CKeys.push_back(CKeyDate);
    _CKeys.push_back(CKeyRound);
    _CKeys.push_back(CKeyWhite);
    _CKeys.push_back(CKeyBlack);
    _CKeys.push_back(CKeyResult);
    _CKeys.push_back(CKeyWhiteElo);
    _CKeys.push_back(CKeyBlackElo);

    //lecture du fichier que l'on va stocker dans un string
    std::filebuf* lpbuf = myfile.rdbuf();
    if (lpbuf) {
        // get file size using buffer's members
        int lsize = lpbuf->pubseekoff (0, std::ios::end, std::ios::in);
        lpbuf->pubseekpos (0, std::ios::in);
        // allocate memory to contain file data
        char* buffer = new char[lsize];

        // get file data
        lpbuf->sgetn (buffer, lsize);
        _DataFile = QString (buffer);
        delete buffer;
    }

    myfile.close();
}

//compte nbre occurences du param obligatoire [CKeyEvent "
int PGNReaderWriter::NbreGames () const {

    QString lstr = "["+CKeyEvent+ " \"";
    int n = _DataFile.count(lstr);
    return n;
}

void PGNReaderWriter::getLectureString (int aNoGame, const QString& afilter, QString& aVar) {

    const QString& lDataGame = getDataGame(aNoGame);

    if (lDataGame.size() == 0) return;
    int lPos, lPosDebut, lPosEnd;
    if ((lPos= lDataGame.indexOf(afilter)) != -1) {

        lPosDebut = lPos+afilter.size()+2;
        lPosEnd   = lDataGame.indexOf(']', lPos) -1;
        aVar      = lDataGame.mid(lPosDebut, lPosEnd-lPosDebut);
    }
}

void PGNReaderWriter::ReadEntete(int aNoGame) {

    _DataGame = "";//reinitialise à chaque lecture

    getLectureString (aNoGame, CKeyEvent,    _Event);
    getLectureString (aNoGame, CKeySite,     _Site);
    getLectureString (aNoGame, CKeyDate,     _Date);
    getLectureString (aNoGame, CKeyRound,    _Round);
    getLectureString (aNoGame, CKeyWhite,    _WhiteName);
    getLectureString (aNoGame, CKeyBlack,    _BlackName);
    getLectureString (aNoGame, CKeyResult,   _Resultat);
    getLectureString (aNoGame, CKeyWhiteElo, _WhiteELO);
    getLectureString (aNoGame, CKeyBlackElo, _BlackELO);
}

bool PGNReaderWriter::EstUneLigneEnteteTraitee (const QString& aline) const {

    for (int i=0; i < _CKeys.count(); i++) {
        if (aline.contains(_CKeys [i])){
            //on parcourt la ligne entete pour avoir tout le nom de la clé
            //à partir de [+1 jusqu'à \" ou espace
            for (int j=1; j < aline.size (); j++) {
                if ( (aline.at(j) == ' ') || (aline.at(j) == '\"'))
                    return true;//car on a pas trouvé de diff donc c'est la meme
                if ( (_CKeys[i].size() < j) || (aline.at(j) != _CKeys[i].at(j-1)))
                    break;//il faut essayer une autre cle
            }
        }
    }
    return false;
}

void PGNReaderWriter::OverWriteEntete (const sDataTypeEntete &aDataEntete, int aNoGame) const
{

    //ouvre un fichier à ecrire
    QFile fileOut ("out.txt");
    if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    //ouvre unstream pour ecrire
    QTextStream out(&fileOut);

    //ouvre le fichier que l'on veut re-ecrire
    QFile file(_FileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    //lit le fichier line/line
    QTextStream in(&file);
    int lNoGame = 0;
    bool lEnteteEcrite = false;
    while (!in.atEnd()) {

        QString line = in.readLine();
        if (line.contains("[" + CKeyEvent   + " \""))
            lNoGame++;
        if (lNoGame == aNoGame) {

           if (!lEnteteEcrite)
             WriteEntete (out, aDataEntete);
           lEnteteEcrite = true;

            if (line.contains("\"]")) {
                if (EstUneLigneEnteteTraitee (line))
                  continue;
            }
        }
        //ecrit les lignes suivantes du fichier dans le nouveau
        out << line << "\n";
    }
    file.close();
    fileOut.close();

    if (QFile::remove (_FileName))
        QFile::rename("out.txt", _FileName);
}

bool PGNReaderWriter::DeleteGame (int aNoGame) const {

    //si un seul jeu, on supprime le fichier, sinon seulement la partie
    if (NbreGames () == 1) {
        return QFile::remove(_FileName);
    } else
    {
        //ouvre un fichier à ecrire
        QFile fileOut ("out.txt");
        if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;
        //ouvre unstream pour ecrire
        QTextStream out(&fileOut);

        //ouvre le fichier que l'on veut re-ecrire
        QFile file(_FileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;
        //lit le fichier line/line
        QTextStream in(&file);
        int lNoGame = 0;
        while (!in.atEnd()) {

            QString line = in.readLine();
            //compte à quelle partie on est
            if (line.contains("[" + CKeyEvent   + " \""))
                lNoGame++;
            //n'ecrit pas les lignes correpsondant à la partie
            if (lNoGame == aNoGame) {
                continue;
            }
            //ecrit les lignes suivantes du fichier dans le nouveau
            out << line << '\n';
        }
        file.close();
        fileOut.close();

        if (QFile::remove (_FileName))
            return QFile::rename("out.txt", _FileName);
    }

    return false;
}

bool PGNReaderWriter::TrouveDelimiteur (const QString& aData, int aPosDebut, int& aPosDelim) const {

    aPosDelim = aData.indexOf(' ', aPosDebut);
    int lPosLine = aData.indexOf('\n', aPosDebut);
    if (aPosDelim < 0)
        aPosDelim = lPosLine;
    if ((lPosLine >= 0) && (lPosLine< aPosDelim))
        aPosDelim = lPosLine;
    int lPosRetour = aData.indexOf('\r', aPosDebut);
    if (aPosDelim < 0)
        aPosDelim = lPosRetour;
    if ((lPosRetour >= 0) && (lPosRetour< aPosDelim))
        aPosDelim = lPosRetour;
    return (aPosDelim >= 0);
}

void PGNReaderWriter::ChercherDernierDelimiteur (const QString& aData, int& aPosDelim) const {

    if (aPosDelim >= 0) {
        int lPosDelim = -1;
        while (TrouveDelimiteur (aData, aPosDelim+1, lPosDelim) && (lPosDelim==(aPosDelim+1))) {
            aPosDelim = lPosDelim;
        }
    }
}

void PGNReaderWriter::EnleverDelimiteurSiExistant (QString& aData) const {

    aData.remove(' ');
    aData.remove('\n');
    aData.remove('\r');
}

void PGNReaderWriter::LoadGame (ILoaderGame& aLoaderGame, int aNoGame) {

    //lit l'entete
    ReadEntete(aNoGame);

    //prend la reference sur les donnees du jeu
    const QString& lDataGame = getDataGame(aNoGame);

    aLoaderGame.InitializeGame(true);
    if (lDataGame.size() == 0) return;

    size_t lPosLastEntete = lDataGame.lastIndexOf(']');
    while ( (lPosLastEntete >= 1) &&
            (lDataGame [lPosLastEntete-1] != '\"') )
       lPosLastEntete = lDataGame.mid(0, lPosLastEntete-1).lastIndexOf(']');
    if (lPosLastEntete<=0) return;


    QString lDataNotationGame = lDataGame.mid(lPosLastEntete);

    //lecture de la partie
    int lPosDebutCoup=0, lPosStartNextDemiCoup=0;
    int lNbreCoups = 1;

    QString lKeyBegin;
    lPosDebutCoup = FindPosNextMove (lDataNotationGame, lNbreCoups, lKeyBegin, 0);



    try {
        while (lPosDebutCoup != -1){


#ifdef _DEBUG
    qDebug() << "lNbreCoups ="<<lNbreCoups;
#endif

            lPosDebutCoup += lKeyBegin.size();

            //cherche prochain espace
            int lPosNextSpace = -1;
            if (!TrouveDelimiteur (lDataNotationGame, lPosDebutCoup, lPosNextSpace))
                throw Exceptions ("problem delimiteur!");

            ChercherDernierDelimiteur (lDataNotationGame, lPosNextSpace);

            if ((lPosDebutCoup < 0) || (lPosNextSpace < 0))
                throw Exceptions ("problem lPosDebutCoup!");

            //trouve les data du demi-coup des blancs
            QString lDataWhite = lDataNotationGame.mid(lPosDebutCoup, lPosNextSpace - lPosDebutCoup);
            EnleverDelimiteurSiExistant (lDataWhite);

            //parse les donnees du demi coup blanc
            ParseDataDemiCoup (WHITE,
                               lDataWhite,
                               lPosNextSpace,
                               &lPosStartNextDemiCoup,
                               lDataNotationGame,
                               aLoaderGame);

            if (!TrouveDelimiteur (lDataNotationGame, lPosStartNextDemiCoup, lPosNextSpace))
                throw Exceptions ("problem delimiteur!");

            ChercherDernierDelimiteur (lDataNotationGame, lPosNextSpace);

            if ((lPosNextSpace - lPosStartNextDemiCoup) <=0)
                throw Exceptions ("lPosNextSpace - lPosStartNextDemiCoup");

            //trouve les data du demi-coup des blancs
            QString lDataBlack = lDataNotationGame.mid(lPosStartNextDemiCoup, lPosNextSpace - lPosStartNextDemiCoup);
            EnleverDelimiteurSiExistant (lDataBlack);

            //parse les donnees du demi coup noir
            ParseDataDemiCoup (BLACK,
                               lDataBlack,
                               lPosNextSpace,
                               &lPosStartNextDemiCoup,
                               lDataNotationGame,
                               aLoaderGame);

            lNbreCoups  += 1;
            lPosDebutCoup = FindPosNextMove (lDataNotationGame, lNbreCoups, lKeyBegin, lPosStartNextDemiCoup);

        }//while (lPosDebutCoup = lDataGameFile.find(QString("%1.").arg(lNbreCoups).toStdString()) != std::string::npos)

    }catch(...) {

#ifdef _DEBUG
    qDebug() << "pgnread exception occured";
#endif
    }
}

int PGNReaderWriter::FindPosNextMove (const QString& adataGameFile,
                                       int aNbreCoups,
                                       QString& aKeyBegin,
                                       int aLastPosition) {

    QString lKeyBegin1 = QString("%1. ").arg(aNbreCoups);
    QString lKeyBegin2 = QString("%1.\n").arg(aNbreCoups);
    QString lKeyBegin3 = QString("%1.").arg(aNbreCoups);


    int lPosDebutCoup1 = adataGameFile.indexOf(lKeyBegin1, aLastPosition);
    int lPosDebutCoup2 = adataGameFile.indexOf(lKeyBegin2, aLastPosition);
    int lPosDebutCoup3 = adataGameFile.indexOf(lKeyBegin3, aLastPosition);

    int lPosDebutCoup = qMax<int> (lPosDebutCoup1, lPosDebutCoup2);
    lPosDebutCoup = qMax<int> (lPosDebutCoup, lPosDebutCoup3);
    if (lPosDebutCoup != -1)
        lPosDebutCoup += 1;


    if (lPosDebutCoup1 >=0) {
        aKeyBegin     = lKeyBegin1;
        lPosDebutCoup = lPosDebutCoup1;
    }
    if ((lPosDebutCoup2 >= 0) && (lPosDebutCoup2 < lPosDebutCoup)) {
        aKeyBegin     = lKeyBegin2;
        lPosDebutCoup = lPosDebutCoup2;
    }
    if ((lPosDebutCoup3 >=0) && (lPosDebutCoup3 < lPosDebutCoup)) {
        aKeyBegin     = lKeyBegin3;
        lPosDebutCoup = lPosDebutCoup3;
    }

    return lPosDebutCoup;
}

bool PGNReaderWriter::EstUneNotationPiece (const QChar aNote, eTypePiece& aType) {

    return (InfosGraphiques::instance()->get_TypePieceDeNotation (aNote.toAscii(), aType)
            && (aType!= Pawn) );
}

void PGNReaderWriter::ParseDataDemiCoup (eColorPiece aColor,
                                         const QString& aDataDemiCoup,
                                         int aPosEndPreviousDemiCoup,
                                         int* aPosStartNextDemiCoup,
                                         const QString& aDataGameFile,
                                         ILoaderGame &aLoaderGame) {


    if (aDataDemiCoup.size() ==0 ) return;

    bool lGrandRoque = false;
    bool lPetitRoque = false;
    if (aDataDemiCoup.indexOf('-') != -1) {
        //verifie si grand roque ou petit roque
        if (aDataDemiCoup.indexOf("O-O-O") != -1) {
            lGrandRoque = true;
        } else if (aDataDemiCoup.indexOf("O-O") != -1) {
            lPetitRoque = true;
        } else
            return; //partie terminee
    }

    CommandPlay *lCommandPlay = new CommandPlay (aLoaderGame.getChessManager());
    if (!lCommandPlay)  throw Exceptions ("Pas de commandPlay cree");

    try {
        bool lEchec   = (aDataDemiCoup.indexOf("+") != -1);
        if (lGrandRoque || lPetitRoque) {

            lCommandPlay->AddActionCastling(aColor, lPetitRoque);

        } else {

            eTypePiece lTypePiece;
            eTypePiece lPiecePromotion = Pawn;
            int lPos = 0;
            bool lOk;
            bool lDestroy = (aDataDemiCoup.indexOf("x") != -1);
            bool lMat     = (aDataDemiCoup.indexOf("#") != -1);
            bool lPriseEnPassant = (aDataDemiCoup.indexOf("e.p") != -1);

            int lOldPosX =0;
            int lOldPosY =0;
            int lPosNewX, lPosNewY;
            if (!EstUneNotationPiece (aDataDemiCoup [lPos], lTypePiece))
            {
                if (lDestroy) {
                    lOldPosX = aLoaderGame.getChessManager().GetXPos (aDataDemiCoup[lPos].toUpper().toAscii());
                    lPos++;
                }
                if (aDataDemiCoup[lPos] == 'x') {
                  lPos++;
                }
                //si pion, alors on a que sa position
                lPosNewX = aLoaderGame.getChessManager().GetXPos (aDataDemiCoup[lPos].toUpper().toAscii());
                lPos++;
                lPosNewY = QString(QChar(aDataDemiCoup[lPos])).toInt(&lOk);
                if (!lOk) throw Exceptions ("lPosNewY pas converti!");
                //verifie si promotion
                lPos = aDataDemiCoup.indexOf('=');
                bool lPromote = (lPos != -1);
                if (lPromote) {
                    if (!EstUneNotationPiece (aDataDemiCoup [lPos+1], lPiecePromotion)) throw Exceptions ("Pb lecture promote");
                }
            }
            else {//si notation piece, lit sa colonne si renseignee (cas ambiguite)

                int lSize = aDataDemiCoup.size();
                if (lDestroy)
                    lSize--;
                if (lEchec)
                    lSize--;
                if (lMat)
                    lSize--;
                if (lPriseEnPassant)
                    lSize -= QString("e.p.").size();
                bool lAmbiguite = (lSize == 4);

                lPos++;
                lOldPosX = 0;
                lOldPosY = 0;
                if (lAmbiguite) {
                    lOldPosX = aLoaderGame.getChessManager().GetXPos (aDataDemiCoup[lPos].toUpper().toAscii());
                    if (lOldPosX<=0) {
                        //si ici, c'est qu'il ne s'agit pas d'une colonne mais d'une ligne
                        lOldPosY = QString(QChar(aDataDemiCoup[lPos])).toInt(&lOk);
                        if (!lOk) throw Exceptions ("lOldPosX et lOldPosY pas trouvé(ambiguite)");
                    }
                    lPos++;
                }

                if (aDataDemiCoup[lPos] == 'x') {
                  lPos++;
                }

                lPosNewX = aLoaderGame.getChessManager().GetXPos (aDataDemiCoup[lPos].toUpper().toAscii());
                lPos++;
                lPosNewY = QString(QChar(aDataDemiCoup[lPos])).toInt(&lOk);
                if (!lOk) throw Exceptions ("lPosNewY pas trouvé (2)");
                lPos++;
              }


              ArticlePiece lPiece;
              if ( (lOldPosX > 0) || (lOldPosY > 0))  {
                  if (!aLoaderGame.getChessManager().findPieceAtPosThatCanMoveTo (lOldPosX, lOldPosY,
                                                                      aColor,
                                                                      lTypePiece,
                                                                      lPosNewX, lPosNewY,
                                                                      lPiece)) throw Exceptions ("Pb findPieceAtPosThatCanMoveTo avec lOldPosX>0");
              }
              else
                 if (!aLoaderGame.getChessManager().findPieceThatCanMoveTo (aColor,
                                                            lTypePiece,
                                                            lPosNewX,
                                                            lPosNewY,
                                                            lPiece)) throw Exceptions ("Pb findPieceThatCanMoveTo");

              ArticlePiece lPiecetoDestroy;
              if (lDestroy) {
                  if (aLoaderGame.getChessManager().UnePieceExiste (lPosNewX, lPosNewY, lPiecetoDestroy)){
                    lCommandPlay->AddActionMangerPiece (lPiece, lPiecetoDestroy);
                  }
                  else {
                    //chercher le pion au dessus ou en dessous lPosNewY
                    if ((aLoaderGame.getChessManager().UnePieceExiste (lPosNewX, lPosNewY+1, lPiecetoDestroy)
                            || aLoaderGame.getChessManager().UnePieceExiste (lPosNewX, lPosNewY-1, lPiecetoDestroy))
                            && (lPiecetoDestroy._TypePiece == Pawn)
                            && (lPiece._PosY == lPiecetoDestroy._PosY))
                        lCommandPlay->AddActionPriseEnPassant(lPiece, lPosNewX, lPosNewY, lPiecetoDestroy);
                  }
              }else
              {
                 lCommandPlay->AddActionMove (lPiece, lPosNewX, lPosNewY);
              }

              if (lPiecePromotion != Pawn) {
                  lPiece._PosX = lPosNewX;
                  lPiece._PosY = lPosNewY;
                  lCommandPlay->AddActionPromotion(lPiece, lPiecePromotion);
              }
            }//else (grandroque ou petitroque)

          *aPosStartNextDemiCoup = -1;
          //juste apres l'espace, verifie si commentaire ou variante ou NAG
          ChercherPosEndCommentary (aPosStartNextDemiCoup, aDataGameFile, aPosEndPreviousDemiCoup+1);

          if (*aPosStartNextDemiCoup == -1)
            *aPosStartNextDemiCoup = aPosEndPreviousDemiCoup +1;//si pas de commentaire, le demi coup noir est juste apres l'espace
          else {
              lCommandPlay->setCommentary(aDataGameFile.mid(aPosEndPreviousDemiCoup+1, *aPosStartNextDemiCoup- (aPosEndPreviousDemiCoup+1)));
                *aPosStartNextDemiCoup += 1;//pour apres l'espace
            }

          if (lCommandPlay->Execute(lEchec)) {
              //ajoute demi coup dans le loader
              aLoaderGame.AjouterDemiCoup(lCommandPlay);
          }
          else {
              lCommandPlay->UnExecute();
              throw Exceptions ("Pb Execution");
          }


        }//try
        catch (const std::exception &e) {

#ifdef _DEBUG
    qDebug() << e.what();
#endif
            RelancerException (lCommandPlay);
        }
        catch(...) {
            RelancerException (lCommandPlay);
        }
}

void PGNReaderWriter::RelancerException (CommandPlay* aCommandPlay)  {
    if (aCommandPlay)
        delete aCommandPlay;
    throw Exceptions ("problem");//relance exception + haut
}

void PGNReaderWriter::ChercherPosEndCommentary (int* aPosStartNextDemiCoup,
                                                const QString& aDataGameFile,
                                                int aPosStartForLooking) {

    //juste apres l'espace, verifie si commentaire ou variante
    bool lHasVariante         = (aDataGameFile.indexOf('(', aPosStartForLooking-2) == aPosStartForLooking);
    bool lHasCommentaryAcc    = (aDataGameFile.indexOf('{', aPosStartForLooking-2) == aPosStartForLooking);
    bool lHasCommentaryAutre  = (aDataGameFile.indexOf(';', aPosStartForLooking-2) == aPosStartForLooking);
    bool lHasNAG              = (aDataGameFile.indexOf('$', aPosStartForLooking-2) == aPosStartForLooking);
    bool lHasPriseEnPassant   = (aDataGameFile.indexOf("e.p.", aPosStartForLooking-2) == aPosStartForLooking);

    if (lHasVariante) {
      *aPosStartNextDemiCoup = aDataGameFile.indexOf (')', aPosStartForLooking)+1;
    }

    if (lHasCommentaryAcc) {
      //prend le premiere fin accolade et se met apres
      *aPosStartNextDemiCoup = aDataGameFile.indexOf ('}', aPosStartForLooking)+1;
      int lPosAutreAcc = aDataGameFile.mid (aPosStartForLooking+1, *aPosStartNextDemiCoup  - aPosStartForLooking - 1).indexOf('{');
      if (lPosAutreAcc != -1) {
         *aPosStartNextDemiCoup = aDataGameFile.indexOf ('}', *aPosStartNextDemiCoup)+1;

      }
    }

    if (lHasCommentaryAutre) {
      //prend le premier retour ligne et se met après
      *aPosStartNextDemiCoup = aDataGameFile.indexOf ('\n', aPosStartForLooking)+1;
    }

    if (lHasNAG) {
        //prend le premier espace apres le NAG et se met derrière
        *aPosStartNextDemiCoup =  aDataGameFile.indexOf (' ', (aPosStartForLooking));
    }

    if (lHasPriseEnPassant) {
        *aPosStartNextDemiCoup =  aDataGameFile.indexOf("e.p.", aPosStartForLooking) + QString("e.p.").size();
    }

    //cherche si d'autres commentaires derriere
    if (lHasVariante || lHasCommentaryAcc || lHasCommentaryAutre || lHasNAG || lHasPriseEnPassant)
        ChercherPosEndCommentary (aPosStartNextDemiCoup, aDataGameFile, *aPosStartNextDemiCoup+1);
}

void PGNReaderWriter::WriteGame (const QString &aFileName,
                                 const sDataTypeEntete &aDataEntete,
                                 const QVector<CommandPlay *> &aListCommandesPlay) {


    QFile fileOut (aFileName);
    if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        return;
    //ouvre unstream pour ecrire
    QTextStream outStream(&fileOut);


    try
    {
        WriteEntete(outStream, aDataEntete);

        int lNbreCoups = 0, lNbreDemiCoups = 0;
        for (QVector<CommandPlay*>::const_iterator it = aListCommandesPlay.begin();
             it != aListCommandesPlay.end();
             ++it) {

            lNbreDemiCoups += 1;
            CommandPlay* lCommand = dynamic_cast<CommandPlay*>(*it);
            if (!lCommand) throw "problem";
            lNbreCoups = (lNbreDemiCoups/2) + (lNbreDemiCoups % 2);
            if (lNbreDemiCoups%2 == 1)
                outStream << lNbreCoups << ". ";

            QString lDescriptionCoup = lCommand->getDescription ();
            if (lDescriptionCoup.size() > 0) {
                outStream << lDescriptionCoup;
             } else throw "problem";

            QString lCommentaireCoup = lCommand->getCommentary();
            if (lCommentaireCoup.size() > 0) {
                outStream << " "<<lCommentaireCoup;
            }

            //met un retour ligne tous les 8 coups
            if (lNbreCoups%8 == 1)
                outStream << "\n";
            else
                outStream << ' ';


        }//for
        outStream << "\n";

    }catch(...)
    {

    }
    fileOut.close();
}

void PGNReaderWriter::WriteEntete(QTextStream& astreamfile, const sDataTypeEntete &aDataEntete) const {

    astreamfile << "[" << CKeyEvent  << " \"" <<aDataEntete._Event    << "\"]"    <<"\n";
    astreamfile << "[" << CKeySite   << " \"" <<aDataEntete._Site     << "\"]"    <<"\n";
    astreamfile << "[" << CKeyDate   << " \"" <<aDataEntete._Date     << "\"]"    <<"\n";
    astreamfile << "[" << CKeyRound  << " \"" <<aDataEntete._Round    << "\"]"    <<"\n";
    astreamfile << "[" << CKeyWhite  << " \"" <<aDataEntete._White    << "\"]"    <<"\n";
    astreamfile << "[" << CKeyBlack  << " \"" <<aDataEntete._Black    << "\"]"    <<"\n";
    astreamfile << "[" << CKeyResult << " \"" <<aDataEntete._Resultat << "\"]"    <<"\n";
    //facultatif
    if (aDataEntete._WhiteElo.size() > 0)
        astreamfile << "[" << CKeyWhiteElo << " \"" <<aDataEntete._WhiteElo << "\"]"    <<"\n";
    if (aDataEntete._BlackElo.size() > 0)
        astreamfile << "[" << CKeyBlackElo << " \"" <<aDataEntete._BlackElo << "\"]"    <<"\n";
}

bool PGNReaderWriter::TrouveDebutEnTete (int aPosDebutFile, int& aPosDebutEnTete) {

    aPosDebutEnTete   = _DataFile.indexOf("[" + CKeyEvent   + " \"", aPosDebutFile);
    return (aPosDebutEnTete >= 0) ;
}

QString PGNReaderWriter::getDataGame(int aNoGame) {

    if (_DataGame.size() <= 0) {

        int nGame = 0;
        int lPosDebutEntete = -1;
        //trouve debut entete pour le jeu
        while ((nGame < aNoGame) && TrouveDebutEnTete (lPosDebutEntete+1, lPosDebutEntete)) {
            nGame++;
        }
        int lPostNextGame = -1;
        if (TrouveDebutEnTete (lPosDebutEntete+1, lPostNextGame))
            _DataGame = _DataFile.mid(lPosDebutEntete, lPostNextGame - lPosDebutEntete);
        else
            _DataGame = _DataFile.mid(lPosDebutEntete);
    }

    return _DataGame;
}


