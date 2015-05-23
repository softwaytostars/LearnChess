#include "chessboardscene.h"
#include "TypeInfoChessBoard.hpp"
#include "qgraphicspieceitem.h"

#include <QSequentialAnimationGroup>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QGraphicsView>
#include <QtCore/qmath.h>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>

#include "infosgraphiques.h"
#include "descriptordemo.h"
#include "IChessBoardView.hpp"
#include "ChessBoardManager/commandplay.h"
#include "ImportExport/pgnreaderwriter.h"
#include "ClientGraphique/dialogaskentetefichierpgn.h"
#include "infosconfiguration.h"
#include "Demos/actionsdemo.h"
#include "ClientGraphique/painterpathmoves.h"
#include "Algo/algocomputer.h"
#include "Algo/ThreadAlgo.h"
#include "ClientGraphique/affichages.h"
#include <sstream>

#ifdef _DEBUG
#include <QDebug>
#endif

//#include <QtNetwork/QNetworkInterface>

//QString getMacAddress()
//{
//    QList<QNetworkInterface> lInterfaces =  QNetworkInterface::allInterfaces();
//    for (int i= 0; i < lInterfaces.count(); i++)
//    {
//        // Return only the first non-loopback MAC Address
//        if (!(lInterfaces [i].flags() & QNetworkInterface::IsLoopBack))
//            return lInterfaces [i].hardwareAddress();
//    }
//    return QString();
//}

ChessBoardScene::ChessBoardScene(QObject *parent) :
    QGraphicsScene (QRect (-constants::GLOBAL_CONST_BORDER, 0, 441+constants::GLOBAL_CONST_BORDER, 441+constants::GLOBAL_CONST_BORDER), parent),
    ObserverChessManager(),
    _ChessManager (false),
    _ModeRotation(false),
    _ModeInfosAllowedMoves (false),
    _ModeShowAllowedPath(false),
    _CurrentDemiMove(0),
    _FileNameLoaded(""),
    _groupAnimation(0),
    _AlgoComputer(0),
    _ModeComputerAgainstComputer(false),
    _threadAlgo(0),
    _AllowUserInteraction(true),
    _RegisterHistory(true),
    _AllowShowAnySquareForMovePiece(false)

{
    InfosGraphiques::instance()->AbonnerUtilisateurPiecesGraphiques(this);

    _ListCommandesPlay.reserve(200);
   // setSceneRect(-constants::GLOBAL_CONST_BORDER, 0, 401+constants::GLOBAL_CONST_BORDER, 401+constants::GLOBAL_CONST_BORDER);
    _InfosItemSelected.Init();
    //enregistre la scene comme observateur du chessManager
    _ChessManager.RegisterObserver(*this);
    setFreeUseChessBoard (false, false);

    //creation des items pour les notations sur les bords
    QGraphicsSimpleTextItem *itemCol, *itemLigne;
    for (int i = 1; i <= constants::GLOBAL_CONST_DIMENSION; i++) {

        itemCol = this->addSimpleText(QString("%1").arg(i));
        itemCol->setFlag(QGraphicsItem::ItemIsMovable, false);
        itemCol->setFlag(QGraphicsItem::ItemIsSelectable, false);
        itemCol->setTransformOriginPoint(itemCol->boundingRect().center());
        itemCol->setPos(-itemCol->boundingRect().width()/2 - qreal (1.0*constants::GLOBAL_CONST_BORDER/2),
                        (constants::GLOBAL_CONST_DIMENSION-i)*getCellSize()+getCellSize()/2-(itemCol->boundingRect().height()/2));

        itemLigne = this->addSimpleText(QString(QChar (i-1+int('A'))));
        itemLigne->setFlag(QGraphicsItem::ItemIsMovable, false);
        itemLigne->setFlag(QGraphicsItem::ItemIsSelectable, false);
        itemLigne->setTransformOriginPoint(itemLigne->boundingRect().center());
        itemLigne->setPos((i-1)*getCellSize()+getCellSize()/2-(itemLigne->boundingRect().width()/2),
                          sceneRect().height()-qreal (1.0*constants::GLOBAL_CONST_BORDER));
    }

}

ChessBoardScene::~ChessBoardScene() {

    InfosGraphiques::instance()->DesabonnerUtilisateurPiecesGraphiques(this);

    if (StopAllComputations()) {

        this->clear();

        for (QVector<CommandPlay*>::const_iterator it =_ListCommandesPlay.begin();
             it != _ListCommandesPlay.end();
             ++it) {
            delete (*it);
        }

        if (_AlgoComputer) delete _AlgoComputer;
    }
}


void ChessBoardScene::SetModeRotation (bool aModeRotation) {

    _ModeRotation = aModeRotation;
    //boucle sur tous les items de la scene
    QList<QGraphicsItem *> lItems = this->items();
    for (QList<QGraphicsItem *>::iterator it = lItems.begin();
         it != lItems.end();
         ++it) {

        //si item est une piece, fait rotation (pas meme fonctionnement que la view car ici c'est un etat)
        QGraphicsItem* itemPiece = dynamic_cast<QGraphicsItem*> (*it);
        if (itemPiece) {
            if ( (itemPiece->rotation() == 0) && aModeRotation)
                itemPiece->setRotation(180);
            else
                itemPiece->setRotation(0);
        }

    }
}

void ChessBoardScene::setModeInfoAllowedMoves (bool aInfoAllowedMoves) {

    _ModeInfosAllowedMoves = aInfoAllowedMoves;
}

bool ChessBoardScene::getArticlesPiecesDeCouleur (eColorPiece aColorPiece,
                                                  tlisteArticlesPieces& aListeArticles) const {

    return (_ChessManager.getArticlesPiecesDeCouleur(aColorPiece, aListeArticles));
}

float ChessBoardScene::getCellSize() const {

    return (width()-constants::GLOBAL_CONST_BORDER)/constants::GLOBAL_CONST_DIMENSION;
}

void ChessBoardScene::ConvertPosSceneToPosBoardManager (qreal aXscene, qreal aYscene, int &aXChessManager, int &aYChessManager)
{

    float lCellSize = getCellSize();
    aXChessManager = qFloor (aXscene/lCellSize)+1;
    if (!_CanMoveOutOfScene) {
        //assure limites si erreur scene
        aXChessManager = qMax<int> (aXChessManager, 1);
        aXChessManager = qMin<int> (aXChessManager, constants::GLOBAL_CONST_DIMENSION);
    }

    aYChessManager = constants::GLOBAL_CONST_DIMENSION - floor (aYscene/lCellSize);
    //assure limites si erreur scene
    if (!_CanMoveOutOfScene)
        aYChessManager = qMax<int> (aYChessManager, 1);

}

void ChessBoardScene::ConvertPosBoardManagerToPosScene (int aXChessManager, int aYChessManager, qreal &aXscene, qreal &aYscene)
{
    float lCellSize = getCellSize();
    aXscene = (aXChessManager -1)*lCellSize;
    aYscene = (constants::GLOBAL_CONST_DIMENSION - aYChessManager)*lCellSize;//origin of the scene is at left-top
}

//methode pour remplacer ItemAt car comprend pas pourquoi marche pas aprÃ¨s rotation (meme avec scenepos plutot que pos!!)
QGraphicsPieceItem* ChessBoardScene::TrouverPieceAtPosScene (qreal aPosSceneX, qreal aPosSceneY) {

    if (!_ModeRotation)
        return (dynamic_cast<QGraphicsPieceItem*> (this->itemAt(aPosSceneX, aPosSceneY)));
    else {
        //pour rotation, il suffit d'ajouter 1 pour Ãªtre sÃ»r d'Ãªtre dans le rect mais comprends pas pourquoi
        return (dynamic_cast<QGraphicsPieceItem*> (this->itemAt(aPosSceneX+1, aPosSceneY+1)));
        //sinon je bouclais sur les items
    }
    return 0;

}

