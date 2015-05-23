#include "frameinitiation.h"
#include "ui_frameinitiation.h"

#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>

#include "infosgraphiques.h"
#include "infosconfiguration.h"
#include "chessboardview.h"
#include "descriptordemo.h"
#include "implinitiation.h"
#include "decochessboardviewwithrotation.h"
#include "TypeDemo.hpp"
#include "ClientGraphique/dialoginfosmove.h"
#include "ClientGraphique/dialogexplanation.h"
#include "framecreationdemosmove.h"
#include "Demos/factoryfournisseurdemo.h"
#include "IControleur.h"


FrameInitiation::FrameInitiation(eTypeInitiation aTypeInitiation, ImplInitiation *aImplInitiation, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameInitiation),
    _CurrentIndexDemo(0),
    _CustomConfigDemoFile(""),
    _ImplInitiation(aImplInitiation),
    _TypeInitiation(aTypeInitiation)
{
    ui->setupUi(this);
    InfosGraphiques::instance()->AbonnerUtilisateurTraductions(this);

    setLayout (ui->MainverticalLayout);

    if (_ImplInitiation)
        ui->pushButtonRejeu->setVisible(_ImplInitiation->PossibiliteRejeu());
    ui->pushButtonRejeu->setEnabled(false);
    ui->pushButtonPrec->setEnabled(false);
    ui->pushButtonSuiv->setEnabled(false);
    ui->ButtonInfos->setEnabled(false);

    // lit _CustomConfigDemoFile à l'ouverture dans le fichier de config
    _CustomConfigDemoFile = _ImplInitiation->getConfigFileName(aTypeInitiation);
    if (_CustomConfigDemoFile.size() == 0)
        ui->radioButtonDefaut->setChecked (true);
    else
        ui->radioButtonCustomFile->setChecked(true);


    //actualisation graphique + donnees
   // ActualiserChoixConfig (); pas besoin car sera fait pas le Actualiser de l'interface (cf onglets)

    /* Creation du ChessManager */
    _ChessView = new ChessBoardViewWithRotation (new ChessBoardView(0));
    _ChessView->setVisible(false);
    _ChessView->setUseCommentaries(false);
    _ChessView->setUseInfosAllowMoves (_ImplInitiation->AllowChessBoardInteraction());
    _ChessView->getControleur().AllowPlayFirstBlack(true);//pas de mode permissif sauf pour premier coup
    _ChessView->getControleur().AllowShowAnySquareForMovePiece (true);
    _ChessView->getControleur().AllowUserInteraction(_ImplInitiation->AllowChessBoardInteraction());
    ui->LayoutBoard->addWidget(_ChessView);

    ui->MainverticalLayout->setAlignment (Qt::AlignLeft | Qt::AlignTop);

    //creation du fournisseur de demo (en fait une autre reference est comptee , cf shared_ptr)
    _FournisseurDemo = FactoryFournisseurDemo::getFournisseur(aTypeInitiation);//singleton permet de copier le shared_ptr (reference counting)

    //creation de la liste
    _ListDemo = FactoryFournisseurDemo::getListeDemo(aTypeInitiation, this, *(_FournisseurDemo.get()));
    ui->verticalLayout->addWidget(_ListDemo);

    //connect signal clicked
    if (_ListDemo)
        connect(_ListDemo, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(OnClickListe(QListWidgetItem*)));


    ui->ButtonNewDemo->setEnabled(false);

    ui->horizontalLayout_4->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    QFont lFontTitle = QApplication::font();
    lFontTitle.setBold(true);
    ui->labelInfoSelection->setFont(lFontTitle);
    ui->labelInfoSelection->setVisible (false);
    ui->toolButtonInfoSelection->setVisible (false);
    ui->toolButtonInfoSelection->setIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation));


    ui->ButtonInfos->setIcon (QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation));

    InitTraductions();
}

