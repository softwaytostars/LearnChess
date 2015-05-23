#ifndef CHESSMANAGER_H
#define CHESSMANAGER_H


#include <list>
#include "Data/tablepiece.h"
#include "ChessBoardManager/IChessManager.h"
#include "Types/TypeInfosMoves.h"

class CommandPlay;

/**
 * @brief Classe contenant l'intelligence du jeu :  gère les pièces
 *
 */
class ObserverChessManager;

class ChessManager : public IChessManager
{
public:
    ChessManager (bool aAvecInit);
    ~ChessManager ();
    ChessManager(const tlisteArticlesPieces& aListesPieces);
    void RegisterObserver (ObserverChessManager &aObserver);


    /**
     * @brief Pour recreer les pieces
     *
     */
    void CreationDesPieces();

    /**
     * @brief Méthode pour changer le jeu de données des pièces
     *
     * @param aListesPieces
     */
    void ResetPieces (const tlisteArticlesPieces& aListesPieces = tlisteArticlesPieces());


    /**
     * @brief Pour retirer une pièce des donnees
     *
     * @param aPosX
     * @param aPosY
     * @return bool
     */
    bool Remove (int aPosX, int aPosY);

    /**
     * @brief Pour jouer la piece
     *
     * @param aOldPosX
     * @param aOldPosY
     * @param aNewPosX
     * @param aNewPosY
     * @return bool
     */
    bool Play (int aOldPosX,
               int aOldPosY,
               int aNewPosX,
               int aNewPosY,
               CommandPlay *&aCommandPlay,
               const CommandPlay *const aPreviousPlay);
    bool SimulationPlayOk (int aOldPosX,
                           int aOldPosY,
                           int aNewPosX,
                           int aNewPosY);

    //interface IChessManager
    virtual bool UnePieceExiste (int aPosX, int aPosY, ArticlePiece &aArticlePiece) const;
    virtual bool ModifierPlacePiece (const ArticlePiece& aArticleFrom,
                                     int anewPosX,
                                     int anewPosY,
                                     bool aHasEverMoved=true);//derniers champs pour le redo
    virtual bool AllowedMove (int aOldPosX, int aOldPosY,
                              int aNewPosX, int aNewPosY,
                              const CommandPlay *const aPreviousPlay,
                              bool aVerifIfKingIsCheck=true,
                              bool aDoNotCheckFriendFinalSquare=false) const;
    virtual bool getArticlesPiecesDeCouleur (eColorPiece aColorPiece, tlisteArticlesPieces& aListeArticles) const;
    virtual bool ForcerAjoutPiece (eTypePiece aTypePiece,
                                   eColorPiece aColor,
                                   int aPosX, int aPosY,
                                   bool aHasEverMoved);
    virtual bool KillPiece (const ArticlePiece& aArticle);
    virtual bool ColorIsUnderAttackForPosition(eColorPiece aColor,
                                       int aPosX, int aPosY,
                                       tlisteArticlesPieces &alistePiecesEnnemies,
                                       const tQVectorTypePiece& aTypesPiecesConsiderees=tQVectorTypePiece()) const;
    virtual bool KingIsAttacked (eColorPiece aColor,
                                 tlisteArticlesPieces& alistePiecesEnnemies,
                                 bool aStopFirstOne=false) const;
    virtual bool KingIsAttacked (eColorPiece aColor) const;
    virtual bool findPieceAtPosThatCanMoveTo (int aPosXdepart,
                                              int aPosYdepart,
                                              eColorPiece aColor,
                                              eTypePiece  aTypePiece,
                                              int aPosX,
                                              int aPosY,
                                              ArticlePiece& aArticle) const;
   virtual bool findPieceThatCanMoveTo (eColorPiece aColor,
                                              eTypePiece  aTypePiece,
                                              int aPosX,
                                              int aPosY,
                                              ArticlePiece& aArticle) const;
   virtual bool findAllPiecesThatCanMoveTo (eColorPiece aColor,
                                               eTypePiece  aTypePiece,
                                               int aPosX,
                                               int aPosY,
                                               tlisteArticlesPieces& aPieces) const;
   virtual int GetXPos (const char ACol) const;
   virtual char GetNomXPos (int aPos, bool aUpper=true) const;
   virtual bool CastlingIsPossibleForKing (const ArticlePiece& aKing,
                                    int aPosX,
                                    int aPosY,
                                    sDiagnostiqueCastling &aDiagnostiqueCastling) const;
   virtual sInfosCastling getInfosSmallCastling () const;
   virtual sInfosCastling getInfosBigCastling () const;
   virtual bool KingIsCastled (eColorPiece aColor) const {

        if (aColor == WHITE)
            return _WhiteKingIsCastled;
        else
            return _BlackKingIsCastled;
   }
   virtual void SetKingIsCastled (eColorPiece aColor, bool aIsCastled) {

        if (aColor == WHITE)
             _WhiteKingIsCastled = aIsCastled;
         else
             _BlackKingIsCastled = aIsCastled;
    }