QGraphicsPieceItem* ChessBoardScene::TrouverPiece (eTypePiece aTypePiece,
                                                    eColorPiece aColor,
                                                    bool aVisible,
                                                    int aPosX,
                                                    int aPosY)
{
    qreal lPosXScene, lPosYScene;
    //convertit position de depart de la piece en position de depart de la scene
    ConvertPosBoardManagerToPosScene (aPosX, aPosY, lPosXScene, lPosYScene);

    std::pair<tMapTypePiece_GraphicPieceItems::iterator, tMapTypePiece_GraphicPieceItems::iterator> ppp;
    ppp = _mapTypePieceListePieceItems.equal_range(aTypePiece);
    for (tMapTypePiece_GraphicPieceItems::iterator it = ppp.first;
         it != ppp.second;
         ++it) {
        QGraphicsPieceItem* litem = (*it).second;
        if (litem &&
                (litem->getTypePiece() == aTypePiece) &&
                (litem->getColorPiece() == aColor) &&
                (litem->isVisible() == aVisible) &&
                (litem->pos().x() == lPosXScene) &&
                (litem->pos().y() == lPosYScene)) {
               return litem;
           }

    }
    return 0;

}

QGraphicsPieceItem* ChessBoardScene::CreateAPieceItemInScene (eColorPiece aColor, eTypePiece aTypePiece) {
    QPixmap lGraphicPiece;
    if (InfosGraphiques::instance()->get_GraphicPiece (aColor,
                                                       aTypePiece,
                                                       lGraphicPiece,
                                                       getCellSize())) {

        QGraphicsPieceItem* item = new QGraphicsPieceItem(lGraphicPiece, aTypePiece, aColor);
        if (item) {
            _mapTypePieceListePieceItems.insert (std::pair<eTypePiece, QGraphicsPieceItem*>(aTypePiece, item));
            this->addItem(item);
            item->setTransformOriginPoint(item->boundingRect().center());
            //en mode rotation, toute piece est ajoutee pivotee sur elle-meme
            if (_ModeRotation)
                item->setRotation(180);
            else
                item->setRotation(0);
        }
        return item;
       }
    return 0;
}

void ChessBoardScene::DoOnAjouterPiece(eTypePiece aTypePiece, eColorPiece aColor, int aPosX, int aPosY) {

    qreal lPosXScene, lPosYScene;
    //convertit la position de chessmanager Ã  la scene
    ConvertPosBoardManagerToPosScene (aPosX, aPosY, lPosXScene, lPosYScene);

    //essaie de trouver une piece invisible deja cree
    QGraphicsPieceItem* item = TrouverPiece (aTypePiece, aColor, false);
    //si existe, la rend visible et la met Ã  la position voulue
    if (item) {

        item->setVisible(true);
        item->setPos (QPointF(lPosXScene, lPosYScene));

    }else {//sinon la cree

        item = CreateAPieceItemInScene (aColor, aTypePiece);
        if (item) {
            //item->setFlag(QGraphicsItem::ItemIsMovable, true);//inutile car redefinission de MouseMoveEvent pour le bouger
            item->setFlag(QGraphicsItem::ItemIsSelectable , true);
           // item->setTransform(QTransform::fromScale(lfactorX, lfactorY));     
            item->setPos (QPointF(lPosXScene, lPosYScene));
        }
    }
}


void ChessBoardScene::DoOnResetPieces () {

    //plutot que tout supprimer, on va les rendre invisibles
    QList<QGraphicsItem *> lListItems = this->items();
    for (QList<QGraphicsItem *>::const_iterator it = lListItems.begin();
         it != lListItems.end();
         ++it) {
        QGraphicsPieceItem* litem = dynamic_cast<QGraphicsPieceItem*>(*it);
        if (litem) {
            qreal lPosXScene, lPosYScene;
            litem->setVisible(false);
            ConvertPosBoardManagerToPosScene (_PosCachePieceKilled, _PosCachePieceKilled, lPosXScene, lPosYScene);
            litem->setPos(lPosXScene, lPosYScene);

        }
    }
    //re-initialisation
    ReInit();

}

void ChessBoardScene::DoOnModificationPlacePiece (eTypePiece, eColorPiece, int aOldPosX, int aOldPosY, int anewPosX, int anewPosY) {

    qreal lPosXScene, lPosYScene;
    //convertit position de depart de la piece en position de depart de la scene
    ConvertPosBoardManagerToPosScene (aOldPosX, aOldPosY, lPosXScene, lPosYScene);
    //trouve l'item Ã  la position donnee
    QGraphicsPieceItem *item = TrouverPieceAtPosScene (lPosXScene, lPosYScene);
    if (item) {
        //deplace la piece
        ConvertPosBoardManagerToPosScene (anewPosX, anewPosY, lPosXScene, lPosYScene);
        item->setPos(lPosXScene, lPosYScene);
    }
}

//plutot que supprimer l'item, on va le cacher!
void ChessBoardScene::DoOnKillPiece (eTypePiece aTypePiece, eColorPiece aColorPiece,
                                     int aPosX, int aPosY) {

    qreal lPosXScene, lPosYScene;
//    //convertit position de depart de la piece en position de depart de la scene
//    ConvertPosBoardManagerToPosScene (aPosX, aPosY, lPosXScene, lPosYScene);
    //trouve l'item Ã  la position donnee
    QGraphicsPieceItem *item = TrouverPiece (aTypePiece, aColorPiece, true, aPosX, aPosY);// TrouverPieceAtPosScene (lPosXScene, lPosYScene);
    if (item) {
        ConvertPosBoardManagerToPosScene (_PosCachePieceKilled, _PosCachePieceKilled, lPosXScene, lPosYScene);
        item->setPos(lPosXScene, lPosYScene);
        item->setVisible(false);
    }
}

void ChessBoardScene::LaunchAlgo (CommandPlay *aCommandPlay) {

    if (!_AlgoComputer) return;

    //cree un thread pour lancer l'algo (permet d'afficher les mouvements du coup prÃ©cÃ©dent)
    //le thread sera dÃ©truit automatiquement dÃ¨s qu'il est finit (cf constructeur)
    _threadAlgo = new ThreadAlgo (*_AlgoComputer, _CurrentDemiMove, aCommandPlay);
    QObject::connect (_threadAlgo, SIGNAL (StartComputation()), this , SLOT (OnStartAlgoComputation()));
    QObject::connect (_threadAlgo, SIGNAL (GiveMoveToPlay (int, int, int, int)), this, SLOT (DoComputerMove (int, int, int, int)), Qt::QueuedConnection);
    QObject::connect (_threadAlgo, SIGNAL (ComputerHasNoMove()), this, SLOT (DoOnComputerHasNoMove ()), Qt::QueuedConnection);
    QObject::connect (_threadAlgo, SIGNAL (EnnemyHasNoMoveAfter (int, int, int, int, bool)), this, SLOT (DoOnEnnemyComputerHasNoMoveAfter (int, int, int, int, bool)), Qt::QueuedConnection);
    QObject::connect (_threadAlgo, SIGNAL (finished()), this, SLOT (NettoyageThreadAlgo()));
    _threadAlgo->start();

}


void ChessBoardScene::DoOnPlay(CommandPlay *aCommandPlay) {

    if (_RegisterHistory)
        _ListCommandesPlay.append(aCommandPlay);
    _CurrentDemiMove += 1;

    ActualiserInfosAffichageVue ();

    //laisse ensuite l'ordinateur
    if (_AlgoComputer
            && (_ModeComputerAgainstComputer ||
                (_AlgoComputer->get_ColorComputer() == _ChessManager.get_ColorEnnemy(aCommandPlay->getColor())))) {

        LaunchAlgo (aCommandPlay);

    }
}


