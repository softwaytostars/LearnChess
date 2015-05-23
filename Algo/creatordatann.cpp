#include "creatordatann.h"

#include <QDir>
#include <QFile>
#include <QtGlobal>
#include "ChessBoardManager/commandplay.h"
#include "Types/TypeInfosMoves.h"
#include "ChessBoardManager/chessmanager.h"
#include "ImportExport/pgnreaderwriter.h"
#include "infosconfiguration.h"
//#include "floatfann.h"
#include "exceptionsInfos.h"

#ifdef _DEBUG
#include <QDebug>
#endif

CreatorDataNN::CreatorDataNN():
     _ChessManager (new ChessManager(false)),
    _EvaluatorPosition(EvaluatorPosition (*_ChessManager)),
    _NbreLinesForTraining (0),
    _NbreInputCriteria (0)
{

    _ChessManager->DesActiverNotification(true);
    _EvaluatorPosition.setUseNN(false);

    if (QFile::exists("TrainingNN.data")) return;

    QFile file ("TrainingNN.data");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream lTextStream (&file);
    _TextStream = &lTextStream;

    QString FormatFirstLine = QString ("10000 7 1 ");
    *_TextStream << FormatFirstLine;

    //prend le répertoire contenant les fichiers PGN
    //QDir dir (InfosConfiguration::instance()->getLastDirForPGNFiles());
    QDir dir ("/home/alexis/PGNFileForTrainingNN");
    dir.setFilter(QDir::Files);
    QStringList filters;
    filters << "*.PGN" << "*.pgn";
    //filtre pour prendre que les fichiers PGN
    dir.setNameFilters(filters);
    QFileInfoList list = dir.entryInfoList();

    //parcourt tous les fichiers PGN pour les lire
    for (int i=0; i < list.size(); i++) {

        QFileInfo fileInfo = list.at(i);
        PGNReaderWriter pgn (fileInfo.absoluteFilePath());
        //charger le jeu par l'intermediaire de ILoaderGame
        pgn.LoadGame(*this);

#ifdef _DEBUG
    qDebug()<<fileInfo.absoluteFilePath();
#endif

    }


    *_TextStream << '\r';

    //remplacer la premiere ligne
    _TextStream->seek(0);
    QString newFormatFirstLine = QString("%1 ").arg(_NbreLinesForTraining)
                                  + QString("%1 ").arg(_NbreInputCriteria)
                                  + QString("%1").arg(1);
    *_TextStream << newFormatFirstLine;
    int lSize = newFormatFirstLine.size();
    while (lSize < (FormatFirstLine.size()-1)) {
        *_TextStream << " ";
        lSize += 1;
    }

    file.close();
}

CreatorDataNN::~CreatorDataNN() {

    ViderCommandes();
    if (_ChessManager)
        delete _ChessManager;
}

void CreatorDataNN::CreateTrainingFile() const {


    int lNbreInputCriteria = 8;//todo lire dans le fichier

    /*** training �  partir des data ***/

    const unsigned int num_layers = 3;
    const unsigned int num_hiddenNeurons = 25;//100
    const float desired_error = (const float) 0.008;
    const unsigned int max_epochs = 100;
    const unsigned int epochs_between_reports = 100;

//    struct fann *ann = fann_create_standard (num_layers, lNbreInputCriteria, num_hiddenNeurons, 1);
//    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
//    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
//    fann_train_on_file(ann, "TrainingNN.data", max_epochs,
//                      epochs_between_reports, desired_error);
//    fann_save(ann, "TrainingNN.net");
//    fann_destroy(ann);
}

void CreatorDataNN::ViderCommandes () {

   for (QVector<CommandPlay*>::const_iterator it=_ListCommandesPlay.begin();
        it != _ListCommandesPlay.end();
        ++it) {
       if (*it)
         delete (*it);
   }

   _ListCommandesPlay.clear();

}

void CreatorDataNN::InitializeGame(bool aCreationPiece) {

    //un nouveau jeu est cree
    ViderCommandes();
    _ChessManager->ResetPieces();
    if (aCreationPiece)
        _ChessManager->CreationDesPieces();
}

