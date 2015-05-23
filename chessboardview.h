#ifndef CHESSBOARDVIEW_H
#define CHESSBOARDVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include "IChessBoardView.hpp"

class ChessBoardScene;

class ChessBoardView : public QGraphicsView, public IChessBoardView
{
    Q_OBJECT
public:
    ChessBoardView(QWidget * parent);
    virtual ~ChessBoardView();

    virtual void setDemo (const DescriptorDemo& aDemo, bool aAllowInteractionChessBoard);
    virtual void setFreeUseChessBoard (bool aDragOutPossible, bool aModePermissif);
    virtual void SetModeRotation (bool aModeRotation);
    virtual void setModeInfoAllowedMoves (bool aInfoAllowedMoves);
    virtual void FillBoard ();
    virtual void EmptyBoard ();
    virtual bool getArticlesPiecesDeCouleur (eColorPiece aColorPiece,
                                             tlisteArticlesPieces &aListeArticles) const;
    virtual void RegisterObserver (ObserverChessManager &aObserver);
    virtual void setTitle(const QString&) {}
    virtual void DrawComments (const QString& , const QString&) {}
    virtual void DoOnCheckMate (){}
    virtual void RegisterView (IChessBoardView& aView);
    virtual void ChangeCommentary (const QString& aCommentary) const;
    virtual IControleur& getControleur() const;

    /**
     * @brief Pour dessiner sur la scene les directions possibles de la piece à ajouter
     *
     * @param aPiece
     */
    void AddStaticPieceAndShowAllowedDirections (const ArticlePiece& aPiece);

    /**
     * @brief Pour dessiner des pieces statiques sans autres infos
     *
     * @param aListePieces
     */
    void AddStaticPiecesWithoutShowingDirections (const tlisteArticlesPieces& aListePieces);

protected:
    virtual void resizeEvent (QResizeEvent * event);

    /**
     * @brief Pour gerer un menu contextuel (right click on the view)
     *
     * @param event
     */
    virtual void contextMenuEvent ( QContextMenuEvent * event );
private:
    ChessBoardView();
    ChessBoardScene *_scene;
    int _CurrentRotationAngle;//puisque je trouve pas comment recuperer la rotation d'un qgraphicsviuew!!!
private slots:
    void SaveGame () ;
};

#endif // CHESSBOARDVIEW_H
