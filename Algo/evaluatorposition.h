#ifndef EVALUATORPOSITION_H
#define EVALUATORPOSITION_H

#include "TypePiece.hpp"
#include "Data/ArticlePiece.hpp"
#include "Types/TypeInfosAlgo.h"
#include "Types/TypeInfosMoves.h"
#include <QMap>

class ChessManager;
class CommandPlay;

enum eCriteresAlgo {
    ePoidsMateriel,
    ePoidsMobilite,
    ePoidsDev,
    ePoidsControleCases,
    eSafetyPieces,
    eAgressivitePieces,
    ePoidsControleCentre
};
typedef QMap<eCriteresAlgo, float> tQMapValeursCriteres;

class EvaluatorPosition
{
public:
    EvaluatorPosition(ChessManager& aChessManager);
    float EstimationPosition (int acurrentDemiMove,
                              eColorPiece aColor,
                              const CommandPlay& aCommandeJouee) const;
    float EstimationPosition (int acurrentDemiMove,
                              eColorPiece aColor,
                              const CommandPlay& aCommandeJouee,
                              tQMapValeursCriteres& aValeursCriteres) const;
    void setUseNN (bool aUseNN) {_UseNN = aUseNN;}

private:
    ChessManager& _ChessManager;//devrait etre passé en const mais SimulationPlayOk!!!

    bool _UseNN;

    struct sPieceEvaluee {
        ArticlePiece _Piece;
        float _ValeurPiece;//poids associé au mouvement de la piece
        sPieceEvaluee () :
            _Piece(ArticlePiece()),
            _ValeurPiece (0)
        {}
        sPieceEvaluee (const ArticlePiece& aPiece,
                       float aValeurPiece) :
            _Piece(aPiece),
            _ValeurPiece (aValeurPiece)
        {}
    };
    struct sPieceEvalueeValeursCroissantes {
        bool operator() (const sPieceEvaluee& aPiece1, const sPieceEvaluee& aPiece2) {
            if (aPiece1._Piece._TypePiece == King) return false;
            if (aPiece2._Piece._TypePiece == King) return true;
            return (aPiece1._ValeurPiece < aPiece2._ValeurPiece);
        }
    };

    typedef QVector<sPieceEvaluee> tPieceEvalueeVector;

    void EvaluationPositionStatique(int acurrentDemiMove,
                                    eColorPiece aColor,
                                    const CommandPlay& aCommandeJouee,
                                    tQMapValeursCriteres& aValeursCriteres) const;
    float EstimationNN (const tQMapValeursCriteres& aValeursCriteres, int acurrentDemiMove) const;
    float EstimationBasique (const tQMapValeursCriteres& aValeursCriteres) const;

    /****méthodes d'évaluations critères ****/

    qreal CalculPoidsDistanceAuCentre (int anewPosX, int anewPosY) const;

    float CalculerValeurMaterielPiece (const ArticlePiece &aPiece, int aCurrentDemiMove) const;
    int EvaluerMobiliteStatiquePiece (const ArticlePiece& aPiece,
                                      int aCurrentDemiMove,
                                      const CommandPlay &aCommandeJouee,
                                      QMap<int, tQVectorInfosSquare>& aMapPosPieceListePossibleSquares) const;
    int EvaluerDeveloppementStatiquePiece (const ArticlePiece& aPiece,
                                           int aCurrentDemiMove) const;
    float EvaluerSafetyPieceStatique(int aCurrentDemiMove,
                                    const ArticlePiece &aPiece,
                                    const CommandPlay &aCommandeJouee,
                                    const QMap<int, tPieceEvalueeVector>& aMapPiecesDefenseurs,
                                    const QMap<int, tPieceEvalueeVector>& aMapPiecesAttaquants,
                                    QMap<int, tQVectorInfosSquare>& aMapPosPieceListePossibleSquares,
                                    bool &aEchangeEstFavorable) const;
    int EvaluerAgressiviteStatiquePiece (const ArticlePiece& aPiece,
                                         int aCurrentDemiMove,
                                         const CommandPlay &aCommandeJouee,
                                         const QMap<int, tPieceEvalueeVector> &aMapPiecesDefenseurs,
                                         const QMap<int, tPieceEvalueeVector> &aMapPiecesAttaquants,
                                         const bool aEchangeEstFavorable,
                                         QMap<int, tQVectorInfosSquare>& aMapPosPieceListePossibleSquares) const;
    int getPoidsControleCentre (eColorPiece aColor,
                                int aPosX, int aPosY,
                                int aCurrentDemiMove,
                                QMap<int, tPieceEvalueeVector>& aMapPosDefenseurs, QMap<int, tPieceEvalueeVector> &aMapPosAttaquants) const;
    void CalculerDefenseursAttaquantsDeCase(eColorPiece aColor,
                                            int aPosX, int aPosY,
                                            int aCurrentDemiMove,
                                              QMap<int, tPieceEvalueeVector> &aMapPosDefenseurs,
                                              QMap<int, tPieceEvalueeVector> &aMapPosAttaquants) const;
    int PenalisationBadBishop (eColorPiece aColor, int aPosX, int aPosY) const;
    bool EstUnePieceDeveloppee (const ArticlePiece& aPiece) const;
    bool getListePossibleSquares (const ArticlePiece& aPiece,
                                  const CommandPlay &aCommandeJouee,
                                  QMap<int, tQVectorInfosSquare>& aMapPosPieceListePossibleSquares,
                                  tQVectorInfosSquare& aListSquares) const;
    bool PawnIsPassed (const ArticlePiece& aPawn, const tlisteArticlesPieces& alistePawnsEnnemies) const;

    int DistancePawnFromPromotion (const ArticlePiece& aPawn) const;

    bool CouleurPeutMenacerMatDuCouloir (eColorPiece aColor,
                                        const CommandPlay &aLastCommand) const;
    int PoidsSiEnfilade (const ArticlePiece& aPiece,
                         int aCurrentDemiMove,
                         const CommandPlay &aCommandeJouee,
                         const QMap<int, tPieceEvalueeVector> &aMapPiecesDefenseurs) const;
    int CalculPoidsSurPiecesEnfilees (const tPieceEvalueeVector& aPiecesEnfilees,
                                      const ArticlePiece& aPiece,
                                      const int aValeurPiece,
                                      const QMap<int, tPieceEvalueeVector> &aMapPiecesDefenseurs) const;
    bool EnnemyWillBeMated (const CommandPlay &aCommandeJouee,
                            int aIter=0) const;
    bool AMoveLeadToMate (const CommandPlay &aEnnemyCommandMove, int aIter) const;
    bool AllMovesFromKingLeadToMate (const CommandPlay &aCommandeJouee,
                                     const ArticlePiece& aKingEnnemy,
                                     const tQVectorInfosSquare& aListSquaresKing,
                                     int aIter) const;
    bool IlExistePiecesAInterposerProtegees (const CommandPlay &aPreviousPlay,
                                             const ArticlePiece& aKingEnnemy,
                                             const ArticlePiece& aPieceDonnantEchec) const;
    bool FourchetteDePionPossible (const ArticlePiece& aPawn) const;
    bool EnnemyCanMateAfterMove (const CommandPlay &aCommandeJouee) const;

};

#endif // EVALUATORPOSITION_H
