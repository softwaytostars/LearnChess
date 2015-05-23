#ifndef COMMANDPLAY_H
#define COMMANDPLAY_H

#include "Data/ArticlePiece.hpp"
#include "Demos/actionsdemo.h"

class IChessManager;

class CommandPlay
{
public:
    CommandPlay(IChessManager& aChessManager);
    ~CommandPlay();
    void AddActionMove (const ArticlePiece& aArticleFrom, int anewPosX, int anewPosY);
    void AddActionMangerPiece (const ArticlePiece& aArticleFrom, const ArticlePiece& aArticleTo);
    void AddActionPromotion (const ArticlePiece& aArticleFrom,
                             eTypePiece aGenrePromotion);
    void AddActionPriseEnPassant (const ArticlePiece& aArticleFrom,
                                  int aNewPosX,
                                  int aNewPosY,
                                  const ArticlePiece& aPawnToDelete);
    void AddActionCastling (eColorPiece aColor, bool aSmallCastle);
    bool Execute (bool aComputeIfEnnemyIsChecked=true);
    bool UnExecute();
    QString getDescription() const {return _Description;}
    QString getCommentary() const {return _Commentary;}
    QString getCommentaryMiseEnForme() const;
    void setCommentary (const QString& aCommentary) {
        _Commentary = aCommentary;}

    bool EstUnSimpleMove (eTypePiece& aPiece,
                          int &aOldPosX,
                          int &aOldPosY,
                          int &aNewPosX,
                          int &aNewPosY,
                          eColorPiece &aColorPiece) const;
    bool EstUneSimpleAttaque (eTypePiece& aPiece,
                              int &aOldPosX,
                              int &aOldPosY,
                              int &aNewPosX,
                              int &aNewPosY,
                              eColorPiece &aColorPiece) const;
    bool EstUnePromotion (int &aOldPosX,
                          int &aOldPosY,
                          int &aNewPosX,
                          int &aNewPosY,
                          eTypePiece& aPiecePromotion,
                          bool &aAvecAttaque,
                          eColorPiece &aColorPiece) const;

    bool EstUnGrandRoque(eColorPiece &aColorPiece, int &aPosXKing, int &aPosYKing) const;
    bool EstUnPetitRoque(eColorPiece& aColorPiece, int &aPosXKing, int &aPosYKing) const;

    bool EstUnePriseEnPassant (int &aOldPosX, int &aOldPosY,
                               int &aNewPosX, int &aNewPosY) const;

    bool GetInfosPieceDeplacee (int &aOldPosX, int &aOldPosY,
                                int &aNewPosX, int &aNewPosY) const;
    eColorPiece getColor () const;
    bool KingEnnemyIsChecked() const {return (_PiecesGivingCheck.count()>0);}
    const tlisteArticlesPieces& getPiecesGivingCheck() const {return _PiecesGivingCheck;}

    void FillActionsForDemo (QVector <IActionDemo*>& aListeActionsDemo,
                             bool aAllDescriptions,
                             bool aActionPlayer) const;
private:
    class IAction {
    public:
        IAction (IChessManager& aChessManager) : _ChessManager (aChessManager) {;}
        virtual bool Execute()=0;
        virtual bool UnExecute()=0;
        virtual eColorPiece getColorPieceInvolved () const =0;
    protected:
        IChessManager& _ChessManager;
    };

    class ActionMove : public IAction {
    public:
        ActionMove (IChessManager& aChessManager,
                    const ArticlePiece& aArticleFrom,
                    int anewPosX,
                    int anewPosY);
        virtual bool Execute();
        virtual bool UnExecute();
        virtual eColorPiece getColorPieceInvolved () const;
        ArticlePiece _ArticleFrom, _ArticleTo;
        bool _ExecutionCorrecte;
    };
   // friend class ActionMove;

    class ActionMangerPiece : public IAction {
    public:
        ActionMangerPiece (IChessManager& aChessManager,
                           const ArticlePiece& aArticleFrom,
                           const ArticlePiece& aArticleTo);
        virtual bool Execute();
        virtual bool UnExecute();
        virtual eColorPiece getColorPieceInvolved () const;
        ArticlePiece _ArticleFrom, _ArticleTo;
        bool _Execution1Correcte, _Execution2Correcte;
    };
   // friend class ActionMangerPiece;

    class ActionPromotion : public IAction {
    public:
        ActionPromotion (IChessManager& aChessManager,
                         const ArticlePiece& aArticleFrom,
                         eTypePiece aGenrePromotion);
        virtual bool Execute();
        virtual bool UnExecute();
        virtual eColorPiece getColorPieceInvolved () const;
        ArticlePiece _ArticleFrom;
        int _newPosX, _newPosY;
        eTypePiece _GenrePromotion;
        bool _Execution1Correcte, _Execution2Correcte;
    };
   // friend class ActionPromotion;

    class ActionPriseEnPassant : public IAction {
    public:
        ActionPriseEnPassant (IChessManager& aChessManager,
                              const ArticlePiece& aArticleFrom,
                              int anewPosX,
                              int anewPosY,
                              const ArticlePiece& aPawnToDelete);
        virtual bool Execute();
        virtual bool UnExecute();
        virtual eColorPiece getColorPieceInvolved () const;
        ArticlePiece _ArticleFrom, _ArticleTo, _PawnToDelete;
        bool _Execution1Correcte, _Execution2Correcte;
    };

    class ActionCastling : public IAction {
    public:
        ActionCastling (IChessManager& aChessManager,
                        const ArticlePiece& aKing,
                        const ArticlePiece& aRook,
                        int anewPosXKing,
                        int anewPosXRook);
        virtual bool Execute();
        virtual bool UnExecute();
        virtual eColorPiece getColorPieceInvolved () const;
        ArticlePiece _King, _Rook;
        int _newPosXKing, _newPosXRook;
        bool _Execution1Correcte, _Execution2Correcte;
    };

    CommandPlay();
    IChessManager& _ChessManager;
    QVector<IAction*> _ListeActions;
    bool _HasBeenExecuted;
    QString _Commentary;
    tlisteArticlesPieces _PiecesGivingCheck;
    bool _HasEverComputedIfEnnemyIsChecked;
    QString _Description;

    /**
     * @brief Fonction utilitaire pour savoir si roque
     *
     * @param aDeltaRook
     * @return bool
     */
    bool EstUnRoque(bool &aEstPetitRoque, eColorPiece &aColorPiece, int &anewPosXKing, int &anewPosYKing) const;

    /**
     * @brief Pour renseigner si besoin la colonne ou ligne de la piece qui doit bouger
     *
     * @param aColor
     * @param aTypePiece
     * @param aFromPosX
     * @param aFromPosY
     * @param aToPosX
     * @param aToPosY
     * @return QString
     */
    QString AmbiguiteSurPiece (eColorPiece aColor, eTypePiece aTypePiece,
                               int aFromPosX, int aFromPosY,
                               int aToPosX, int aToPosY) const;

    QString ComputeDescription() const;

};

#endif // COMMANDPLAY_H
