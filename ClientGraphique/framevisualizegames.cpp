#include "framevisualizegames.h"
#include "ui_framevisualizegames.h"

#include <QStandardItemModel>
#include <QDir>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QPushButton>
#include <QDragMoveEvent>
#include <QMenu>

#include "ImportExport/pgnreaderwriter.h"
#include "infosconfiguration.h"
#include "infosgraphiques.h"
#include "framemultigames.h"
#include "dialogaskentetefichierpgn.h"

/************************************************************************************/
/******Definition de l'objet proxy model pour gerer le tri et le filtre du modele****/
/************************************************************************************/

static const int CNumColWhiteName = 0;
static const int CNumColBlackName = 1;
static const int CNumColWhiteELO  = 2;
static const int CNumColBlackELO  = 3;
static const int CNumColResult    = 4;
static const int CNumColEvent     = 5;
static const int CNumColDate      = 6;
static const int CNumColSite      = 7;
static const int CNumColFilename  = 8;//col bidon servant juste à stocker nom de fichier (sera cachee)
static const int CNumColNoPartie  = 9;//col bidon servant juste à stocker numero de partie (sera cachee)


FrameVisualizeGames::GameInfosModel::GameInfosModel(QObject *parent):
    QStandardItemModel (parent) {

    this->setSupportedDragActions(Qt::MoveAction);
}
QMimeData* FrameVisualizeGames::GameInfosModel::mimeData ( const QModelIndexList & indexes ) const {

    if (indexes.size() <= 0) return 0;

    //cree objet contenant infos pour le drag
    QMimeData *mimeData = new QMimeData();

    bool lOk = false;
    int lNoPartie = this->index(indexes [0].row(), CNumColNoPartie).data().toInt(&lOk);
    if (!lOk)
        lNoPartie = 1;
    mimeData->setText("mimeDataPGNGame:"+
                      this->index(indexes [0].row(), CNumColFilename).data().toString()+
                      ":"+
                      QString("%1").arg(lNoPartie));
    return mimeData;
}


/*********************************************************************/
/**** definition pour le proxy du modele servant à trier et filter****/
/*********************************************************************/

class GamesSortProxyModel : public QSortFilterProxyModel {
public:
    GamesSortProxyModel (QObject *parent = 0);
protected:
     virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};

GamesSortProxyModel::GamesSortProxyModel (QObject *parent)
     : QSortFilterProxyModel(parent) {
}


bool GamesSortProxyModel::lessThan(const QModelIndex &left,
                                      const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);

    QString leftString = leftData.toString();
    QString rightString = rightData.toString();

    return QString::localeAwareCompare(leftString, rightString) < 0;
}


/************************************************/
/**********definition du tableau ****************/
/************************************************/

FrameVisualizeGames::QTableViewGame::QTableViewGame(QWidget *parent)
    : QTableView (parent)
{
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    //pas de hightlight des header si selection
    this->horizontalHeader()->setHighlightSections(false);
    this->verticalHeader()->setHighlightSections(false);

}

void FrameVisualizeGames::QTableViewGame::startDrag(Qt::DropActions) {

    QMimeData *mimeData = this->model()->mimeData(this->selectedIndexes());
    //cree l'objet drag
    QDrag *drag = new QDrag(this);
    //lui associe les data
    drag->setMimeData(mimeData);
    //execute le drag
    drag->exec();
}

void FrameVisualizeGames::QTableViewGame::dragMoveEvent(QDragMoveEvent *event) {

    event->accept();
}

void FrameVisualizeGames::QTableViewGame::DeleteSelection () {

    QModelIndexList lListeRows = this->selectionModel()->selectedRows();
    for (QModelIndexList::const_iterator it = lListeRows.begin();
         it != lListeRows.end();
         ++it){
        QString lFileName = this->model()->index((*it).row(), CNumColFilename).data().toString();
        bool lOk;
        int lNoPartie = this->model()->index((*it).row(), CNumColNoPartie).data().toInt(&lOk);
        if (!lOk) lNoPartie = 1;

        if (lFileName.size() > 0) {
            PGNReaderWriter pgn (lFileName);
            pgn.DeleteGame(lNoPartie);
        }

    }
}


