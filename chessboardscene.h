#ifndef CHESSBOARDSCENE_H
#define CHESSBOARDSCENE_H

#include <QGraphicsScene>
#include <QPainter>
#include "ChessBoardManager/ObserverChessManager.h"
#include "ChessBoardManager/chessmanager.h"
#include "ChessBoardManager/ILoaderGame.h"
#include "IControleur.h"
#include "ClientGraphique/IUserPiecesGraphiques.h"
#include "Data/ArticlePiece.hpp"
#include <QPixmap>
#include <QDragEnterEvent>
#include <map>
#include <list>
#include <QPair>
#include <QVector>
#include <QPropertyAnimation>

class ChessManager;
class DescriptorDemo;
class QGraphicsPieceItem;
class QGraphicsTextItem;
class CommandPlay;
class IChessBoardView;
class QAction;
class QSequentialAnimationGroup;
class IActionDemo;
class AlgoComputer;
class IObserverAlgo;
class ThreadAlgo;

/**
 * @brief Classe représentant la scène. C'est- -dire le containeur des pieces. Cette classe est un observateur du ChessManager.
 *
 */
class ChessBoardScene : public QGraphicsScene,
                        public ObserverChessManager,
                        public ILoaderGame,
                        public IControleur,
                        public IUserPiecesGraphiques
{
    Q_OBJECT
public:
    ChessBoardScene( QObject *parent);
    virtual ~ChessBoardScene();
//interface ObserverChessManager
    virtual void DoOnAjouterPiece(eTypePiece aTypePiece, eColorPiece aColor, int aPosX, int aPosY);
    virtual void DoOnResetPieces ();
    virtual void DoOnModificationPlacePiece (eTypePiece, eColorPiece, int aOldPosX, int aOldPosY, int anewPosX, int anewPosY);
    virtual void DoOnKillPiece (eTypePiece , eColorPiece aColorPiece,
                                int aPosX, int aPosY);
    virtual void DoOnPlay(CommandPlay *aCommandPlay);

//**interface ILoaderGame **//

    virtual void InitializeGame(bool aCreationPiece);
    virtual void AjouterDemiCoup(CommandPlay* aCommandPlay);
    virtual IChessManager& getChessManager();

//**************************//

    void setDemo (const DescriptorDemo& aDemo, bool aAllowInteractionChessBoard);
    void setFreeUseChessBoard (bool aDragOutPossible, bool aModePermissif);
    void SetModeRotation (bool aModeRotation);
    void setModeInfoAllowedMoves (bool aInfoAllowedMoves);

    /**
     * @brief Pour dessiner sur la scene les directions possibles de la piece ajoutee
     *
     * @param aPiece
     */
    void AddStaticPieceAndShowAllowedDirections (const ArticlePiece& aPiece);

    /**
     * @brief Pour dessiner des pieces qui ne bougeront pas, mais sans directions de déplacement
     *
     * @param aListePieces
     */
    void AddStaticPiecesWithoutShowingDirections (const tlisteArticlesPieces& aListePieces);

    bool getArticlesPiecesDeCouleur (eColorPiece aColorPiece,
                                     tlisteArticlesPieces &aListeArticles) const;
    void RegisterObserver (ObserverChessManager &aObserver);
    void RegisterView (IChessBoardView& aView) {_ListeViews.append(&aView);}
    void ChangeCurrentCommentary (const QString& anewCommentary);
    void SaveGame () const;

    /***Interface IControleur****/

    /**
     * @brief Pour jouer contre l'ordi
     *
     * @param aModePlayAgainComputer
     */
    virtual void setModePlayAgainstComputer (IObserverAlgo & aObserver,
                                     eColorPiece aColorComputer,
                                     bool aModeComputerAgainstComputer);
    virtual bool StopAllComputations();
    virtual void AllowPlayFirstBlack (bool aAllow);
    virtual void AllowShowAnySquareForMovePiece (bool aAllow);
    virtual void AllowUserInteraction (bool aAllow);


    /***Interface IUserPiecesGraphiques****/
    /**
     * @brief Pour updater le graphic des pieces
     *
     */
    virtual void RechargerGraphiquesPieces ();

    /****************************/

    bool IsMovingPiece () const {return (_InfosItemSelected._ItemSelected != 0);}

protected:
    virtual void drawBackground ( QPainter * painter, const QRectF & arect);
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event);
    //necessaire pour enlever le forbidden cursor!!
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * );

    /**
     * @brief Pour la gestion du replay et du undo par la souris
     *
     * @param event
     */
    virtual void wheelEvent(QGraphicsSceneWheelEvent* event);

