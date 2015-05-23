#ifndef FRAMEINITIATION_H
#define FRAMEINITIATION_H

#include <QFrame>
#include <list>
#include <map>
#include "TypePiece.hpp"
#include "xmlreaderwriterfordemos.h"
#include "ClientGraphique/IActualisable.hpp"
#include "ClientGraphique/IUserTraductions.h"
#include <boost/shared_ptr.hpp>

namespace Ui {
class FrameInitiation;
}

class ChessBoardViewWithRotation;
class QListWidgetItem;
class ChessManager;
class DescriptorDemo;
class QListWidget;
class ImplInitiation;
class IFournisseurDemo;


class FrameInitiation : public QFrame,
                        public IActualisable,
                        public IUserTraductions
{
    Q_OBJECT
    
public:
    explicit FrameInitiation(eTypeInitiation aTypeInitiation,
                             ImplInitiation* aImplInitiation, QWidget *parent = 0);
    ~FrameInitiation();
    virtual void Actualiser();
    virtual void Translate();
    
private:
    Ui::FrameInitiation *ui;
    ChessBoardViewWithRotation *_ChessView;
    ChessManager   *_ChessManager; /**< l'intelligence du jeu */
    XmlReaderWriterForDemos::tmapDemoMoveDescriptorDemo_vector _mapTypeDemoMove_ListeDemos;
    unsigned int _CurrentIndexDemo;
    int _CurrentTypeDemoMoveSelected;/**< representation de l'enuméré désignant le type sélectionné dans la liste*/
    QString _CustomConfigDemoFile; /**< path+Nom du fichier utilisateur de config pour demo */
    ImplInitiation* _ImplInitiation; /**< Implementation du comportement de la frame */
    boost::shared_ptr<IFournisseurDemo> _FournisseurDemo;
    QListWidget *_ListDemo;
    eTypeInitiation _TypeInitiation;
    void ActualiserChoixConfig (bool AvecActualisationDemos=true);
    void ActualiserDemos ();
    QString getFileNameConfigDemo ();
    void InitTraductions();
private slots:
    void OnClickListe(QListWidgetItem *aItemWidget);
    void on_pushButtonRejeu_clicked();
    void on_pushButtonPrec_clicked();
    void on_pushButtonSuiv_clicked();
    void on_radioButtonDefaut_clicked(bool);
    void on_radioButtonCustomFile_clicked(bool );
    void on_ButtonChoixFile_clicked();
    void on_ButtonNewDemo_clicked();
    void on_ButtonInfos_clicked();
    void on_toolButtonInfoSelection_clicked();
};

#endif // FRAMEINITIATION_H
