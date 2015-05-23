#include "chessboardview.h"
#include <QCheckBox>
#include "chessboardscene.h"
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include "descriptordemo.h"



ChessBoardView::ChessBoardView(QWidget *parent) : QGraphicsView (parent),
    _CurrentRotationAngle(0)
{
    setRenderHint(QPainter::HighQualityAntialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);

    _scene = new ChessBoardScene (this);//, QRectF(-20, 0, 401, 401+20));

    //ajoute la scene
    setScene (_scene);

    QSizePolicy lpolicy = this->sizePolicy();
    lpolicy.setHeightForWidth(true);
    this->setSizePolicy(lpolicy);

}

ChessBoardView::~ChessBoardView() {
   // delete _scene;
}

void ChessBoardView::resizeEvent (QResizeEvent *event) {


//    qreal lAspectRatio = 1;//event->oldSize().height()/event->oldSize().width();
//    if ((event->size().height()/event->size().width()) != lAspectRatio) {
//        QSize lnewSize;
//        lnewSize.setWidth(event->size().width());
//        lnewSize.setHeight(lAspectRatio*lnewSize.width());
//        QResizeEvent lnewEvent (lnewSize, event->oldSize());

//        //QGraphicsView::resizeEvent(&lnewEvent);
//        resize (lnewSize);
//    }
    QGraphicsView::resizeEvent(event);
    //pour ajuster la taille de la scene Ã  la vue
    ensureVisible (sceneRect());
    fitInView(sceneRect(), Qt::KeepAspectRatio);
}

void ChessBoardView::setDemo (const DescriptorDemo& aDemo, bool aAllowInteractionChessBoard) {

    this->SetModeRotation(aDemo.UseRotation ());

    if (_scene) _scene->setDemo (aDemo, aAllowInteractionChessBoard);
}

void ChessBoardView::setFreeUseChessBoard (bool aDragOutPossible, bool aModePermissif) {

    if (_scene) _scene->setFreeUseChessBoard (aDragOutPossible, aModePermissif);
}

void ChessBoardView::SetModeRotation (bool aModeRotation) {

   //il faut tourner la scene de 180 degres Ã  chaque fois
    if ((aModeRotation && (_CurrentRotationAngle==0)) || ((!aModeRotation  && (_CurrentRotationAngle != 0)))) {
        this->rotate(180);
        if (_CurrentRotationAngle != 0)
            _CurrentRotationAngle = 0;
        else
            _CurrentRotationAngle = 180;
        if (_scene)
             _scene->SetModeRotation(aModeRotation);
    }

}

void ChessBoardView::setModeInfoAllowedMoves (bool aInfoAllowedMoves) {

    if (_scene)
        _scene->setModeInfoAllowedMoves(aInfoAllowedMoves);
}

void ChessBoardView::FillBoard () {

    if (_scene)
        _scene->InitializeGame(true);
}

void ChessBoardView::EmptyBoard () {

    if (_scene)
        _scene->InitializeGame(false);
}

bool ChessBoardView::getArticlesPiecesDeCouleur (eColorPiece aColorPiece,
                                                 tlisteArticlesPieces& aListeArticles) const {
    if (_scene)
        return _scene->getArticlesPiecesDeCouleur(aColorPiece, aListeArticles);
    else
        return false;
}

void ChessBoardView::RegisterObserver (ObserverChessManager &aObserver) {

    if (_scene)
        _scene->RegisterObserver(aObserver);
}

void ChessBoardView::RegisterView (IChessBoardView& aView) {

    if (_scene)
        _scene->RegisterView(aView);
}

void ChessBoardView::ChangeCommentary (const QString& aCommentary) const {

    if (_scene)
        _scene->ChangeCurrentCommentary(aCommentary);
}

IControleur& ChessBoardView::getControleur() const {

    return *_scene;

}

void ChessBoardView::contextMenuEvent ( QContextMenuEvent * event ) {

    //pas de menu contextuel si une piece est selectionnee pour bouger
    if (_scene && _scene->IsMovingPiece ()) return;
    QMenu menu;
    QAction *ActionSaveGame = menu.addAction (tr("&Save the game"));
    connect(ActionSaveGame, SIGNAL(triggered()), this, SLOT(SaveGame()));
    menu.exec(event->globalPos());
}

void ChessBoardView::SaveGame () {

    if (_scene)
        _scene->SaveGame();
}

void ChessBoardView::AddStaticPieceAndShowAllowedDirections (const ArticlePiece& aPiece) {

    if (_scene)
        _scene->AddStaticPieceAndShowAllowedDirections(aPiece);
}

void ChessBoardView::AddStaticPiecesWithoutShowingDirections (const tlisteArticlesPieces& aListePieces) {

    if (_scene)
        _scene->AddStaticPiecesWithoutShowingDirections(aListePieces);
}