private :
    //re-initialisation
    void ReInit();

    void GererDropDePiece (const QString& aMessage,
                           int aPosDebut,
                           int aPosFin, QGraphicsSceneDragDropEvent *event);
    void GererDropDePGNGame (const QString& aMessage,
                             int aPosDebut,
                             int aPosFin);
    static const int _PosCachePieceKilled = 10;
    ChessBoardScene ();
    void ConvertPosSceneToPosBoardManager (qreal aXscene, qreal aYscene, int &aXChessManager, int &aYChessManager);
    void ConvertPosBoardManagerToPosScene (int aXChessManager, int aYChessManager, qreal &aXscene, qreal &aYscene);

    /**
     * @brief Methode pour remplacer ItemAt car comprend pas pourquoi marche pas après rotation (meme avec scenepos plutot que pos!!)
     *
     * @param aPosSceneX
     * @param aPosSceneY
     * @return QGraphicsPieceItem
     */
    QGraphicsPieceItem* TrouverPieceAtPosScene (qreal aPosSceneX, qreal aPosSceneY);
    QGraphicsPieceItem* TrouverPiece (eTypePiece aTypePiece,
                                      eColorPiece aColor,
                                      bool aVisible,
                                      int aPosX=_PosCachePieceKilled,
                                      int aPosY=_PosCachePieceKilled);
    ChessManager _ChessManager;
    typedef std::multimap<eTypePiece, QGraphicsPieceItem*> tMapTypePiece_GraphicPieceItems;
    tMapTypePiece_GraphicPieceItems _mapTypePieceListePieceItems;

    struct sItemSelected {
        QGraphicsPieceItem* _ItemSelected; /**< piece selectionnee */
        QPair <int, int> _OldChessManagerPos; /**< position d'origine (x, y)*/
        QPair <int, int> _NewChessManagerPos; /**< nouvelle position (x, y)*/
        bool _MustBeRemoved;   /**< cas particuliers de move en dehors de la scene si autorise */
        sItemSelected () :
            _ItemSelected(0),
            _MustBeRemoved (false)
        {
        }
        void Init () {_ItemSelected = 0;_MustBeRemoved=false;}
    };

    sItemSelected _InfosItemSelected;
    bool _CanMoveOutOfScene;

    /********Pour gestion rotation**********/

    bool _ModeRotation; /**< Pour désigner si la scene doit etre tournee de 180 degres */

    /********Pour gestion Infos Allowed moves**********/

    bool _ModeInfosAllowedMoves; /**< Pour afficher les cases autorisees par la piece sélectionnée */

    /********Pour gestion dessiner direction moves**********/

    bool _ModeShowAllowedPath; /**< Pour afficher les directions autorisees par la piece sélectionnée */

    /***************************************/
    /*******Pour gestion replay*************/

    QVector<CommandPlay*> _ListCommandesPlay;
    void Replay (int aNbreMoves);
    void Rollback (int aNbreMoves);
    int _CurrentDemiMove; /**< Pour indiquer   quel move on est (commence   1) */
    bool AskForNewVariante ();
    void EraseRollBackMoves();

    /***************************************/

    QVector<IChessBoardView*> _ListeViews; /**< Liste des vues de la scene */

    /**************************************/

    QString _FileNameLoaded; /**< stocke le nom du dernier fichier PGN qui a été chargé */

    /**
     * @brief Pour supprimer de la scene tous les items pieces graphiques
     *
     */
    void NettoyerPieces ();


    /**
     * @brief Pour connaitre la taille d'une case
     *
     * @return float
     */
    inline float getCellSize() const;


    /**
     * @brief Pourt créer un item piece graphique dans la scene
     *
     * @param aColor
     * @param aTypePiece
     * @return QGraphicsPieceItem
     */
    QGraphicsPieceItem* CreateAPieceItemInScene (eColorPiece aColor, eTypePiece aTypePiece);

    /************************************************/

    /**
     * @brief Cette méthode va servir   transmettre les infos   la vue
     *
     * @param aDescriptionMove
     * @param aCommentary
     */
    void DoOnInfosChanged (const QString& aDescriptionMove, const QString& aCommentary) const;

    /**
     * @brief Pour mettre   jour les infos de la vue
     *
     */
    void ActualiserInfosAffichageVue ();

    CommandPlay* getPreviousCommandPlay() const;

    /********Pour gestion des animations  **********/
    struct sMoveAttendu {
        int _OldPosX, _OldPosY, _newPosX, _newPosY;
        eTypePiece _TypePiece;//type finale de la piece attendue (pour cas promotion)
        sMoveAttendu (int aOldPosX, int aOldPosY,
                      int anewPosX, int anewPosY,
                      eTypePiece aTypePiece) :
            _OldPosX(aOldPosX), _OldPosY (aOldPosY),
            _newPosX(anewPosX), _newPosY (anewPosY),
            _TypePiece(aTypePiece)
        {}
        sMoveAttendu () :
            _OldPosX (0),
            _OldPosY (0),
            _newPosX (0),
            _newPosY (0),
            _TypePiece(King)
        {}
    };

    QSequentialAnimationGroup *_groupAnimation; /**< Gere les animations de pieces */
    QList<sMoveAttendu> _ListeMovesAttendus;
    bool CheckIfExpectedMoveAndGoodOne (int aOldPosX, int aOldPosY,
                              int anewPosX, int anewPosY, bool aCheckTypePiece);
    bool CheckIfActionMoveIsPlayer (const QVector<IActionDemo*>& aListeActionsDemo,
                                     int aNextIndex,
                                     sMoveAttendu& aMoveAttendu);
    friend class QPropertyAnimationDemoMove;
    enum eTypeSequenceDemo {
        eSequenceDemoMoveAttendu,
        eSequenceDemoAnimation
    };

    QList<eTypeSequenceDemo> _SequencesDemo;
    void ReinitGestionAnimationsOnPlay ();
    bool APlayerMoveIsExpected ();

    //////////Pour gestion des animations  ////////////

    /********Pour gestion Play against Computer  **********/
    AlgoComputer *_AlgoComputer;
    bool _ModeComputerAgainstComputer;
    void LaunchAlgo (CommandPlay *aCommandPlay);
    ThreadAlgo *_threadAlgo;
    bool _AllowUserInteraction;
    //*******....Pour gestion Play against Computer  ******/

    bool _RegisterHistory;

    //****pour mermettre de voir la piece se deplacee sur n'importe quelle case (avec curseur interdit tout de meme)
    bool _AllowShowAnySquareForMovePiece;


    /**
     * @brief Pour jouer le coup
     *
     * @param aOldPosX
     * @param aOldPosY
     * @param aNewPosX
     * @param aNewPosY
     * @return bool
     */
    bool PlayMove (int aOldPosX, int aOldPosY, int aNewPosX, int aNewPosY);

private slots:
    void OnAnimationFinished();
    void AnimationsAreFinished ();

    void OnStartAlgoComputation();
    void NettoyageThreadAlgo ();

    /**
     * @brief Pour prévenir la vue qu'il y a échec et mat
     *
     */
    void DoOnComputerHasNoMove () const;
    void DoOnEnnemyComputerHasNoMoveAfter(int aOldPosX, int aOldPosY, int aNewPosX, int aNewPosY, bool aGivesCheck);
    void DoComputerMove (int aOldPosX, int aOldPosY, int aNewPosX, int aNewPosY);
 };


class QPropertyAnimationDemoMove : public QPropertyAnimation {
    Q_OBJECT
public:
    QPropertyAnimationDemoMove (const QString& aName,
                                ChessBoardScene& aChessBoardScene,
                                bool aModifierDataBase,
                                QObject *target,
                                const QByteArray &propertyName,
                                QObject *parent = 0);
    const QString& getName() const {return _Name;}
protected :
    virtual void updateCurrentValue ( const QVariant & value );
private:
    ChessBoardScene& _ChessBoardScene;
    QString _Name;
private slots:
    void OnFinished();

};

#endif // CHESSBOARDSCENE_H
