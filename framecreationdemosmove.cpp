#include "framecreationdemosmove.h"
#include "ui_framecreationdemosmove.h"
#include "TypeDemo.hpp"
#include "xmlreaderwriterfordemos.h"
#include "infosgraphiques.h"
#include <QMessageBox>
#include <QFileDialog>
#include "framecreationechiquier.h"
#include "infosconfiguration.h"
#include "ClientGraphique/affichages.h"
#include "Demos/actionsdemo.h"
#include "ClientGraphique/dialogexplanation.h"
#include "implinitiation.h"
#include "Demos/IFournisseurDemo.h"
#include "ChessBoardManager/commandplay.h"

FrameCreationDemosMove::FrameCreationDemosMove(const ImplInitiation &aImplInitiation,
                                               int aTypeDemoMove,
                                               const IFournisseurDemo &aFournisseurDemo, QWidget *parent) :
    QFrame(parent),
    _ImplInitiation(aImplInitiation),
    _FournisseurDemo (aFournisseurDemo),
    ui(new Ui::FrameCreationDemosMove),
    _TypeDemoMove (aTypeDemoMove),
    _Explanation("")
{
    ui->setupUi(this);

    switch (_ImplInitiation.getTypeConfig()) {
    case eSimuMove     : _FileNameXmlGiven = InfosConfiguration::instance()->getLastFileNameForDemo(_FournisseurDemo.get_TypeInitiation());break;
        case eTrainingMove : _FileNameXmlGiven = InfosConfiguration::instance()->getLastFileNameForTrainingMove(_FournisseurDemo.get_TypeInitiation());break;
        default : _FileNameXmlGiven = ""; break;
    }

    _CTextButtonModeSelectDefault = tr("Set mode register auto move");
    setLayout (ui->verticalLayout);

    _frameCreationEchiquier = new FrameCreationEchiquier (*this);
    //on ne verifie pas le tour des blancs ou noirs, on suppose que configurateur sait ce qu'il fait
    _frameCreationEchiquier->setModePermissifEtDragOutOfScene(true, true);
    ui->LayoutFrameEchiquier->addWidget(_frameCreationEchiquier);


    ui->Button_ValidationConfig->setCheckable(true);
    ui->Button_ValidationConfig->setStyleSheet(
    ":checked { color: blue } "
    );

    ui->Button_SelectPieceMode->setVisible(_ImplInitiation.AllowChessBoardInteraction());
    ui->Button_SelectPieceMode->setText(_CTextButtonModeSelectDefault);
    ui->Button_SelectPieceMode->setToolTip(tr("Keep this button pushed for registering auto moves"));
    ui->Button_SelectPieceMode->setCheckable(true);//pour garder l'etat down tant que pas cliqué dessus ou envoyé signal clicked
    ui->Button_SelectPieceMode->setStyleSheet(
    ":checked { color: blue } "
    );

    _CTextButtonModeSelectPlayerMoveDefault = tr("Set mode register player move");
    ui->Button_RegisterPlayerMove->setVisible(_ImplInitiation.AllowChessBoardInteraction());
    ui->Button_RegisterPlayerMove->setText(_CTextButtonModeSelectPlayerMoveDefault);
    ui->Button_RegisterPlayerMove->setToolTip(tr("Register THE move expected by the player just after the last recording move"));
    ui->Button_RegisterPlayerMove->setCheckable(true);
    ui->Button_RegisterPlayerMove->setStyleSheet(
    ":checked { color: blue } "
    );

    ui->Button_CreateXml->setText(tr("Save Demo"));
    ui->Button_CreateXml->setStyleSheet(
                ":pressed { color: blue } "
                );

    ui->Button_ChoiceXmlFile->setText(tr("Define another configuration file"));

    ui->Button_AddExplanation->setText(tr("Add an explanation"));


    ActualiserLabel();
}

void FrameCreationDemosMove::InitialiserEnabled () {

    ui->Button_SelectPieceMode->setEnabled(false);
    ui->Button_RegisterPlayerMove->setEnabled(false);
    ui->Button_CreateXml->setEnabled(false);
    ui->Button_ChoiceXmlFile->setEnabled(false);
    ui->Button_AddExplanation->setEnabled(false);
    ui->Button_ValidationConfig->setChecked(false);
}

void FrameCreationDemosMove::ActualiserLabel(){

    ui->label->setVisible(_FileNameXmlGiven.size() > 0);
    if (_FileNameXmlGiven.size() > 0)
        ui->label->setText(tr("Demo will be saved in the configuration file :\n")+_FileNameXmlGiven);

    ui->Button_ChoiceXmlFile->setVisible(true);
    ui->Button_CreateXml->setEnabled(_FileNameXmlGiven.size() > 0);
    Actualiser();
}

void FrameCreationDemosMove::ClearActionsDemos () {

    for (QVector<IActionDemo*>::const_iterator it = _ListesActionsDemo.begin();
         it != _ListesActionsDemo.end();
         ++it) {
        delete (*it);
    }
    _ListesActionsDemo.clear();
    _Explanation = "";
}

FrameCreationDemosMove::~FrameCreationDemosMove()
{
    //delete _frameCreationEchiquier;
    //_frameCreationEchiquier = 0;

    ClearActionsDemos();

    delete ui;
}