void ChessBoardScene::drawBackground ( QPainter * painter, const QRectF &arect)
{
    painter->eraseRect(arect);

    QColor lColorBlack, lColorWhite;
    InfosGraphiques::instance()->get_ColorsBackground(lColorBlack, lColorWhite);

    float lCellSize = getCellSize();
    for (int i=1; i<=constants::GLOBAL_CONST_DIMENSION; i++){
        for (int j=1; j<=constants::GLOBAL_CONST_DIMENSION; j++){
              if (((i+j)%2)==1){
                  painter->fillRect ((i-1)*lCellSize, (j-1)*lCellSize, lCellSize, lCellSize, lColorBlack);
              } else { painter->fillRect ((i-1)*lCellSize, (j-1)*lCellSize, lCellSize, lCellSize, lColorWhite);};
        }
    }
    painter->drawRect(0, 0, width()-constants::GLOBAL_CONST_BORDER, height()-constants::GLOBAL_CONST_BORDER);

    //pour dessiner les cases permises par la piece selectionnee
    if (_ModeInfosAllowedMoves && _InfosItemSelected._ItemSelected) {

        //definit un nouveau pinceau
        QPen lPenOld = painter->pen();
        QPen lPen = lPenOld;
        lPen.setWidth(2);
        lPen.setColor(Qt::blue);
        painter->setPen(lPen);

        qreal lPosXScene, lPosYScene;
        CommandPlay *lCommandPreviousPlay = getPreviousCommandPlay();
        ArticlePiece lPiece;
        tQVectorInfosSquare lListSquares;
        if (_ChessManager.UnePieceExiste (_InfosItemSelected._OldChessManagerPos.first,
                                           _InfosItemSelected._OldChessManagerPos.second,
                                           lPiece)
                && _ChessManager.get_SquaresForMoves (lPiece,
                                lCommandPreviousPlay,
                                lListSquares,
                                false)) {

            for (tQVectorInfosSquare::const_iterator itInfoSquare = lListSquares.begin();
                 itInfoSquare != lListSquares.end();
                 ++itInfoSquare) {
                ConvertPosBoardManagerToPosScene ((*itInfoSquare)._PosX, (*itInfoSquare)._PosY, lPosXScene, lPosYScene);
                painter->drawRect(lPosXScene, lPosYScene, lCellSize, lCellSize);

            }
        }

        //retablit l'ancien pinceau
        painter->setPen(lPenOld);

    }//if (_ModeInfosAllowedMoves && _InfosItemSelected._ItemSelected)

   if (_ModeShowAllowedPath && _InfosItemSelected._ItemSelected) {

        if (painter) {
            PainterPathMoves lPainterPathMoves (*painter,
                                                _InfosItemSelected._ItemSelected->pos(),
                                                _InfosItemSelected._ItemSelected->getTypePiece(),
                                                _InfosItemSelected._ItemSelected->getColorPiece(),
                                                lCellSize
                                                );
            lPainterPathMoves.DrawPaths();
        }
    }//if (_ModeShowAllowedPath && _InfosItemSelected._ItemSelected)


}//drawBackground

void ChessBoardScene::NettoyerPieces () {

    QList<QGraphicsItem *> lItems = this->items();
    for (QList<QGraphicsItem *>::iterator it = lItems.begin();
         it != lItems.end();
         ++it) {
        QGraphicsPieceItem* itemPiece = dynamic_cast<QGraphicsPieceItem*> (*it);
        if (itemPiece) {
            this->removeItem(itemPiece);
            delete itemPiece;
            itemPiece = 0;
        }
    }
}

void ChessBoardScene::RechargerGraphiquesPieces () {

    QList<QGraphicsItem *> lItems = this->items();
    for (QList<QGraphicsItem *>::iterator it = lItems.begin();
         it != lItems.end();
         ++it) {
        QGraphicsPieceItem* itemPiece = dynamic_cast<QGraphicsPieceItem*> (*it);
        if (itemPiece) {
            QPixmap lGraphicPiece;
            if (InfosGraphiques::instance()->get_GraphicPiece (itemPiece->getColorPiece(),
                                                               itemPiece->getTypePiece(),                                                           
                                                               lGraphicPiece,
                                                               getCellSize()))
                itemPiece->ChangePieceGraphique (lGraphicPiece);
        }
    }
    this->update();
}

QPropertyAnimationDemoMove::QPropertyAnimationDemoMove (const QString &aName,
                                                        ChessBoardScene &aChessBoardScene,
                                                        bool aModifierDataBase,
                                                        QObject *target,
                                                        const QByteArray &propertyName, QObject *parent) :
    QPropertyAnimation (target, propertyName, parent),
    _Name (aName),
    _ChessBoardScene (aChessBoardScene)
{
    if (aModifierDataBase)
        connect (this, SIGNAL (finished()), this, SLOT(OnFinished()));
}

void QPropertyAnimationDemoMove::updateCurrentValue ( const QVariant & value ) {

    //cette condition à cause BUG Qt : qd groupe est mis en pause, la premiere animation se reinitialise!!
    if (!_ChessBoardScene._groupAnimation || _ChessBoardScene._groupAnimation->state() == Running) {

        QPropertyAnimation::updateCurrentValue(value);

    }

}

void QPropertyAnimationDemoMove::OnFinished() {


    int lOldPosX, lOldPosY, lnewPosX, lnewPosY;


    QPointF lOldPos = this->keyValueAt(0).toPointF();
    _ChessBoardScene.ConvertPosSceneToPosBoardManager (lOldPos.x(),
                                      lOldPos.y(),
                                      lOldPosX,
                                      lOldPosY);

    QPointF lnewPos = this->keyValueAt(1).toPointF();
    _ChessBoardScene.ConvertPosSceneToPosBoardManager (lnewPos.x(),
                                      lnewPos.y(),
                                      lnewPosX,
                                      lnewPosY);

    if (!_ChessBoardScene.PlayMove (lOldPosX,
                               lOldPosY,
                               lnewPosX,
                               lnewPosY)) {
#ifdef _DEBUG
    qDebug()<<"PROBLEME GESTION : PlayMove pas fait!!!!"<<lOldPosX<<lOldPosY<<lnewPosX<<lnewPosY;
#endif
        //remet la piece à sa place
        QGraphicsPieceItem* item = dynamic_cast<QGraphicsPieceItem*> (this->targetObject());//TrouverPieceAtPosScene (lnewPos.x(), lnewPos.y());
        if (item)
            item->setPos(lOldPos);
        //relance l'algo pour comprendre
        _ChessBoardScene.LaunchAlgo(_ChessBoardScene.getPreviousCommandPlay());
    } else {

        _ChessBoardScene.OnAnimationFinished();
    }


}