void FrameInitiation::Actualiser() {

    ActualiserChoixConfig (true);

    //reprend la selection
    if (_ListDemo && _FournisseurDemo.get() && (_FournisseurDemo.get()->_SelectedRow >=0)) {

        _ListDemo->clearSelection();
        _ListDemo->setCurrentRow(_FournisseurDemo.get()->_SelectedRow);
        OnClickListe (_ListDemo->selectedItems()[0]);
    }

    if (_FournisseurDemo.get()) {
        ui->labelInfoSelection->setVisible (_ListDemo->selectedItems().count() > 0);
        ui->toolButtonInfoSelection->setVisible (_ListDemo->selectedItems().count() > 0);
    }
}

void FrameInitiation::ActualiserChoixConfig (bool AvecActualisationDemos) {

    ui->ButtonChoixFile->setEnabled(ui->radioButtonCustomFile->isChecked());
    //permet nouvelles demos slt pour custom file
    ui->ButtonNewDemo->setVisible (InfosConfiguration::instance()->getModeConfigurateur());

    ui->labelChoiceConfig->setVisible(ui->radioButtonCustomFile->isChecked() && (_CustomConfigDemoFile.size()>0));
    ui->labelChoiceConfig->setText(tr("File :")+_CustomConfigDemoFile);
    if (AvecActualisationDemos)
        ActualiserDemos ();
}

QString FrameInitiation::getFileNameConfigDemo () {

    if (ui->radioButtonDefaut->isChecked())
        return "";
    else
        return _CustomConfigDemoFile;
}

void FrameInitiation::ActualiserDemos () {

    if (!_ImplInitiation) return;
    if (!_FournisseurDemo.get()) return;

    /* Lecture de fichiers xml pour creer les objets de descriptions de demos pour chaque type de piece*/
    XmlReaderWriterForDemos *lXmlReaderDemo = new XmlReaderWriterForDemos(_ImplInitiation->getTypeConfig(),
                                                                          *(_FournisseurDemo.get()));
    lXmlReaderDemo->get_MappingTypeDemoMoveListeDemos(_mapTypeDemoMove_ListeDemos, getFileNameConfigDemo());
    delete lXmlReaderDemo;
}

void FrameInitiation::OnClickListe(QListWidgetItem *aItemWidget)
{

    if (_FournisseurDemo.get())
        _FournisseurDemo.get()->_SelectedRow = _ListDemo->currentRow();

    _ChessView->setVisible(false);
    ui->labelInfoSelection->setVisible (false);
    ui->toolButtonInfoSelection->setVisible (false);

    _CurrentIndexDemo = 0;

    ui->pushButtonRejeu->setEnabled(false);
    ui->pushButtonPrec->setEnabled(false);
    ui->pushButtonSuiv->setEnabled(false);
    ui->ButtonInfos->setEnabled(false);
    ui->ButtonNewDemo->setEnabled(false);

    if (!_FournisseurDemo.get()
            || !_FournisseurDemo.get()->get_IndexDemoForTypeDemoName(aItemWidget->text(), _CurrentTypeDemoMoveSelected) ) return;

    ui->labelInfoSelection->setVisible (true);
    ui->toolButtonInfoSelection->setVisible (true);

    ui->labelInfoSelection->setText(_FournisseurDemo.get()->get_SmallDescription(_CurrentTypeDemoMoveSelected));

    ui->ButtonNewDemo->setEnabled(true);

    //vérifie qu'il y a des demos pour ce genre de pieces
    if (_mapTypeDemoMove_ListeDemos.find(_CurrentTypeDemoMoveSelected) == _mapTypeDemoMove_ListeDemos.end()) return;

    XmlReaderWriterForDemos::tDescriptorDemo_vector lListeDemos = _mapTypeDemoMove_ListeDemos [_CurrentTypeDemoMoveSelected];

    _ChessView->setDemo (*(lListeDemos[_CurrentIndexDemo]), _ImplInitiation->AllowChessBoardInteraction());
    _ChessView->setVisible(true);

    ui->pushButtonRejeu->setEnabled (true);
    ui->pushButtonPrec->setEnabled (_CurrentIndexDemo > 0);
    ui->pushButtonSuiv->setEnabled ((lListeDemos.size() > 0) && (_CurrentIndexDemo < (lListeDemos.size()-1)));
    ui->ButtonInfos->setEnabled(true);
}

