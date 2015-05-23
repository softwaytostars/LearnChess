#ifndef IRULEMOVEPIECE_H
#define IRULEMOVEPIECE_H

#include "Data/ArticlePiece.hpp"
#include "IChessManager.h"
#include "Types/TypeInfosMoves.h"

class CommandPlay;
class CommandPlay;

/**
 * @brief Classe interface pour donner les regles de deplacement ou de prise. Les objets en dérivant pourront exister pour plusieurs chessmanager donc pas mis dans constructeur.
 *
 */
class IRuleMovePiece
{
public:
    IRuleMovePiece ();
    bool AllowMove (const IChessManager &aIChessManager,
                    int aOldPosX, int aOldPosY,
                    int aNewPosX, int aNewPosY,
                    const CommandPlay *const aPreviousPlay,
                    bool aVerifIfKingIsCheck=true,
                    bool aDoNotCheckFriendFinalSquare=false,
                    bool ARayonX=false) const;

    /**
     * @brief
     *
     * @param aIChessManager
     * @param aOldPosX
     * @param aOldPosY
     * @param aNewPosX
     * @param aNewPosY
     * @param aCommandPlay : la commande permettant l'execution ou l'undo du play
     * @return bool
     */
    bool DoPlay (IChessManager &aIChessManager,
                 int aOldPosX, int aOldPosY,
                 int aNewPosX, int aNewPosY,
                 CommandPlay *&aCommandPlay,
                 const CommandPlay *const aPreviousPlay);

    virtual bool get_SquaresForMoves (const IChessManager &aIChessManager,
                                      const ArticlePiece &aArticleFrom,
                                      const CommandPlay *const aPreviousPlay,
                                      tQVectorInfosSquare &aListMoves,
                                      bool aCalculDefenseursAttaquants=true,
                                      bool ARayonX=false) const;
protected:
    enum eTypeDirection {
        edirX,
        edirY,
        edirDiag
    };
    virtual bool getDirections(QVector<eTypeDirection>&) const { return false;}
    virtual bool _DoMovement (const IChessManager &aIChessManager,
                              const ArticlePiece& aArticleFrom,
                              int aNewPosX,
                              int aNewPosY,
                              CommandPlay* aCommandPlay,
                              const CommandPlay* const aPreviousPlay,
                              bool aVerifIfKingIsCheck,
                              bool aDoNotCheckFriendFinalSquare,
                              bool ARayonX) const =0;

    /**
     * @brief
     *
     * @param aIChessManager
     * @param aArticleFrom
     * @param aNewPosX
     * @param aNewPosY
     * @param aCommandPlay
     * @param aDoNotCheckFriendFinalSquare
     * @param aRayonX : Si Rayon X permet de traverser piece ennemie pour voir si attaque derriere
     * @return bool
     */
    bool _ActionSurDeplacementDiag (const IChessManager &aIChessManager,
                                    const ArticlePiece &aArticleFrom,
                                    int aNewPosX,
                                    int aNewPosY,
                                    CommandPlay *aCommandPlay,
                                    bool aDoNotCheckFriendFinalSquare,
                                    bool aRayonX) const;
    bool _ActionSurDeplacementVertouHoriz (const IChessManager &aIChessManager,
                                           const ArticlePiece &aArticleFrom,
                                           eTypeDirection aTypeDir,
                                           int aInvariant,
                                           int aDebut,
                                           int aFin,
                                           CommandPlay *aCommandPlay,
                                           bool aDoNotCheckFriendFinalSquare,
                                           bool ARayonX) const;
    void AddPossibleMoves (const IChessManager &aIChessManager,
                           const ArticlePiece& aArticleFrom,
                           int anewPosX, int anewPosY,
                           const CommandPlay* const aPreviousPlay,
                           tQVectorInfosSquare& aListMoves,
                           bool aCalculDefenseursAttaquants,
                           bool ARayonX) const;

private:
    bool _ConditionMinimumRespectee (const IChessManager &aIChessManager,
                                     bool aDoNotCheckFriendFinalSquare,
                                     int aOldPosX, int aOldPosY,
                                     int anewPosX, int anewPosY, ArticlePiece &aArticle) const;
    bool _PasseCritereEvaluationRapide (const ArticlePiece& aArticleFrom, const CommandPlay* const aPreviousPlay, int aNewPosX, int aNewPosY) const;

};

#endif // IRULEMOVEPIECE_H