//sert Ã  creer les animations
void ChessBoardScene::setDemo (const DescriptorDemo& aDemo, bool aAllowInteractionChessBoard) {


    qreal lPosXScene, lPosYScene;

    ReInit();
/***Initialisation config****/
    if (aAllowInteractionChessBoard) {

        _ChessManager.ResetPieces(aDemo.get_ListePieces());

    } else
    {
        //Reinitialise la scene
        NettoyerPieces();

        //met en place les pieces de la demo
        for (tlisteArticlesPieces::const_iterator it = aDemo.get_ListePieces().begin();
             it != aDemo.get_ListePieces().end();
             ++it) {
            QGraphicsPieceItem* newitem = CreateAPieceItemInScene ((*it)._Color, (*it)._TypePiece);
            if (newitem) {
                //newitem->setFlag(QGraphicsItem::ItemIsMovable, true);surtout pas car on veut pas permettre deplacement piece
                newitem->setFlag(QGraphicsItem::ItemIsSelectable, true);
                //convertit la position de chessmanager Ã  la scene
                ConvertPosBoardManagerToPosScene ((*it)._PosX, (*it)._PosY, lPosXScene, lPosYScene);
                newitem->setPos (QPointF(lPosXScene, lPosYScene));
            }
        }//for
    }

/****Ajout des animations ****/

    //trouve l'item Ã  la position donnee
    QGraphicsPieceItem *item=0;
    QPropertyAnimation *animationMove=0, *animationKilled=0;

    if (_groupAnimation) {
        delete _groupAnimation;
        _groupAnimation = 0;
    }

    if (aDemo.get_ListeActionDemo().size() > 0) {

        _AllowUserInteraction = false;
        _groupAnimation = new QSequentialAnimationGroup(this);
        connect (_groupAnimation, SIGNAL (finished()), this, SLOT (AnimationsAreFinished()));

    }

    int lPosXDebut, lPosYDebut, lPosXFin, lPosYFin;
    eTypePiece lTypePiece;
    eColorPiece lColorPiece;
    int lPosX, lPosY;
    tListMovesdemo lListeMovesDemo;
    QMultiMap<int, QGraphicsPieceItem *> lMapPosPiece;//mapping entre la position finale sur laquelle sera bougÃ©e une piece et la piece
    sMoveAttendu lMoveAttendu;

    QList<QGraphicsPieceItem *> lListesItemsKilled;
    //anime les mouvements demandes
    for (int i=0; i < aDemo.get_ListeActionDemo().size(); i++) {

        IActionDemo * lActionDemo = aDemo.get_ListeActionDemo()[i];

        //si action est une suppression
        if (lActionDemo->isActionDemoKill (lTypePiece, lPosX, lPosY)) {

            if (aAllowInteractionChessBoard) continue;

            //convertit position d'arrivee de la piece en position d'arrivee de la scene
            ConvertPosBoardManagerToPosScene (lPosX, lPosY, lPosXScene, lPosYScene);

            QGraphicsPieceItem *itemExistant = TrouverPieceAtPosScene (lPosXScene, lPosYScene);
            //Si piece pas trouvee Ã  l'endroit , c'est que peut-Ãªtre pas encore dÃ©placÃ©e
            if (lListesItemsKilled.contains(itemExistant) || !itemExistant) {
                int lqHashKey = _ChessManager.qHashKey (lPosX, lPosY);
                if (lMapPosPiece.contains(lqHashKey)) {
                    //parcours ceux qui existent du plus ancien au plus recent
                    QList<QGraphicsPieceItem*> lvalues = lMapPosPiece.values(lqHashKey);
                    for (int i=0; i < lvalues.size (); i++) {
                        QGraphicsPieceItem * item = lvalues.at(lvalues.size () -1 -i);
                        if (item && (item->getTypePiece() == lTypePiece)) {
                          itemExistant = item;//prend le premier item existant
                          lMapPosPiece.remove(lqHashKey, item);//le retire car sera "supprime" donc plus utilise
                          break;
                        }
                    }

                }
            }
            //si une piece existe sur la destination finale, la cache
            if (itemExistant) {

                //cree une animation en faisant varier sa QProperty opacity pour la faire disparaitre
                animationKilled = new QPropertyAnimation (itemExistant, "opacity");//avec visible ne marche pas alors que c'est une qproperty!!!
                animationKilled->setEasingCurve(QEasingCurve::InOutQuad);
                animationKilled->setDuration(50);
                animationKilled->setStartValue(1.0);
                animationKilled->setEndValue(0.0);
                _groupAnimation->addAnimation(animationKilled);
                lListesItemsKilled.push_back(itemExistant);

            }
        }//lActionDemo->isActionDemoKill

        else if (lActionDemo->isActionDemoAjout(lTypePiece, lColorPiece, lPosX, lPosY)) {

            if (aAllowInteractionChessBoard) continue;

            QGraphicsPieceItem* newitem = CreateAPieceItemInScene (lColorPiece, lTypePiece);
            if (newitem) {

                newitem->setFlag(QGraphicsItem::ItemIsSelectable, true);
                //convertit la position de chessmanager Ã  la scene
                ConvertPosBoardManagerToPosScene (lPosX, lPosY, lPosXScene, lPosYScene);
                newitem->setPos (QPointF(lPosXScene, lPosYScene));
                newitem->setOpacity(0.0);

                QPropertyAnimation *animationAjout = new QPropertyAnimation (newitem, "opacity");//avec visible ne marche pas alors que c'est une qproperty!!!
                animationAjout->setEasingCurve(QEasingCurve::InOutQuad);
                animationAjout->setDuration(50);
                animationAjout->setStartValue(0.0);
                animationAjout->setEndValue(1.0);
                _groupAnimation->addAnimation(animationAjout);
                int lqHashKey = _ChessManager.qHashKey (lPosX, lPosY);
                lMapPosPiece.insert(lqHashKey, newitem);

            }

        }//else if (lActionDemo->isActionDemoAjout

        else if (lActionDemo->isActionDemoAutoMove (lTypePiece, lListeMovesDemo)) {

            //parcourt les Ã©tapes decomposees du move
            for (unsigned int j= 0; j < lListeMovesDemo.size(); j++) {

                sMoveDemo lMoveDemo = lListeMovesDemo [j];
                //convertit position d'arrivee de la piece en position d'arrivee de la scene
                ConvertPosBoardManagerToPosScene (lMoveDemo._posX, lMoveDemo._posY, lPosXScene, lPosYScene);

                if (lMoveDemo._tempsRelatif == 0.0) {

                    animationMove = 0;
                    item = 0;
                    int lqHashKey = _ChessManager.qHashKey (lMoveDemo._posX, lMoveDemo._posY);
                    if (lMapPosPiece.contains(lqHashKey)) {
                        //parcours ceux qui existent du plus recent au plus ancien
                        QList<QGraphicsPieceItem*> lvalues = lMapPosPiece.values(lqHashKey);
                        for (int i=0; i < lvalues.size (); i++) {
                            QGraphicsPieceItem * litem = lvalues.at(i);
                            if (litem && (litem->getTypePiece() == lTypePiece)) {
                              item = litem;//prend le premier item existant
                              break;
                            }
                        }
                    }

                    if (!item) {
                        item = TrouverPieceAtPosScene (lPosXScene, lPosYScene);
                    }

                    if (item) {                       
                        lPosXDebut = lMoveDemo._posX;
                        lPosYDebut = lMoveDemo._posY;
                        //cree une animation en faisant varier sa QProperty pos
                        animationMove = new QPropertyAnimationDemoMove (QString("%1").arg(i),
                                                                        *this,
                                                                        aAllowInteractionChessBoard,
                                                                        item,
                                                                        "pos");
                        animationMove->setDuration(2000);
                        animationMove->setEasingCurve(QEasingCurve::Linear);
                    }
                }

                if (animationMove)
                    animationMove->setKeyValueAt(lMoveDemo._tempsRelatif, QPointF (lPosXScene, lPosYScene));


                if (lMoveDemo._tempsRelatif == 1.0) {
                    if (animationMove) {

                        lPosXFin = lMoveDemo._posX;
                        lPosYFin = lMoveDemo._posY;

                        int lqHashKey = _ChessManager.qHashKey (lMoveDemo._posX, lMoveDemo._posY);
                        lMapPosPiece.insert(lqHashKey, item);
                        _groupAnimation->addAnimation(animationMove);
                        _SequencesDemo.push_back(eSequenceDemoAnimation);

                    }//if (animationMove)
                }//if (lMoveDemo._tempsRelatif == 1.0)
            }
        }//if (lActionDemo->isActionDemoAutoMove (lTypePiece, lListeMovesDemo))
        else if (CheckIfActionMoveIsPlayer (aDemo.get_ListeActionDemo(), i, lMoveAttendu)) {

            _SequencesDemo.push_back(eSequenceDemoMoveAttendu);
            _ListeMovesAttendus.push_back(lMoveAttendu);
            item = 0;//reinitialise

            //todo factoriser TrouverPieceConcerneePourDemo avec parametre bool aFromMoreRecent
            int lqHashKey = _ChessManager.qHashKey (lMoveAttendu._OldPosX, lMoveAttendu._OldPosY);
            if (lMapPosPiece.contains(lqHashKey)) {
                //parcours ceux qui existent du plus recent au plus ancien
                QList<QGraphicsPieceItem*> lvalues = lMapPosPiece.values(lqHashKey);
                for (int i=0; i < lvalues.size (); i++) {
                    QGraphicsPieceItem * litem = lvalues.at(i);
                    if (litem && (litem->getTypePiece() == lMoveAttendu._TypePiece)) {
                      item = litem;//prend le premier item existant
                      break;
                    }
                }
            }
            if (!item) {
                ConvertPosBoardManagerToPosScene (lMoveAttendu._OldPosX, lMoveAttendu._OldPosY, lPosXScene, lPosYScene);
                item = TrouverPieceAtPosScene (lPosXScene, lPosYScene);
            }
            lMapPosPiece.insert(lqHashKey, item);

        }

    }//for aDemo.get_ListeActionDemo()

    /*****Demarrage animations*****/
    if (_groupAnimation) {
        _groupAnimation->start(QAbstractAnimation::DeleteWhenStopped);
        if (APlayerMoveIsExpected() && _groupAnimation) {

            _AllowUserInteraction = true;
            _groupAnimation->pause();
        }

    }

}