   virtual void DesActiverNotification (bool adesactiver) {
        _DesactiverNotifications = adesactiver;
    }

   virtual bool NotificationsSontDesactivees () const {return _DesactiverNotifications;}

   virtual bool AskPiecePromotionIsForbiddenFor (eColorPiece aColor) const {return _ColorsForbiddenForAskPromotion.contains(aColor);}

   void setInterdireAskPiecePromotionFor (const tQVectorColorPiece& aColorsWithoutAsk);

    /**
     * @brief Pour permettre de ne pas tout vérifier  (pour configuration training)
     *
     * @param aModePersmissif
     */
    void setModePermissif (bool aModePersmissif) {_ModePermissif = aModePersmissif;}

    bool get_SquaresForMoves (const ArticlePiece& aPiece,
                              const CommandPlay *const aPreviousPlay,
                              tQVectorInfosSquare & aListSquares,
                              bool aCalculDefenseursAttaquants = true,
                              bool aRayonX=false) const;

    bool findPieces (eColorPiece aColor,
                     eTypePiece  aTypePiece,
                     tlisteArticlesPieces &alistePieces) const;

    bool RooksAreAutoProtected (eColorPiece aColor, bool &aAxeHoriz) const;
    bool RooksAreAutoProtected (int aPosX1, int aPosY1,
                                int aPosX2, int aPosY2,
                                bool &aAxeHoriz) const ;
    bool PieceAttacksThePosition (const ArticlePiece& aPiece, int aPosX, int aPosY) const;
    bool getKing (eColorPiece aColorPiece, ArticlePiece& aKing) const;

    static int qHashKey (int aPosX, int aPosY);

    inline void setAllowFirstPlayBlack (bool aAllow) { _ModeAllowFirstPlayBlack = aAllow;}
    bool PieceEstClouee(const ArticlePiece& aPiece, ArticlePiece& agetPieceEnnemy) const;

private:
    ChessManager();
    ChessManager (const ChessManager& acopy);
    ChessManager& operator=(const ChessManager& acopy);
    TablePiece _tableDataPieces; /**< table de  */
    std::list<ObserverChessManager*> _Observers; /**< liste des observateurs */
    bool _ModePermissif;
    bool _DesactiverNotifications;
    bool _WhiteKingIsCastled, _BlackKingIsCastled;
    tQVectorColorPiece _ColorsForbiddenForAskPromotion;
    bool _ModeAllowFirstPlayBlack;
    void CreationPiece (eTypePiece aTypePiece, eColorPiece aColor, int aPosX, int aPosY,  bool aHasEverMoved=false);
    void NotifierResetPieces ();
    void NotifierAjoutPiece (eTypePiece aTypePiece, eColorPiece aColor, int aPosX, int aPosY);
    void NotifierModificationPlace (eTypePiece aGenrePiece, eColorPiece aColor, int aOldPosX, int aOldPosY, int anewPosX, int anewPosY);
    void NotifierKillPiece (eTypePiece aGenrePiece, eColorPiece aColorPiece,
                            int aPosX, int aPosY);
    void NotifierPlay (CommandPlay *) const;
    void Init (const tlisteArticlesPieces &aListesPieces, bool aAvecInit=true);
    bool ConditionCouleurAJouerRespectee (const CommandPlay* const aPreviousPlay,
                                          eColorPiece aColorToPlay) const;
    void getPiecesThatCanMoveTo (int aPosXdepart,
                                 int aPosYdepart,
                                 eColorPiece aColor,
                                 eTypePiece  aTypePiece,
                                 int aPosX,
                                 int aPosY,
                                 tlisteArticlesPieces& aListPieces,
                                 bool aStopForTheFirstOne) const;

