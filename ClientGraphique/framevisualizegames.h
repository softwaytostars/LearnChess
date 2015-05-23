#ifndef FRAMEVISUALIZEGAMES_H
#define FRAMEVISUALIZEGAMES_H

#include <QFrame>
#include <QTableView>
#include <QStandardItemModel>
#include "ClientGraphique/IActualisable.hpp"
#include "ClientGraphique/IUserTraductions.h"

namespace Ui {
class FrameVisualizeGames;
}

class FrameMultiGames;
class QSplitter;

class FrameVisualizeGames : public QFrame,
                            public IActualisable,
                            public IUserTraductions
{
    Q_OBJECT
    
public:
    explicit FrameVisualizeGames(QWidget *parent = 0);
    ~FrameVisualizeGames();
    virtual void Actualiser();
    virtual void Translate();

private:
    //inner class pour la table vue derivee
    class QTableViewGame : public QTableView
    {
    public:
        explicit QTableViewGame(QWidget *parent);
        void DeleteSelection ();
    protected:
        virtual void startDrag(Qt::DropActions);
        virtual void dragMoveEvent(QDragMoveEvent *event);
////        //pour gestion d'un menu contextuel
//        void contextMenuEvent ( QContextMenuEvent * event );

    };

    //inner class pour le model
    class GameInfosModel : public QStandardItemModel {
    public:
        explicit GameInfosModel(QObject *parent = 0);
        virtual QMimeData *mimeData ( const QModelIndexList & indexes ) const;

    };

private:
    Ui::FrameVisualizeGames *ui;
    QTableViewGame *_tableViewGame;
    GameInfosModel* _SourceModel;
    FrameMultiGames* _frameMultigames;
    QSplitter *_splitter;
    QString _MessageFileSearchedIn;
    void FillSourceModel();
private slots:
    void ContextMenuTableView (const QPoint& );
    void OnDoubleClick (const QModelIndex& aModelIndex);
    void DeleteViewSelection ();
};

#endif // FRAMEVISUALIZEGAMES_H