FrameInitiation::~FrameInitiation()
{
    //!!une reference de _FournisseurDemo est supprimee ici (cf is shared_ptr)!!
    _FournisseurDemo.reset();//pas obligatoire

    InfosGraphiques::instance()->DesabonnerUtilisateurTraductions(this);

    if (_ImplInitiation) delete _ImplInitiation;
    for (XmlReaderWriterForDemos::tmapDemoMoveDescriptorDemo_vector::const_iterator it = _mapTypeDemoMove_ListeDemos.begin();
         it != _mapTypeDemoMove_ListeDemos.end();
         ++it){
        for (XmlReaderWriterForDemos::tDescriptorDemo_vector::const_iterator itDemo = (*it).second.begin();
             itDemo != (*it).second.end();
             ++itDemo) {
            delete (*itDemo);
        }
    }
    _mapTypeDemoMove_ListeDemos.clear();

    delete _ChessView;
    delete ui;
}

void FrameInitiation::InitTraductions() {

    ui->pushButtonRejeu->setText(tr("Reload"));
    ui->pushButtonPrec->setText(tr("Previous"));
    ui->pushButtonSuiv->setText(tr("Next"));
    ui->labelChooseConfig->setText(tr("Choose a configuration file"));
    ui->radioButtonDefaut->setText(tr("By default"));
    ui->radioButtonCustomFile->setText(tr("Load another file"));
    ui->ButtonChoixFile->setText(tr("Open file"));
    ui->toolButtonInfoSelection->setToolTip(tr("Click here for having informations about the selection"));
    ui->ButtonInfos->setText(tr("Explanations"));
    ui->ButtonNewDemo->setText(tr("Create your own configurations"));

    if (_FournisseurDemo.get() && _ListDemo) {

        _FournisseurDemo.get()->translate();
        int lWidthMax = 0;
        for (int i=0; i < _ListDemo->model()->rowCount(); i++) {
            lWidthMax = qMax<int> (_ListDemo->sizeHintForColumn(0), lWidthMax);
        }
        _ListDemo->setMaximumWidth(lWidthMax);
        _ListDemo->setMinimumWidth(lWidthMax);
    }

}

void FrameInitiation::Translate() {

   if (_ListDemo && _FournisseurDemo.get()) {

       FactoryFournisseurDemo::Translate(_TypeInitiation,
                                         _ListDemo,
                                         *_FournisseurDemo.get());
    }


//    if (_FournisseurDemo.get())
//        _FournisseurDemo.get()->translate();
    InitTraductions();
}

void FrameInitiation::on_pushButtonRejeu_clicked()
{
    if (_mapTypeDemoMove_ListeDemos.find(_CurrentTypeDemoMoveSelected) == _mapTypeDemoMove_ListeDemos.end()) return;
    XmlReaderWriterForDemos::tDescriptorDemo_vector lListeDemos = _mapTypeDemoMove_ListeDemos [_CurrentTypeDemoMoveSelected];
    _ChessView->setDemo (*(lListeDemos[_CurrentIndexDemo]), _ImplInitiation->AllowChessBoardInteraction());
}

void FrameInitiation::on_pushButtonPrec_clicked()
{
    if (_mapTypeDemoMove_ListeDemos.find(_CurrentTypeDemoMoveSelected) == _mapTypeDemoMove_ListeDemos.end()) return;
    XmlReaderWriterForDemos::tDescriptorDemo_vector lListeDemos = _mapTypeDemoMove_ListeDemos [_CurrentTypeDemoMoveSelected];
    _ChessView->setDemo (*(lListeDemos[--_CurrentIndexDemo]), _ImplInitiation->AllowChessBoardInteraction());
    ui->pushButtonPrec->setEnabled (_CurrentIndexDemo>0);
    ui->pushButtonSuiv->setEnabled ((lListeDemos.size() > 0) && (_CurrentIndexDemo < (lListeDemos.size()-1)));

}