void CreatorDataNN::AjouterDemiCoup (CommandPlay* aCommandPlay) {

    if (!aCommandPlay) return;

    //evaluation de la position
    float lEstimationBasiqueBonCoup = _EvaluatorPosition.EstimationPosition(_ListCommandesPlay.count()+1,
                                                                            aCommandPlay->getColor(),
                                                                            *aCommandPlay);

    //enleve le coup pour voir la situation avant
    aCommandPlay->UnExecute();

    eTypeCoupPourNN lTypeCoupPourNN = eNeutral;
    if (aCommandPlay->getCommentary().contains("y"))
      lTypeCoupPourNN = eGood;
    else if (aCommandPlay->getCommentary().contains("n"))
      lTypeCoupPourNN = eBad;


    try {
        tlisteArticlesPieces lListePieces;
        //avant chaque nouveau coup, trouve tous les coups qui étaient possibles
        if (!_ChessManager->getArticlesPiecesDeCouleur(aCommandPlay->getColor(), lListePieces)) throw Exceptions ("pas d'articles!");

        int lOldPosXBonCoup, lOldPosYBonCoup, lNewPosXBonCoup, lNewPosYBonCoup;
        if (!aCommandPlay->GetInfosPieceDeplacee (lOldPosXBonCoup,
                                                  lOldPosYBonCoup,
                                                  lNewPosXBonCoup,
                                                  lNewPosYBonCoup)) {

            throw Exceptions ("pas d'infos sur commande!");
        }

#ifdef _DEBUG
    qDebug()<<"LE BON COUP EST"<<lOldPosXBonCoup<< " "<<lOldPosYBonCoup<<" TO "<<lNewPosXBonCoup<<" "<<lNewPosYBonCoup;
    qDebug()<<"Currentmove ="<<_ListCommandesPlay.count()+1;
#endif


        tQVectorInfosSquare  lListSquares;
        tQMapValeursCriteres lValeursCriteresCoupEtudie;
        int lOldPosXCoup, lOldPosYCoup, lNewPosXCoup, lNewPosYCoup;
        CommandPlay* lCommandPlay;


        CommandPlay* lPreviousCommandPlay = 0;
        if (_ListCommandesPlay.count() > 0)
            lPreviousCommandPlay = _ListCommandesPlay [_ListCommandesPlay.count()-1];


        //parcourt toustes les pieces encore presentes
        for (tlisteArticlesPieces::const_iterator itPiece = lListePieces.begin();
             itPiece!= lListePieces.end();
             ++itPiece) {
            //cherche les moves possibles pour la piece
            lListSquares.clear();
            if (_ChessManager->get_SquaresForMoves (*itPiece,
                                                    lPreviousCommandPlay,
                                                    lListSquares,
                                                    false)) {

                //parcorut tous les moves et les evalue
                for (tQVectorInfosSquare::const_iterator itSquare = lListSquares.begin();
                     itSquare != lListSquares.end();
                     ++itSquare) {

                    lOldPosXCoup = (*itPiece)._PosX;
                    lOldPosYCoup = (*itPiece)._PosY;
                    lNewPosXCoup = (*itSquare)._PosX;
                    lNewPosYCoup = (*itSquare)._PosY;

                    if (_ChessManager->Play(lOldPosXCoup, lOldPosYCoup,
                                            lNewPosXCoup, lNewPosYCoup,
                                            lCommandPlay, lPreviousCommandPlay) && lCommandPlay) {


                        float lEstimationBasiqueCoup = _EvaluatorPosition.EstimationPosition(_ListCommandesPlay.count(),
                                                                                             lCommandPlay->getColor(),
                                                                                             *lCommandPlay,
                                                                                             lValeursCriteresCoupEtudie);
                        bool lEstleBonCoup  = (lOldPosXCoup == lOldPosXBonCoup) && (lOldPosYCoup == lOldPosYBonCoup)
                                              && (lNewPosXCoup == lNewPosXBonCoup) && (lNewPosYCoup == lNewPosYBonCoup);
                        EcrireDataNN (lValeursCriteresCoupEtudie,
                                      lEstleBonCoup,
                                      lTypeCoupPourNN,
                                      lEstimationBasiqueCoup,
                                      lEstimationBasiqueBonCoup,
                                      _ListCommandesPlay.count());
#ifdef _DEBUG
    qDebug()<<"COMPARE "<<100.0*(lEstimationBasiqueBonCoup - lEstimationBasiqueCoup)/lEstimationBasiqueBonCoup<< " %";
#endif
                        lCommandPlay->UnExecute();
                        delete lCommandPlay;
                        lCommandPlay = 0;
                    }//if play
                }//for itSquares

            }//if (_ChessManager->get_SquaresForMoves
         }//for (tlisteArticlesPieces::const_iterator

    }//try
    catch (const std::exception &e) {

#ifdef _DEBUG
    qDebug()<<e.what();
#endif
    }

    catch(...) {

#ifdef _DEBUG
    qDebug()<<"Pb generation test";
#endif

    }

    //ajoute le coup
    _ListCommandesPlay.push_back(aCommandPlay);
    //reexecute le vrai coup
    aCommandPlay->Execute();

}

void CreatorDataNN::EcrireDataNN (const tQMapValeursCriteres &aValeursCriteresCoupEtudie,
                                  bool aEstleBonCoup,
                                  eTypeCoupPourNN aTypeCoupPourNN,
                                  float aEstimationBasiqueCoup,
                                  float aEstimationBasiqueBonCoup,
                                  int aCurrentDemiMove){

    if ((aEstimationBasiqueBonCoup) < 0) return;

    bool lMauvaisCoup  = (aEstimationBasiqueBonCoup -aEstimationBasiqueCoup) > (30./100)*aEstimationBasiqueBonCoup;
    bool lBonCoup      = aEstleBonCoup;

    bool lCoupAPrendre = lBonCoup || lMauvaisCoup;
    if (!lCoupAPrendre) return;

     *_TextStream << "\n";
    _NbreLinesForTraining += 1;
   _NbreInputCriteria     = aValeursCriteresCoupEtudie.count()+1;// car on va ajouter la valeur de aCurrentDemiMove
   for (tQMapValeursCriteres::const_iterator it = aValeursCriteresCoupEtudie.begin();
        it != aValeursCriteresCoupEtudie.end();
        ++it) {
       *_TextStream << it.value() << " ";
   }
   *_TextStream << aCurrentDemiMove;
   //saute une ligne après les critères
   *_TextStream << "\n";
   if (lBonCoup) {
       switch (aTypeCoupPourNN) {
            case eGood    : *_TextStream << "1 "; break;
            case eBad     : *_TextStream << "-1 "; break;
            case eNeutral : *_TextStream << "0 "; break;
            default : break;
       }
   }
   else {
       if (lMauvaisCoup)
           *_TextStream << "-1 ";
   }

}

IChessManager &CreatorDataNN::getChessManager(){

    return dynamic_cast<IChessManager&>(*_ChessManager);

}