    class DirectionsSearchPinningPiece {

        private:
            const ChessManager& _ChessManager;
            QList<eTypePiece> _TypePiecesGivingCheck;
        protected:
            ArticlePiece _King;
            ArticlePiece _PieceEvaluated;
            void AjouterTypePieceGivingCheck (eTypePiece aType) {_TypePiecesGivingCheck.append(aType);}
            virtual int getStartingPos() const =0;
            virtual int getLimPosKing() const =0;
            virtual int getOrient() const =0;
            virtual int getXToSearch(int aPos) const =0;
            virtual int getYToSearch(int aPos) const =0;
        public:
            DirectionsSearchPinningPiece (const ChessManager& aChessManager,
                                          const ArticlePiece& aKing,
                                          const ArticlePiece& aPieceEvaluated)
                : _ChessManager (aChessManager),
                  _King(aKing),
                  _PieceEvaluated(aPieceEvaluated) {}

            static DirectionsSearchPinningPiece* FournirDirectionSearch (const ChessManager& aChessManager,
                                                                         const ArticlePiece& aKing,
                                                                         const ArticlePiece& aPieceEvaluated) {

                DirectionsSearchPinningPiece *lSearch = 0;

                if (aPieceEvaluated._PosX == aKing._PosX) {//vert

                    if ((aPieceEvaluated._TypePiece != Rook) && (aPieceEvaluated._TypePiece != Queen))
                        return 0;
                    lSearch = new DirectionsSearchVertPinningPiece (aChessManager, aKing, aPieceEvaluated);

                } else if (aPieceEvaluated._PosY == aKing._PosY) {//Horiz

                    if ((aPieceEvaluated._TypePiece != Rook) && (aPieceEvaluated._TypePiece != Queen))
                        return 0;
                    lSearch = new DirectionsSearchHorizPinningPiece (aChessManager, aKing, aPieceEvaluated);

                } else {//diag (verifie qd meme)

                    if ((aPieceEvaluated._TypePiece != Bishop)
                            && (aPieceEvaluated._TypePiece != Queen))
                        return 0;
                    float lPente = 1.0*(aPieceEvaluated._PosY-aKing._PosY)/(aPieceEvaluated._PosX-aKing._PosX);
                    if ((qAbs<float> (lPente)) != 1.0) return 0;
                    lSearch = new DirectionsSearchDiagPinningPiece (aChessManager, aKing, aPieceEvaluated);
                }

                return lSearch;

            }

            inline bool UnePieceExisteEntreLeRoiEtPieceEvaluee () const {

                int lPos    = getStartingPos();
                int lPosMax = qMin<int> (8, getLimPosKing ());
                int lOrient = -getOrient ();
                if (lOrient < 0)
                   lPosMax = qMax<int> (1, getLimPosKing ());

                lPos += lOrient;
                while ((lPos != lPosMax) && (lPos>=1) && (lPos<=8)) {


                    int lPosX = getXToSearch (lPos);
                    int lPosY = getYToSearch (lPos);
                     ArticlePiece lPiece;
                    if (_ChessManager.UnePieceExiste(lPosX, lPosY, lPiece))
                        return true;
                    lPos += lOrient;

                }

                return false;

            }
            inline bool UnePieceEnnemieCloueLaPieceEvalue(ArticlePiece& agetPieceEnnemy) const {

                if (_TypePiecesGivingCheck.count() <=0 ) return false;

                eColorPiece lColorOpposed = _ChessManager.get_ColorEnnemy(_PieceEvaluated._Color);



                //si il existe une piece entre le Roi et la piece evaluee, cette derniere n'est pas clouee
                if (UnePieceExisteEntreLeRoiEtPieceEvaluee ()) return false;


                //si il existe une piece ennemie derriere la piece consideree pouvant donner echec, la piece est clouee
                int lPos    = getStartingPos();
                int lPosMax = 8;
                int lOrient = getOrient ();
                if (lOrient < 0)
                   lPosMax = 1;

                lPos += lOrient;
                while ((lPos != lPosMax) && (lPos >=1) && (lPos<=8)) {

                    int lPosX = getXToSearch (lPos);
                    int lPosY = getYToSearch (lPos);

                    ArticlePiece lPiece;
                    //s'arrete des qu'une piece trouvee
                    if (_ChessManager.UnePieceExiste(lPosX, lPosY, lPiece)) {

                        if (_TypePiecesGivingCheck.contains(lPiece._TypePiece)
                                && (lPiece._Color == lColorOpposed)) {

                            agetPieceEnnemy = lPiece;
                            return true;
                        }

                    }

                    lPos += lOrient;
                }

                return false;
            }
    };

