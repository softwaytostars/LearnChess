#ifndef CREATORDATANN_H
#define CREATORDATANN_H

#include <QVector>
#include <QTextStream>
#include <QFile>
#include "ChessBoardManager/ILoaderGame.h"
#include "Algo/evaluatorposition.h"

class ChessManager;
class CreatorDataNN : public ILoaderGame
{
public:
    CreatorDataNN();
    ~CreatorDataNN();
    void CreateTrainingFile() const;
    virtual void InitializeGame(bool aCreationPiece);
    virtual void AjouterDemiCoup(CommandPlay* aCommandPlay);
    virtual IChessManager& getChessManager();
private:
    ChessManager* _ChessManager;
    QVector<CommandPlay*> _ListCommandesPlay;
    EvaluatorPosition _EvaluatorPosition;
    int _NbreLinesForTraining;
    int _NbreInputCriteria;
    QTextStream *_TextStream;//le stream du fichier à écrire

    void ViderCommandes ();

    /**
     * @brief Pour entrainer le NN, on doit lui donner la valeur du coup pour la position statique (sans compter sur les coups à venir)
              Un commentaire y, n, ou rien permettra de tagger si le coup est bon, mauvais ou neutre (un échange par ex)
     *
     */
    enum eTypeCoupPourNN {
        eGood,
        eBad,
        eNeutral
    };

    void EcrireDataNN (const tQMapValeursCriteres& aValeursCriteresCoupEtudie,
                       bool aEstleBonCoup,
                       eTypeCoupPourNN aTypeCoupPourNN,
                       float aEstimationBasiqueCoup,
                       float aEstimationBasiqueBonCoup, int aCurrentDemiMove);

};

#endif // CREATORDATANN_H