bool ChessBoardScene::CheckIfActionMoveIsPlayer (const QVector<IActionDemo*>& aListeActionsDemo,
                                               int aNextIndex,
                                               sMoveAttendu& aMoveAttendu) {

    if (aNextIndex >= aListeActionsDemo.count()) return false;
    IActionDemo* lActionDemo = aListeActionsDemo [aNextIndex];
    eTypePiece lTypePiece;

    if (lActionDemo->isActionDemoPlayer(lTypePiece,
                                        aMoveAttendu._OldPosX,
                                        aMoveAttendu._OldPosY,
                                        aMoveAttendu._newPosX,
                                        aMoveAttendu._newPosY,
                                        aMoveAttendu._TypePiece)) {
        return true;
    } else
        return false;
}

void ChessBoardScene::OnAnimationFinished() {

    //enleve la sequence
    if (_SequencesDemo.count() >= 1)
        _SequencesDemo.pop_front();
    //met en pause les animations pour attendre le coup attendu (qui resumera les animations)
    if (_groupAnimation && APlayerMoveIsExpected() && _SequencesDemo.contains(eSequenceDemoAnimation)) {

        _AllowUserInteraction = true;
        _groupAnimation->pause();

    }

}



void ChessBoardScene::AnimationsAreFinished () {

    _groupAnimation = 0;
    _AllowUserInteraction = true;
}


void ChessBoardScene::setFreeUseChessBoard (bool aDragOutPossible, bool aModePermissif) {

    _CanMoveOutOfScene = aDragOutPossible;
    _ChessManager.setModePermissif (aModePermissif);
    _RegisterHistory   = !aDragOutPossible;
    ReInit ();
}

void ChessBoardScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event) {

    if (event->mimeData()->hasText()) {
        event->setAccepted(true);
        update();
    } else {
        event->setAccepted(false);
    }
}

void ChessBoardScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event) {
    Q_UNUSED(event);
    update();
}

void ChessBoardScene::dropEvent(QGraphicsSceneDragDropEvent *event) {

    //pas terrible car suppose qu'on connait ce qui vient...
    if (event->mimeData()->hasText()) {

        //deserialisation des donnees
        QString lText = event->mimeData()->text();
        //le message est attendant comme mimeData:data1:data2
        int lPosEndDelimiteur = lText.lastIndexOf(':');
        if (lPosEndDelimiteur == -1) return;
        int lPosDebutDelimiteur = lText.indexOf(':');
        if (lPosDebutDelimiteur == -1) return;
        if (lPosDebutDelimiteur == lPosEndDelimiteur) return;

        //drop d'une piece sur la scene
        if (lText.contains("mimeDataPiece")) {
            GererDropDePiece (lText, lPosDebutDelimiteur, lPosEndDelimiteur, event);
        }
        //drop d'une partie sur la scene
        else if (lText.contains("mimeDataPGNGame")) {
            GererDropDePGNGame (lText, lPosDebutDelimiteur, lPosEndDelimiteur);
        }


    }

}

void ChessBoardScene::GererDropDePiece (const QString& aMessage,
                                        int aPosDebut,
                                        int aPosFin,
                                        QGraphicsSceneDragDropEvent *event) {


    //prend le nom de la piece et determine son genre
    eTypePiece lGenrePiece;
    if (!InfosGraphiques::instance()->get_GenrePiece(QString (aMessage.toStdString().substr(aPosDebut+1, aPosFin-aPosDebut-1).c_str()),
                                                    lGenrePiece)) return;

    //prend ensuite la couleur
    std::stringstream lstream;
    lstream << aMessage.toStdString().substr(aPosFin+1);
    int lColor;
    lstream >> lColor;
    eColorPiece lColorPiece = eColorPiece(lColor);

    //essaie de creer la piece
    int lXChessManager, lYChessManager;

    ConvertPosSceneToPosBoardManager (event->scenePos().x(),
                                      event->scenePos().y(),
                                      lXChessManager,
                                      lYChessManager);

    _ChessManager.ForcerAjoutPiece (lGenrePiece, lColorPiece, lXChessManager, lYChessManager, false);
    update();
}

void ChessBoardScene::GererDropDePGNGame (const QString& aMessage,
                                        int aPosDebut,
                                        int aPosFin) {
    //lecture du fichier
    QString lFileName  = aMessage.mid(aPosDebut+1, aPosFin-aPosDebut-1);
    bool lOk = false;
    int lNoPartie = aMessage.mid(aPosFin+1).toInt(&lOk);
    if (!lOk)
        lNoPartie = 1;

    PGNReaderWriter *pgn = new PGNReaderWriter (lFileName);
    //charger le jeu par l'intermediaire de ILoaderGame
    pgn->LoadGame(*this, lNoPartie);
    QString lInfosGame = pgn->getWhiteName()+"-"+pgn->getBlackName()+ " " + pgn->getResultat();
    _CurrentDemiMove = _ListCommandesPlay.size();
    for (int i=0; i< _ListeViews.size(); i++) {
        _ListeViews [i]->setTitle (QString::fromUtf8(lInfosGame.toStdString().c_str()));
    }
    _FileNameLoaded = lFileName;
    update();
    delete pgn;
}

void ChessBoardScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    QGraphicsScene::dragMoveEvent(event);
    event->accept();
    event->acceptProposedAction();
}

void ChessBoardScene::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if (!_AllowUserInteraction) return;

    QGraphicsScene::mousePressEvent(mouseEvent); //Call the ancestor
    if (mouseEvent->button() == Qt::LeftButton) {

        QGraphicsPieceItem *litemSelected = TrouverPieceAtPosScene(mouseEvent->scenePos().x(), mouseEvent->scenePos().y()); //Get the item at the position
        _InfosItemSelected.Init();
        if (litemSelected) {
            _InfosItemSelected._ItemSelected = litemSelected;
            ConvertPosSceneToPosBoardManager (litemSelected->pos().x(),
                                              litemSelected->pos().y(),
                                              _InfosItemSelected._OldChessManagerPos.first,
                                              _InfosItemSelected._OldChessManagerPos.second);
            //pour redessiner le background
            if (_ModeInfosAllowedMoves) {
                this->invalidate(QRect(0, 0, width(), height()), QGraphicsScene::BackgroundLayer | QGraphicsScene::ItemLayer);
            }
        }
   }
}