/************************************************/
/**********definition de la frame****************/
/************************************************/

FrameVisualizeGames::FrameVisualizeGames(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameVisualizeGames)
{
    ui->setupUi(this);
    InfosGraphiques::instance()->AbonnerUtilisateurTraductions(this);

    setLayout(ui->MainHLayout);

    _tableViewGame = new QTableViewGame (this);
    _tableViewGame->setEditTriggers(QAbstractItemView::NoEditTriggers);//pas d'edition

    //met un menu contextuel (mis ici car pas de slots possible dans inner class)
    _tableViewGame->setContextMenuPolicy(Qt::CustomContextMenu);
    connect (_tableViewGame, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT (ContextMenuTableView (QPoint)));
    connect (_tableViewGame, SIGNAL (doubleClicked(const QModelIndex&)), this, SLOT (OnDoubleClick (const QModelIndex&)));

    //ajoute un splitter (separateur pour varier size frame)
    _splitter = new QSplitter (Qt::Horizontal, this);//split horizontal

   // splitter->setCursor(Qt::SplitHCursor);//met curseur horiz
    _splitter->addWidget(_tableViewGame);
    //ajout de la frame pour les chessboard
    _frameMultigames = new FrameMultiGames(this);
    _splitter->addWidget(_frameMultigames);
    ui->VLayoutGames->addWidget(_splitter);

    //permet le tri sur clic des titres
    _tableViewGame->setSortingEnabled(true);

    //permet la gestion du drag&drop
    _tableViewGame->setCursor(Qt::OpenHandCursor);
    _tableViewGame->setDragEnabled(true);
    _tableViewGame->setDragDropMode(QAbstractItemView::DragOnly);


    //creation du model source
    _SourceModel = new GameInfosModel(this);

    GamesSortProxyModel *lGamesSortProxyModel = new GamesSortProxyModel (this);
    lGamesSortProxyModel->setSourceModel(_SourceModel);
    _tableViewGame->setModel(lGamesSortProxyModel);

    Translate();
}

void FrameVisualizeGames::ContextMenuTableView (const QPoint&) {

    QMenu menu;
    QAction *ActionDelete = menu.addAction (tr("&Delete game"));
    connect(ActionDelete, SIGNAL(triggered()), this, SLOT(DeleteViewSelection()));
    menu.exec(QCursor::pos());
}

void FrameVisualizeGames::OnDoubleClick (const QModelIndex& aModelIndex) {

    const QString& lFileName = _SourceModel->index(aModelIndex.row(), CNumColFilename).data().toString();
    bool lOk;
    int lNoPartie = _SourceModel->index(aModelIndex.row(), CNumColNoPartie).data().toInt(&lOk);
    if (!lOk) lNoPartie = 1;

    PGNReaderWriter pgn (lFileName);
    pgn.ReadEntete(lNoPartie);

    sDataTypeEntete lDataEntete (pgn.getEvent(),
                                 pgn.getSite(),
                                 pgn.getRound(),
                                 pgn.getDate(),
                                 pgn.getWhiteName(),
                                 pgn.getBlackName(),
                                 pgn.getWhiteELO(),
                                 pgn.getBlackELO(),
                                 pgn.getResultat());

    //ouvre boite dialogue pour choix entete
    DialogAskEnteteFichierPGN ldialogue (lDataEntete);
    if (ldialogue.exec() == QDialog::Accepted) {
        //recupere la nouvelle entete
        ldialogue.getEnTeteData (lDataEntete);
        pgn.OverWriteEntete(lDataEntete, lNoPartie);
        Actualiser();
    }

}

void FrameVisualizeGames::DeleteViewSelection () {

    _tableViewGame->DeleteSelection();
    Actualiser();
}


void FrameVisualizeGames::Actualiser () {

    ui->labelDirFiles->setText(_MessageFileSearchedIn+InfosConfiguration::instance()->getLastDirForPGNFiles());
    FillSourceModel ();
    _frameMultigames->ReInit();

}