void FrameInitiation::on_pushButtonSuiv_clicked()
{
    if (_mapTypeDemoMove_ListeDemos.find(_CurrentTypeDemoMoveSelected) == _mapTypeDemoMove_ListeDemos.end()) return;
    XmlReaderWriterForDemos::tDescriptorDemo_vector lListeDemos = _mapTypeDemoMove_ListeDemos [_CurrentTypeDemoMoveSelected];
    _ChessView->setDemo (*(lListeDemos[++_CurrentIndexDemo]), _ImplInitiation->AllowChessBoardInteraction());
    ui->pushButtonPrec->setEnabled (_CurrentIndexDemo>0);
    ui->pushButtonSuiv->setEnabled ((lListeDemos.size() > 0) && (_CurrentIndexDemo < (lListeDemos.size()-1)));
}


void FrameInitiation::on_radioButtonDefaut_clicked(bool )
{
    Actualiser();
}

void FrameInitiation::on_radioButtonCustomFile_clicked(bool)
{
    bool lActualiserDemo = (_CustomConfigDemoFile.size() > 0);
    if (lActualiserDemo)
      Actualiser();
    else
      ActualiserChoixConfig (false);
}

void FrameInitiation::on_ButtonChoixFile_clicked()
{
    _CustomConfigDemoFile = QFileDialog::getOpenFileName(this, tr("Open File"), "", "(*.xml)", 0, QFileDialog::ReadOnly);
    if ((_CustomConfigDemoFile.size() == 0))
        ui->radioButtonDefaut->setChecked(true);
    else if (_FournisseurDemo.get())
        _ImplInitiation->setLastConfigFileName (_CustomConfigDemoFile, _FournisseurDemo.get()->get_TypeInitiation());
    ActualiserChoixConfig (true);
}

void FrameInitiation::on_ButtonNewDemo_clicked()
{
    if (_ImplInitiation) {

        QDialog ldiag (this);
        QMainWindow * lMainWindowForNewDemo = new QMainWindow (0);
        QString lNameDemo = "";
        if (_FournisseurDemo.get() &&
                _FournisseurDemo.get()->get_TypeDemoNameForIndexDemo (_CurrentTypeDemoMoveSelected, lNameDemo))
            lMainWindowForNewDemo->setWindowTitle(_ImplInitiation->getTitleInitiation () + tr(" for ") + lNameDemo);
        if (_FournisseurDemo.get())
            lMainWindowForNewDemo->setCentralWidget(new FrameCreationDemosMove (*_ImplInitiation,
                                                                                _CurrentTypeDemoMoveSelected,
                                                                                *(_FournisseurDemo.get()),
                                                                                        lMainWindowForNewDemo));

        QVBoxLayout *MainLayout = new QVBoxLayout (&ldiag);
        MainLayout->addWidget(lMainWindowForNewDemo);
        lMainWindowForNewDemo->setParent(&ldiag);
        ldiag.exec();
    }
}


void FrameInitiation::on_ButtonInfos_clicked()
{

    if (_mapTypeDemoMove_ListeDemos.find(_CurrentTypeDemoMoveSelected) == _mapTypeDemoMove_ListeDemos.end()) return;
    XmlReaderWriterForDemos::tDescriptorDemo_vector lListeDemos = _mapTypeDemoMove_ListeDemos [_CurrentTypeDemoMoveSelected];
    if (lListeDemos[_CurrentIndexDemo]) {
        QString lExplanation = lListeDemos[_CurrentIndexDemo]->get_Explanation();
        QString lLangueToStr = InfosConfiguration::instance()->getLastLanguageAppToStr();

        //verifie si une traduction existe (format -> "en:--explanation--" )
        int lPos = lExplanation.indexOf(lLangueToStr+":--");
        if (lPos != -1) {
            lPos += lLangueToStr.size()+3;//on commence après le --
            int lPosFinale = lExplanation.indexOf("--", lPos);
            if (lPosFinale > lPos)
                lExplanation = lExplanation.mid(lPos, lPosFinale- lPos);
        }

        DialogExplanation::getExplanation(lExplanation,
                                          false);
    }

}

void FrameInitiation::on_toolButtonInfoSelection_clicked()
{
    if (_FournisseurDemo.get())
        DialogInfosMove::ShowInfosForDemoMove (_CurrentTypeDemoMoveSelected, *(_FournisseurDemo.get()));
}