void ChessBoardScene::mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent )
{

    if (!_AllowUserInteraction) return;

    //desative le mecanisme pour bouger l'item (flag isMovable)
    QGraphicsScene::mouseMoveEvent(mouseEvent);

      if (_InfosItemSelected._ItemSelected) //If there is an item selected
       {
          if (_InfosItemSelected._ItemSelected->flags().testFlag(QGraphicsItem::ItemIsMovable)) return;//pour prendre le mecanisme de move herite

          _InfosItemSelected._ItemSelected->setCursor(Qt::ArrowCursor);

          QRect sceneRect = this->sceneRect().toRect();

          qreal lPosSceneX = mouseEvent->scenePos().x();
          qreal lPosSceneY = mouseEvent->scenePos().y();


          //calcul nouvelle position dans chess manager
          ConvertPosSceneToPosBoardManager (lPosSceneX,
                                            lPosSceneY,
                                            _InfosItemSelected._NewChessManagerPos.first,
                                            _InfosItemSelected._NewChessManagerPos.second);

          //calcul position scene par rapport au left-top de la piece (fait ici pour test outside)
          ConvertPosBoardManagerToPosScene(_InfosItemSelected._NewChessManagerPos.first,
                                           _InfosItemSelected._NewChessManagerPos.second,
                                           lPosSceneX,
                                           lPosSceneY);


          //verifie si piece en dehors de la scene
          qreal lDeltaSortie = 5;
          bool lOutSide = ( (lPosSceneX < (sceneRect.left()+constants::GLOBAL_CONST_BORDER)) || (lPosSceneX > (sceneRect.right()+lDeltaSortie - _InfosItemSelected._ItemSelected->sceneBoundingRect().width()))
                            || (lPosSceneY < sceneRect.top()) || (lPosSceneY > (sceneRect.bottom()+lDeltaSortie - constants::GLOBAL_CONST_BORDER - _InfosItemSelected._ItemSelected->sceneBoundingRect().height())) );

          _InfosItemSelected._MustBeRemoved = false;
           //si dehors et que pas le droit, on lui met un position limite, sinon, on supprime la piece
          if (lOutSide) {
                if (!_CanMoveOutOfScene) {
                    lPosSceneX = qBound<qreal> (sceneRect.left()+constants::GLOBAL_CONST_BORDER, lPosSceneX, sceneRect.right() - _InfosItemSelected._ItemSelected->sceneBoundingRect().width());
                    lPosSceneY = qBound<qreal> (sceneRect.top(), lPosSceneY, sceneRect.bottom() - constants::GLOBAL_CONST_BORDER - _InfosItemSelected._ItemSelected->sceneBoundingRect().height());
                    ConvertPosSceneToPosBoardManager (lPosSceneX,
                                                      lPosSceneY,
                                                      _InfosItemSelected._NewChessManagerPos.first,
                                                      _InfosItemSelected._NewChessManagerPos.second);
                }else
                    _InfosItemSelected._MustBeRemoved = true;
          }

          CommandPlay *lCommandPreviousPlay = getPreviousCommandPlay();

          //bouge la piece progressivement
          if (_AllowShowAnySquareForMovePiece)
              _InfosItemSelected._ItemSelected->setPos(lPosSceneX, lPosSceneY);


          //si le manager autorise le coup, on prend acte de la nouvelle position, sinon revient Ã  l'ancienne
          if (((_InfosItemSelected._OldChessManagerPos.first == _InfosItemSelected._NewChessManagerPos.first)
                 && (_InfosItemSelected._OldChessManagerPos.second == _InfosItemSelected._NewChessManagerPos.second))
                  || _ChessManager.AllowedMove (_InfosItemSelected._OldChessManagerPos.first,
                                         _InfosItemSelected._OldChessManagerPos.second,
                                         _InfosItemSelected._NewChessManagerPos.first,
                                         _InfosItemSelected._NewChessManagerPos.second,
                                          lCommandPreviousPlay)) {
              if (!_AllowShowAnySquareForMovePiece)
                  _InfosItemSelected._ItemSelected->setPos(lPosSceneX, lPosSceneY);
          }else {
            _InfosItemSelected._ItemSelected->setCursor(Qt::ForbiddenCursor);
          }

      };
}

void ChessBoardScene::mouseReleaseEvent ( QGraphicsSceneMouseEvent *) {

    if (!_AllowUserInteraction) return;

    if (!_InfosItemSelected._ItemSelected) return;

    //pour redessiner le background
    if (_ModeInfosAllowedMoves)
        this->invalidate(QRect(0, 0, width(), height()), QGraphicsScene::BackgroundLayer);

    //remet le curseur
    _InfosItemSelected._ItemSelected->setCursor(Qt::ArrowCursor);

    if (_InfosItemSelected._MustBeRemoved) {
        //on souhaite supprimer l'item seulement de la scene (cas particuliers pour configs)
        if (_ChessManager.Remove (_InfosItemSelected._OldChessManagerPos.first,
                                  _InfosItemSelected._OldChessManagerPos.second)){
            this->removeItem(_InfosItemSelected._ItemSelected);
            delete _InfosItemSelected._ItemSelected;
            _InfosItemSelected.Init();
        }
        return;
    }
    else
    {
        bool lDeplacement = (_InfosItemSelected._NewChessManagerPos.first > 0)
                             && (_InfosItemSelected._NewChessManagerPos.second > 0);
        lDeplacement = lDeplacement
                && ((_InfosItemSelected._OldChessManagerPos.first != _InfosItemSelected._NewChessManagerPos.first)
                    || (_InfosItemSelected._OldChessManagerPos.second != _InfosItemSelected._NewChessManagerPos.second));
        if (!lDeplacement) {
             _InfosItemSelected.Init();
            return;
        }

        qreal lPosSceneX, lPosSceneY;
        bool lOkForPlay = ((!_AlgoComputer)
                            || (_InfosItemSelected._ItemSelected->getColorPiece() != _AlgoComputer->get_ColorComputer()));

        //pose question changer variante slt si pas mode sortir piece et si on change un move precedent
        if (lOkForPlay && !_CanMoveOutOfScene && (_CurrentDemiMove != _ListCommandesPlay.size())) {
            lOkForPlay = AskForNewVariante ();
        }
        //si le manager autorise le coup, on prend acte de la nouvelle position, sinon revient Ã  l'ancienne
        if (lOkForPlay
                && CheckIfExpectedMoveAndGoodOne(_InfosItemSelected._OldChessManagerPos.first,
                                         _InfosItemSelected._OldChessManagerPos.second,
                                         _InfosItemSelected._NewChessManagerPos.first,
                                         _InfosItemSelected._NewChessManagerPos.second,
                                                 false)
                && PlayMove (_InfosItemSelected._OldChessManagerPos.first,
                             _InfosItemSelected._OldChessManagerPos.second,
                             _InfosItemSelected._NewChessManagerPos.first,
                             _InfosItemSelected._NewChessManagerPos.second)) {

            //verifie si move attendu change de type de piece (promotion)
            if (!CheckIfExpectedMoveAndGoodOne (_InfosItemSelected._OldChessManagerPos.first,
                                                _InfosItemSelected._OldChessManagerPos.second,
                                                _InfosItemSelected._NewChessManagerPos.first,
                                                _InfosItemSelected._NewChessManagerPos.second,
                                                true)) {
                Rollback (1);
                EraseRollBackMoves();
                //deselectionner l'item
                _InfosItemSelected.Init();
                return;

            }
            if (_InfosItemSelected._ItemSelected) {
                qreal lPosSceneX, lPosSceneY;
                ConvertPosBoardManagerToPosScene(_InfosItemSelected._NewChessManagerPos.first,
                                                _InfosItemSelected._NewChessManagerPos.second,
                                                lPosSceneX,
                                                lPosSceneY);
               _InfosItemSelected._ItemSelected->setPos(lPosSceneX, lPosSceneY); //peut pas Ãªtre fait sur notification car piece bougee Ã  la main
            }


           //rÃ©-initialise la gestion des animations
            ReinitGestionAnimationsOnPlay () ;

        }else {

            ConvertPosBoardManagerToPosScene(_InfosItemSelected._OldChessManagerPos.first,
                                             _InfosItemSelected._OldChessManagerPos.second,
                                             lPosSceneX,
                                             lPosSceneY);
            _InfosItemSelected._ItemSelected->setPos (lPosSceneX, lPosSceneY);

        }
    }

    //deselectionner l'item
    _InfosItemSelected.Init();

}

bool ChessBoardScene::APlayerMoveIsExpected () {

    return ((_SequencesDemo.count()>0) && (_SequencesDemo[0] == eSequenceDemoMoveAttendu) && (_ListeMovesAttendus.count()>0));
}

bool ChessBoardScene::CheckIfExpectedMoveAndGoodOne (int aOldPosX, int aOldPosY,
                                                     int anewPosX, int anewPosY,
                                                     bool aCheckTypePiece) {

    if (APlayerMoveIsExpected ()) {

        bool lMoveOK =  ((aOldPosX == _ListeMovesAttendus[0]._OldPosX)
                          && (aOldPosY == _ListeMovesAttendus[0]._OldPosY)
                          && (anewPosX == _ListeMovesAttendus[0]._newPosX)
                          && (anewPosY == _ListeMovesAttendus[0]._newPosY));
        ArticlePiece lArticlePiece;
        if (lMoveOK && aCheckTypePiece && _ChessManager.UnePieceExiste(anewPosX, anewPosY, lArticlePiece)) {
            lMoveOK = (_ListeMovesAttendus[0]._TypePiece == lArticlePiece._TypePiece);
        }
        if (!lMoveOK) {
            QMessageBox msgBox;
            msgBox.setText(tr("That is not the expected move"));
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
        }

        return lMoveOK;
    }
    else return true;
}