void FrameCreationDemosMove::on_Button_SelectPieceMode_clicked()
{

    ui->Button_RegisterPlayerMove->setChecked(false);
    if (ui->Button_SelectPieceMode->isChecked())
       ui->Button_SelectPieceMode->setText(tr("Mode Register auto move activated"));
    else
       ui->Button_SelectPieceMode->setText(_CTextButtonModeSelectDefault);


    Actualiser();

}

void FrameCreationDemosMove::on_Button_CreateXml_clicked()
{ 

   if (_FileNameXmlGiven.size() != 0) {

       XmlReaderWriterForDemos *lWriterXml = new XmlReaderWriterForDemos(_ImplInitiation.getTypeConfig(),
                                                                         _FournisseurDemo);
       lWriterXml->WriteDemoFile (_FileNameXmlGiven,
                                  _TypeDemoMove,
                                  _ListesPiecesWhite, _ListesPiecesBlack,
                                  _frameCreationEchiquier->RotationIsAsked(),
                                  _Explanation,
                                  _ListesActionsDemo);
       delete lWriterXml;
       lWriterXml = 0;

       //reinitialise pour rentrer d'autres demos
       ui->Button_SelectPieceMode->setChecked(false);
       on_Button_SelectPieceMode_clicked();

       ClearActionsDemos ();
       InitialiserEnabled ();
       Actualiser();
       //Actualiser l'affichage courant
       Affichages::ActualiserAffichageCourant();
   }else
   {
       QMessageBox msg;
       msg.setText(tr("You need to define a XML file"));
       msg.exec();
   }



}


void FrameCreationDemosMove::DoOnPlay (CommandPlay * aCommandPlay) {

    if (!ui->Button_SelectPieceMode->isChecked() && !ui->Button_RegisterPlayerMove->isChecked()) return;

    if(!aCommandPlay) return;
    aCommandPlay->FillActionsForDemo (_ListesActionsDemo,
                                      !_ImplInitiation.AllowChessBoardInteraction(),
                                      ui->Button_RegisterPlayerMove->isChecked());

}

void FrameCreationDemosMove::on_Button_ChoiceXmlFile_clicked()
{
    _FileNameXmlGiven = QFileDialog::getSaveFileName(this, tr("Save File"), "", "(*.xml)", 0, QFileDialog::DontConfirmOverwrite);

    switch(_ImplInitiation.getTypeConfig()) {
    case eSimuMove    : InfosConfiguration::instance()->setLastFileNameForDemo(_FileNameXmlGiven, _FournisseurDemo.get_TypeInitiation());break;
        case eTrainingMove: InfosConfiguration::instance()->setLastFileNameForTrainingMove(_FileNameXmlGiven, _FournisseurDemo.get_TypeInitiation());break;
    }

    ActualiserLabel();
}

void FrameCreationDemosMove::on_Button_AddExplanation_clicked()
{
    _Explanation = DialogExplanation::getExplanation(_Explanation, true);
}

void FrameCreationDemosMove::on_Button_ValidationConfig_clicked()
{
    //si moves deja stockes, les supprime
    ClearActionsDemos ();
    //avant d'enregsitrer les mouvements de la piece pour la demo, on prend la config existante
    _frameCreationEchiquier->getArticlesPiecesDeCouleur(WHITE, _ListesPiecesWhite);
    _frameCreationEchiquier->getArticlesPiecesDeCouleur(BLACK, _ListesPiecesBlack);

    ui->Button_AddExplanation->setEnabled(true);
    ui->Button_ChoiceXmlFile->setEnabled(true);
    ui->Button_CreateXml->setEnabled(true);
    ui->Button_SelectPieceMode->setEnabled(true);
    ui->Button_RegisterPlayerMove->setEnabled(true);

    Actualiser();
}

void FrameCreationDemosMove::on_Button_RegisterPlayerMove_clicked()
{
    //releve bouton selection auto move
    ui->Button_SelectPieceMode->setChecked(false);

    if (ui->Button_RegisterPlayerMove->isChecked())
       ui->Button_RegisterPlayerMove->setText(tr("Mode Register player move activated"));
    else
       ui->Button_RegisterPlayerMove->setText(_CTextButtonModeSelectPlayerMoveDefault);

    Actualiser();

}

void FrameCreationDemosMove::Actualiser() {


    if (ui->Button_ValidationConfig->isChecked()) {
       ui->Button_ValidationConfig->setText(tr("Configuration Validated"));
       ui->Button_ValidationConfig->setToolTip(tr("Unclick it for changing the configuration"));
       _frameCreationEchiquier->setModePermissifEtDragOutOfScene(false, true);
       _frameCreationEchiquier->setToolTip("");
    }
    else {
       ui->Button_ValidationConfig->setText(tr("Validate the new configuration"));
       ui->Button_ValidationConfig->setToolTip(tr("This step re-initialize demo moves and take the pieces on the chessboard as the new configuration"));
       _frameCreationEchiquier->setModePermissifEtDragOutOfScene(true, true);
       _frameCreationEchiquier->setToolTip(tr("The moves must alternate between black and white"));
    }

    //si bouton pas visible, le met etat check slt si validation OK
    if (!ui->Button_SelectPieceMode->isVisible())
        ui->Button_SelectPieceMode->setChecked(ui->Button_ValidationConfig->isChecked());
}