FrameVisualizeGames::~FrameVisualizeGames()
{
    delete ui;
    InfosGraphiques::instance()->DesabonnerUtilisateurTraductions(this);
}

void FrameVisualizeGames::Translate() {

    ui->labelDirFiles->setToolTip(tr("You can change the repertory by the menu File/Change default PGN directory"));
    _tableViewGame->setToolTip(tr("You can drag a line on the chessboard for loading the game"));
    _splitter->setToolTip(tr("You can change the dimensions of the frame"));
    _MessageFileSearchedIn = tr("The PGN files are searched in :");
}

void FrameVisualizeGames::FillSourceModel() {


    //lit le nbre de fichiers contenus dans le répertoire
    QDir dir (InfosConfiguration::instance()->getLastDirForPGNFiles());
    dir.setFilter(QDir::Files);
    QStringList filters;
    filters << "*.PGN" << "*.pgn";
    dir.setNameFilters(filters);
    QFileInfoList list = dir.entryInfoList();
    int lNbreGames = list.size();

    _SourceModel->clear();
    _SourceModel->setColumnCount(10);
//    _tableViewGame->reset();

    //titre des colonnes
    _SourceModel->setHeaderData(CNumColWhiteName, Qt::Horizontal, tr("White"));
    _SourceModel->setHeaderData(CNumColBlackName, Qt::Horizontal, tr("Black"));
    _SourceModel->setHeaderData(CNumColWhiteELO,  Qt::Horizontal, tr("White ELO"));
    _SourceModel->setHeaderData(CNumColBlackELO,  Qt::Horizontal, tr("Black ELO"));
    _SourceModel->setHeaderData(CNumColResult,    Qt::Horizontal, tr("Result"));
    _SourceModel->setHeaderData(CNumColEvent,     Qt::Horizontal, tr("Event"));
    _SourceModel->setHeaderData(CNumColDate,      Qt::Horizontal, tr("Date"));
    _SourceModel->setHeaderData(CNumColSite,      Qt::Horizontal, tr("Site"));
    _SourceModel->setHeaderData(CNumColFilename,  Qt::Horizontal, "Filename");
    _SourceModel->setHeaderData(CNumColNoPartie,  Qt::Horizontal, "NoGame");

    if (lNbreGames == 0) return;

    int lNbreLignes = 0;
    for (int i = 0; i < lNbreGames; i++) {

        QFileInfo fileInfo = list.at(i);
        PGNReaderWriter lReaderPGN (fileInfo.absoluteFilePath());

        for (int j=1; j <= lReaderPGN.NbreGames(); j++) {

            lReaderPGN.ReadEntete(j);
            _SourceModel->appendRow(new QStandardItem());
            _SourceModel->setData(_SourceModel->index(lNbreLignes, CNumColWhiteName), lReaderPGN.getWhiteName());
            _SourceModel->setData(_SourceModel->index(lNbreLignes, CNumColBlackName), lReaderPGN.getBlackName());
            _SourceModel->setData(_SourceModel->index(lNbreLignes, CNumColWhiteELO), lReaderPGN.getWhiteELO());
            _SourceModel->setData(_SourceModel->index(lNbreLignes, CNumColBlackELO), lReaderPGN.getBlackELO());
            _SourceModel->setData(_SourceModel->index(lNbreLignes, CNumColResult), lReaderPGN.getResultat());
            _SourceModel->setData(_SourceModel->index(lNbreLignes, CNumColEvent), lReaderPGN.getEvent());
            _SourceModel->setData(_SourceModel->index(lNbreLignes, CNumColDate), lReaderPGN.getDate());
            _SourceModel->setData(_SourceModel->index(lNbreLignes, CNumColSite), lReaderPGN.getSite());
            _SourceModel->setData(_SourceModel->index(lNbreLignes, CNumColFilename), fileInfo.absoluteFilePath());
            _SourceModel->setData(_SourceModel->index(lNbreLignes, CNumColNoPartie), j);
            lNbreLignes++;

        }

    }
    //cacher des cols
     _tableViewGame->setColumnHidden(CNumColFilename, true);
     _tableViewGame->setColumnHidden(CNumColNoPartie, true);

}