void ChessBoardScene::ReinitGestionAnimationsOnPlay () {

    if (APlayerMoveIsExpected ()) {

        //enleve la sequence attendue
        _SequencesDemo.pop_front();
        //enleve le move attendu
        _ListeMovesAttendus.pop_front();
        //si prochaine sequence pas mvt attendu
        if (_groupAnimation && !APlayerMoveIsExpected ()) {

             _AllowUserInteraction = false;
            //resume les animations
            _groupAnimation->resume();
        }
    }
}


void ChessBoardScene::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    if (!_AllowUserInteraction) return;

    const int CAngleModulo = 120;//(15*8); 15 degres is a standard mouse wheel step, the unit for delta is 1/8 of degree
    int lNbMoves = (event->delta() / CAngleModulo);

    try {
        if (lNbMoves > 0) {
          Replay (lNbMoves);
        } else if (lNbMoves < 0){
          Rollback (qAbs<int> (lNbMoves));
        }
    }catch(...) {

#ifdef _DEBUG
    qDebug()<<"PROBLEME DANS LE WHEEL";
#endif

    }
}

CommandPlay* ChessBoardScene::getPreviousCommandPlay() const {

    if ((_CurrentDemiMove > 0) && (_CurrentDemiMove <= _ListCommandesPlay.size()))
        return _ListCommandesPlay [_CurrentDemiMove -1];
    else
        return 0;
}

void ChessBoardScene::ChangeCurrentCommentary (const QString& anewCommentary) {

    CommandPlay *lCommand;
    int lindex = _CurrentDemiMove -1;
    if ((lindex >= 0) && (_ListCommandesPlay.size() > lindex)) {
        lCommand = _ListCommandesPlay [lindex];
        if (lCommand) {
            if (lCommand->getCommentaryMiseEnForme() == anewCommentary) return;
            lCommand->setCommentary("{"+anewCommentary+"}");
        }
    }
}

void ChessBoardScene::ActualiserInfosAffichageVue () {

    CommandPlay *lCommand;
    int lindex = _CurrentDemiMove -1;
    if ((lindex >= 0) && (_ListCommandesPlay.size() > lindex)) {
           lCommand = _ListCommandesPlay [lindex];
           if (lCommand) {
               QString lInfoCoup;
               if (_CurrentDemiMove%2 == 0)
                   lInfoCoup = QString::number(qMax<int>(_CurrentDemiMove/2, 1)) + "... ";
               else
                   lInfoCoup = QString::number((_CurrentDemiMove/2)+1) + ". ";
               lInfoCoup += lCommand->getDescription();
               DoOnInfosChanged (lInfoCoup, lCommand->getCommentaryMiseEnForme());
           }
       }else
        DoOnInfosChanged ("", "");
}

void ChessBoardScene::Rollback (int aNbreMoves) {

    if (_CurrentDemiMove <= 0) return;
    int lNMovesBack = 0;
    int lindex = _CurrentDemiMove -1;
    CommandPlay *lCommand;
    while ((lNMovesBack < aNbreMoves) && (_CurrentDemiMove >=1 )) {
        if ((lindex >=0) && (_ListCommandesPlay.size() > lindex)) {
            lCommand = _ListCommandesPlay [lindex];
            if (lCommand)
                if (lCommand->UnExecute())
                    _CurrentDemiMove--;
            lindex--;
        }
        else break;
        lNMovesBack++;
    }

    if (lNMovesBack > 0)
        ActualiserInfosAffichageVue ();
}

void ChessBoardScene::Replay (int aNbreMoves) {

    if (_CurrentDemiMove < 0) return;
    aNbreMoves = qMin<int> (_ListCommandesPlay.size() - _CurrentDemiMove, aNbreMoves);
    if (aNbreMoves <= 0) return;

    CommandPlay *lCommand;
    int lindex = _CurrentDemiMove;//on commence au coup suivant l'actuel
    for (int i = 1; i <= aNbreMoves; i++) {
        lCommand = _ListCommandesPlay [lindex];
        lindex ++;
        if (lCommand) {
            if (lCommand->Execute())
                _CurrentDemiMove++;
        }
    }//for

    if (aNbreMoves > 0)
        ActualiserInfosAffichageVue ();
}

void ChessBoardScene::EraseRollBackMoves() {

    for (int i= _CurrentDemiMove; i < _ListCommandesPlay.size(); i++) {
        delete _ListCommandesPlay [i];
    }
    _ListCommandesPlay.resize(_CurrentDemiMove);
}