    class DirectionsSearchHorizPinningPiece : public DirectionsSearchPinningPiece {

        protected:
            inline virtual int getStartingPos() const {return _PieceEvaluated._PosX;}
            inline virtual int getLimPosKing()  const {return _King._PosX;}
            inline virtual int getOrient()      const {return ((_PieceEvaluated._PosX - _King._PosX)/qAbs<int>((_PieceEvaluated._PosX - _King._PosX)));}
            inline virtual int getXToSearch(int aPos) const { return aPos;}
            inline virtual int getYToSearch(int ) const { return _PieceEvaluated._PosY;}

        public :
            DirectionsSearchHorizPinningPiece (const ChessManager& aChessManager,
                                              const ArticlePiece& aKing,
                                              const ArticlePiece& aPieceEvaluated)
                : DirectionsSearchPinningPiece (aChessManager, aKing, aPieceEvaluated) {
                AjouterTypePieceGivingCheck (Rook);
                AjouterTypePieceGivingCheck (Queen);
            }

    };

    class DirectionsSearchVertPinningPiece : public DirectionsSearchPinningPiece {

        protected:
            inline virtual int getStartingPos() const {return _PieceEvaluated._PosY;}
            inline virtual int getLimPosKing()  const {return _King._PosY;}
            inline virtual int getOrient()      const {return ((_PieceEvaluated._PosY - _King._PosY)/qAbs<int>((_PieceEvaluated._PosY - _King._PosY)));}
            inline virtual int getXToSearch(int ) const { return _PieceEvaluated._PosX;}
            inline virtual int getYToSearch(int aPos) const { return aPos;}


        public :
            DirectionsSearchVertPinningPiece (const ChessManager& aChessManager,
                                              const ArticlePiece& aKing,
                                              const ArticlePiece& aPieceEvaluated)
                : DirectionsSearchPinningPiece (aChessManager, aKing, aPieceEvaluated) {

                AjouterTypePieceGivingCheck (Rook);
                AjouterTypePieceGivingCheck (Queen);
            }


    };

    class DirectionsSearchDiagPinningPiece : public DirectionsSearchPinningPiece {

        private :
            int _Pente;
        protected:
            inline virtual int getStartingPos() const {return _PieceEvaluated._PosX;}
            inline virtual int getLimPosKing()  const {return _King._PosX;}
            inline virtual int getOrient()      const {return ((_PieceEvaluated._PosX - _King._PosX)/qAbs<int>((_PieceEvaluated._PosX - _King._PosX)));}
            inline virtual int getXToSearch(int aPos) const { return aPos;}
            inline virtual int getYToSearch(int aPos) const { return _Pente * (aPos-_PieceEvaluated._PosX) + _PieceEvaluated._PosY;}

        public :
            DirectionsSearchDiagPinningPiece (const ChessManager& aChessManager,
                                              const ArticlePiece& aKing,
                                              const ArticlePiece& aPieceEvaluated)
                : DirectionsSearchPinningPiece (aChessManager, aKing, aPieceEvaluated) {

                _Pente = (_PieceEvaluated._PosY-_King._PosY)/(_PieceEvaluated._PosX-_King._PosX);
                AjouterTypePieceGivingCheck (Bishop);
                AjouterTypePieceGivingCheck (Queen);
            }
    };

};

#endif // CHESSMANAGER_H
