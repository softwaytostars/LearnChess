#ifndef ICHESSMANAGER_H
#define ICHESSMANAGER_H

#include "Data/ArticlePiece.hpp"
#include <vector>
#include "Types/TypeInfosMoves.h"

/**
 * @brief Classe interface pour ChessManager
 *
 */
class CommandPlay;

class IChessManager {
public:
    virtual bool UnePieceExiste (int aPosX, int aPosY, ArticlePiece &aArticlePiece) const = 0;
    virtual bool ModifierPlacePiece (const ArticlePiece& aArticleFrom,
                                     int anewPosX,
                                     int anewPosY,
                                     bool aHasEverMoved=true) = 0;
    virtual bool AllowedMove (int aOldPosX, int aOldPosY,
                              int aNewPosX, int aNewPosY,
                              const CommandPlay* const aPreviousPlay,
                              bool aVerifIfKingIsCheck=true,
                              bool aDoNotCheckFriendFinalSquare=false) const= 0;
    virtual bool getArticlesPiecesDeCouleur (eColorPiece aColorPiece, tlisteArticlesPieces& aListeArticles) const =0;
    virtual bool ForcerAjoutPiece (eTypePiece aTypePiece,
                                   eColorPiece aColor,
                                   int aPosX, int aPosY,
                                   bool aHasEverMoved) = 0;
    virtual bool KillPiece (const ArticlePiece& aArticle) = 0;
    virtual bool ColorIsUnderAttackForPosition(eColorPiece aColor,
                                        int aPosX, int aPosY,
                                        tlisteArticlesPieces& alistePiecesEnnemies,
                                        const tQVectorTypePiece& aTypesPiecesConsiderees=tQVectorTypePiece()) const = 0;
    virtual bool KingIsAttacked (eColorPiece aColor,
                                 tlisteArticlesPieces& alistePiecesEnnemies,
                                 bool aStopFirstOne=false) const =0;
    virtual bool KingIsAttacked (eColorPiece aColor) const =0;
    virtual bool findPieceAtPosThatCanMoveTo (int aPosXdepart,
                                              int aPosYdepart,
                                              eColorPiece aColor,
                                              eTypePiece  aTypePiece,
                                              int aPosX,
                                              int aPosY,
                                              ArticlePiece& aArticle) const = 0;
    virtual bool findPieceThatCanMoveTo (eColorPiece aColor,
                                              eTypePiece  aTypePiece,
                                              int aPosX,
                                              int aPosY,
                                              ArticlePiece& aArticle) const = 0;
    virtual bool findAllPiecesThatCanMoveTo (eColorPiece aColor,
                                                eTypePiece  aTypePiece,
                                                int aPosX,
                                                int aPosY,
                                                tlisteArticlesPieces& aPieces) const =0;
    virtual int GetXPos (const char ACol) const=0;
    virtual char GetNomXPos (int aPos, bool Upper=true) const = 0;
    virtual bool CastlingIsPossibleForKing (const ArticlePiece& aKing,
                                     int aPosX,
                                     int aPosY,
                                     sDiagnostiqueCastling& aDiagnostiqueCastling) const= 0;
    virtual void DesActiverNotification (bool adesactiver)=0;
    virtual bool NotificationsSontDesactivees () const =0;
    virtual sInfosCastling getInfosSmallCastling () const=0;
    virtual sInfosCastling getInfosBigCastling () const=0;
    virtual bool KingIsCastled (eColorPiece aColor) const =0;
    virtual void SetKingIsCastled (eColorPiece aColor, bool aIsCastled) = 0;

    inline static eColorPiece get_ColorEnnemy (eColorPiece aColor)  {

       if (aColor == WHITE)
           return BLACK;
        else
           return WHITE;
   }

    /**
     * @brief Position Y initiale du roi
     *
     * @param aColor
     * @return int
     */
    inline static int get_PosYKingBoard (eColorPiece aColor)  {
        if (aColor == BLACK) return 8;
        else return 1;
    }

    virtual bool AskPiecePromotionIsForbiddenFor (eColorPiece aColor) const = 0;

};

#endif // ICHESSMANAGER_H