bool ChessBoardScene::AskForNewVariante () {

    QMessageBox msgBox;
    msgBox.setText(tr("The previous moves will be lost."));
    msgBox.setInformativeText(tr("Are you sure you want a new variant ?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    switch (ret) {
      case QMessageBox::Yes:
            //on va reinitialiser Ã  partir de currentmove les prochains coups
            EraseRollBackMoves();
            return true;
          break;
      case QMessageBox::No:
          // Don't Save was clicked
        return false;
          break;
    }
    return false;
}

void ChessBoardScene::ReInit() {

    _SequencesDemo.clear();
    _ListeMovesAttendus.clear();
    _FileNameLoaded = "";
    for (QVector<CommandPlay*>::const_iterator it =_ListCommandesPlay.begin();
         it != _ListCommandesPlay.end();
         ++it) {
        delete (*it);
    }
    _ListCommandesPlay.clear();
    _CurrentDemiMove = 0;
    _InfosItemSelected.Init();
    ActualiserInfosAffichageVue ();
}

void ChessBoardScene::InitializeGame(bool aCreationPiece) {

    ReInit();
    if (StopAllComputations()) {
        _ChessManager.ResetPieces();//ne peut pas mettre dans ReInit doOnResetPieces appelle Reinit
        if (aCreationPiece)
            _ChessManager.CreationDesPieces();
    }
}

void ChessBoardScene::AjouterDemiCoup (CommandPlay* aCommandPlay) {

    _ListCommandesPlay.push_back(aCommandPlay);
     _CurrentDemiMove++;
     ActualiserInfosAffichageVue();

#ifdef _DEBUG
     //pour debug
     if (_CurrentDemiMove==53) {//2*(N-1)+1 pour noir
         _AlgoComputer = new AlgoComputer (_ChessManager, BLACK);
         sMove lMoveSelected;
         _AlgoComputer->FindMoveToPlay(_CurrentDemiMove, aCommandPlay, lMoveSelected);
     }
#endif
}

IChessManager &ChessBoardScene::getChessManager() {

    return dynamic_cast<IChessManager&>(_ChessManager);

}

void ChessBoardScene::RegisterObserver (ObserverChessManager &aObserver) {

    _ChessManager.RegisterObserver(aObserver);

}

void ChessBoardScene::SaveGame () const {

    //ouvre le dernier fichier ouvert si existant, on Ã©crira Ã  la suite du fichier
    PGNReaderWriter *pgn = new PGNReaderWriter (_FileNameLoaded);

    QString lresultat  = "";
    QString lWhiteName = "";
    QString lBlackName = "";
    if (_AlgoComputer)
    {
        if (_AlgoComputer->get_ColorComputer() == WHITE)
            lWhiteName = tr("COMPUTER");
        else
            lBlackName = tr("COMPUTER");

        if (_AlgoComputer->get_resultGame() == eWhiteWins)
          lresultat = "1-0";
        else if (_AlgoComputer->get_resultGame() == eBlackWins)
          lresultat = "0-1";
        else
          lresultat = "1/2-1/2";
    }
    sDataTypeEntete lDataEntete (pgn->getEvent(),
                                 pgn->getSite(),
                                 pgn->getRound(),
                                 pgn->getDate(),
                                 lWhiteName,
                                 lBlackName,
                                 pgn->getWhiteELO(),
                                 pgn->getBlackELO(),
                                 lresultat);

    //ouvre boite dialogue pour choix entete
    DialogAskEnteteFichierPGN ldialogue (lDataEntete);
    if (ldialogue.exec() == QDialog::Accepted) {

        //recupere la nouvelle entete
        ldialogue.getEnTeteData (lDataEntete);
        //ouvre boite de dialogue pour choix nom fichier PGN
        QString lFileToSave = QFileDialog::getSaveFileName(0, tr("Save to File (.PGN)"),
                                            InfosConfiguration::instance()->getLastDirForPGNFiles(),
                                            "(*.PGN)", 0, QFileDialog::DontConfirmOverwrite);
        pgn->WriteGame (lFileToSave,
                        lDataEntete,
                        _ListCommandesPlay);
        //Actualiser l'affichage courant
        Affichages::ActualiserAffichageCourant();
    }

    delete pgn;
}

void ChessBoardScene::DoOnInfosChanged (const QString& aDescriptionMove, const QString& aCommentary) const {

    //transmet les infos Ã  la vue
    for (int i = 0; i < _ListeViews.size(); i++) {
        _ListeViews [i]->DrawComments(aDescriptionMove, aCommentary);
    }
}


void ChessBoardScene::AddStaticPieceAndShowAllowedDirections (const ArticlePiece& aPiece) {

    tlisteArticlesPieces lPieces;
    lPieces.push_back(aPiece);
    AddStaticPiecesWithoutShowingDirections (lPieces);

    //pour que le mecanisme de drawbackground fonctionne
    _ModeShowAllowedPath = true;
    _InfosItemSelected._ItemSelected = TrouverPiece(aPiece._TypePiece, aPiece._Color, true, aPiece._PosX, aPiece._PosY);
    this->invalidate(QRect(0, 0, width(), height()), QGraphicsScene::BackgroundLayer);
}

void ChessBoardScene::AddStaticPiecesWithoutShowingDirections (const tlisteArticlesPieces& aListePieces) {

    DescriptorDemo lDescDemo;
    for (tlisteArticlesPieces::const_iterator it = aListePieces.begin();
         it != aListePieces.end();
         ++it) {
        lDescDemo.AddPiece(*it);
    }
    setDemo(lDescDemo, false);
}

void ChessBoardScene::setModePlayAgainstComputer (IObserverAlgo& aObserver,
                                                  eColorPiece aColorComputer,
                                                  bool aModeComputerAgainstComputer) {

    _ModeComputerAgainstComputer = aModeComputerAgainstComputer;

    if (!_AlgoComputer) {
        _AlgoComputer = new AlgoComputer (_ChessManager, aColorComputer);
        _AlgoComputer->RegisterObserver (&aObserver);
    }

    if (_AlgoComputer) {
        //si l'utilisateur change de couleur pour faire jouer l'algo
        CommandPlay* lPreviousCommandPlay = getPreviousCommandPlay();
        bool lComputerMustPlay = (lPreviousCommandPlay == 0)
                                  && (aColorComputer == WHITE);
        lComputerMustPlay = lComputerMustPlay ||
                             (_AlgoComputer->get_ColorComputer() != aColorComputer);

        _AlgoComputer->setColorComputer (aColorComputer);
        _ChessManager.setInterdireAskPiecePromotionFor (tQVectorColorPiece (1, aColorComputer));
        if (lComputerMustPlay)
            LaunchAlgo(lPreviousCommandPlay);
    }
    //todo pour test
 //   PlayMove (5, 2, 5, 4);

    for (int i = 0; i < _ListeViews.size(); i++) {
        _ListeViews [i]->DoOnModePlayAgainstComputer (aColorComputer);
    }

}

bool ChessBoardScene::StopAllComputations() {

    return (!_threadAlgo || _threadAlgo->StopComputation());
}

void ChessBoardScene::AllowPlayFirstBlack (bool aAllow) {

    _ChessManager.setAllowFirstPlayBlack (aAllow);
}

void ChessBoardScene::AllowShowAnySquareForMovePiece (bool aAllow) {

    _AllowShowAnySquareForMovePiece = aAllow;
}

void ChessBoardScene::AllowUserInteraction (bool aAllow) {

    _AllowUserInteraction = aAllow;
}

void ChessBoardScene::OnStartAlgoComputation() {

    _AllowUserInteraction = false;
    if (_AlgoComputer)
       _AlgoComputer->NotifierBeginComputation();

}

bool ChessBoardScene::PlayMove (int aOldPosX, int aOldPosY, int aNewPosX, int aNewPosY) {

    if (_AlgoComputer)
       _AlgoComputer->NotifierEndComputation();

    CommandPlay *lCommandPlay;
    return (_ChessManager.Play(aOldPosX, aOldPosY,
                               aNewPosX, aNewPosY,
                               lCommandPlay,
                                getPreviousCommandPlay()));

}

void ChessBoardScene::DoComputerMove(int aOldPosX, int aOldPosY, int aNewPosX, int aNewPosY) {

   _AllowUserInteraction = false;//pas de mvts manuels autorises tant que mouvement pas fait

    qreal lPosXScene, lPosYScene;
    ConvertPosBoardManagerToPosScene (aOldPosX, aOldPosY, lPosXScene, lPosYScene);
    QGraphicsPieceItem* item = TrouverPieceAtPosScene (lPosXScene, lPosYScene);
    if (item) {
        QSequentialAnimationGroup* lgroupAnimation = new QSequentialAnimationGroup(this);

        //cree une animation en faisant varier sa QProperty pos
        QPropertyAnimationDemoMove* lanimationMove = new QPropertyAnimationDemoMove ("computer", *this, true, item, "pos");
        lanimationMove->setDuration(500);
        lanimationMove->setEasingCurve(QEasingCurve::Linear);
        lanimationMove->setKeyValueAt(0, QPointF (lPosXScene, lPosYScene));

        //move special Knight
        if (item->getTypePiece() == Knight) {

           int ldiffX = qAbs<int> (int(aOldPosX)-int(aNewPosX));
           int ldiffY = qAbs<int> (int(aOldPosY)-int(aNewPosY));
           if (ldiffX > ldiffY) {
               ConvertPosBoardManagerToPosScene (aOldPosX, aNewPosY, lPosXScene, lPosYScene);
           }else {
               ConvertPosBoardManagerToPosScene (aNewPosX, aOldPosY, lPosXScene, lPosYScene);
           }
           lanimationMove->setKeyValueAt(0.5, QPointF (lPosXScene, lPosYScene));

        }//if (item->getTypePiece() == Knight)


        ConvertPosBoardManagerToPosScene (aNewPosX, aNewPosY, lPosXScene, lPosYScene);
        lanimationMove->setKeyValueAt(1, QPointF (lPosXScene, lPosYScene));
        lgroupAnimation->addAnimation(lanimationMove);

        lgroupAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }

}


void ChessBoardScene::DoOnComputerHasNoMove () const {

    if (_AlgoComputer) {

       CommandPlay *lPrevious = getPreviousCommandPlay();
       //si suite Ã  Ã©chec , pas de coups possible alors c'est mat sinon c'est pat!
       if (lPrevious->KingEnnemyIsChecked()) {

           _AlgoComputer->setAlgoLose ();
            //transmet les infos Ã  la vue
            for (int i = 0; i < _ListeViews.size(); i++) {
                _ListeViews [i]->DoOnCheckMate ();
            }
       }else
            _AlgoComputer->setEqualityResult();
    }

}


void ChessBoardScene::DoOnEnnemyComputerHasNoMoveAfter(int aOldPosX, int aOldPosY, int aNewPosX, int aNewPosY, bool aGivesCheck) {

    if (_AlgoComputer) {

       DoComputerMove (aOldPosX, aOldPosY, aNewPosX, aNewPosY);

       if (aGivesCheck)
         _AlgoComputer->setAlgoWins();
       else
        _AlgoComputer->setEqualityResult();
    }
}

void ChessBoardScene::NettoyageThreadAlgo () {

    if (_threadAlgo) {
        delete _threadAlgo;
        _threadAlgo = 0;
    }
    _AllowUserInteraction = true;
}




