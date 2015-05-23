#include "decochessboardviewwithrotation.h"
#include "ui_decochessboardviewwithrotation.h"
#include <QGraphicsItem>
#include "descriptordemo.h"
#include "infosgraphiques.h"

ChessBoardViewWithRotation::ChessBoardViewWithRotation(QWidget *aChessBoardView, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ChessBoardViewWithRotation),
    _ChessBoardView (aChessBoardView)
{
    ui->setupUi(this);
    InfosGraphiques::instance()->AbonnerUtilisateurTraductions(this);

    setLayout(ui->horizontalLayout);
    ui->checkBoxComments->setChecked(false);
    ui->horizontalLayout_2->setAlignment (Qt::AlignLeft | Qt::AlignTop);
    on_checkBoxComments_toggled(false);//sinon il faut mettre dans le designer checkbox cochee à true pour changer d'etat...
    ui->LayoutChessView->addWidget(_ChessBoardView);
    ui->label->setVisible(false);
    ui->labelInfoCoup->setText("");

    ui->RightVLayout->setAlignment (Qt::AlignLeft | Qt::AlignTop);

    RegisterView(*this);
    Translate();
}

ChessBoardViewWithRotation::~ChessBoardViewWithRotation()
{
    delete _ChessBoardView;
    delete ui;
    InfosGraphiques::instance()->DesabonnerUtilisateurTraductions(this);
}

void ChessBoardViewWithRotation::Translate() {

    ui->checkBoxRotate->setText(tr("Rotate"));
    ui->checkBoxComments->setText(tr("Commentaries"));
    ui->checkBoxInfoAllowedMoves->setText(tr("Display allowed moves"));
}

void ChessBoardViewWithRotation::setDemo (const DescriptorDemo& aDemo, bool aAllowInteractionChessBoard) {

    ui->checkBoxRotate->setChecked(aDemo.UseRotation());

    IChessBoardView *lIChessBoard = dynamic_cast<IChessBoardView*> (_ChessBoardView);
    if (lIChessBoard)
        lIChessBoard->setDemo(aDemo, aAllowInteractionChessBoard);
}

void ChessBoardViewWithRotation::setFreeUseChessBoard (bool aDragOutPossible, bool aModePermissif) {

    IChessBoardView *lIChessBoard = dynamic_cast<IChessBoardView*> (_ChessBoardView);
    if (lIChessBoard)
        lIChessBoard->setFreeUseChessBoard(aDragOutPossible, aModePermissif);
}

void ChessBoardViewWithRotation::SetModeRotation (bool aModeRotation) {

    IChessBoardView *lIChessBoard = dynamic_cast<IChessBoardView*> (_ChessBoardView);
    if (lIChessBoard)
       lIChessBoard->SetModeRotation(aModeRotation);
}

void ChessBoardViewWithRotation::setModeInfoAllowedMoves (bool aInfoAllowedMoves) {

    IChessBoardView *lIChessBoard = dynamic_cast<IChessBoardView*> (_ChessBoardView);
    if (lIChessBoard)
       lIChessBoard->setModeInfoAllowedMoves(aInfoAllowedMoves);
}

void ChessBoardViewWithRotation::FillBoard () {

    IChessBoardView *lIChessBoard = dynamic_cast<IChessBoardView*> (_ChessBoardView);
    if (lIChessBoard)
        lIChessBoard->FillBoard();
}

void ChessBoardViewWithRotation::EmptyBoard () {

    IChessBoardView *lIChessBoard = dynamic_cast<IChessBoardView*> (_ChessBoardView);
    if (lIChessBoard)
        lIChessBoard->EmptyBoard();
}

bool ChessBoardViewWithRotation::getArticlesPiecesDeCouleur (eColorPiece aColorPiece,
                                                             tlisteArticlesPieces& aListeArticles) const {

    IChessBoardView *lIChessBoard = dynamic_cast<IChessBoardView*> (_ChessBoardView);
    if (lIChessBoard)
        return lIChessBoard->getArticlesPiecesDeCouleur(aColorPiece, aListeArticles);
    else return false;
}

void ChessBoardViewWithRotation::RegisterObserver (ObserverChessManager &aObserver) {

    IChessBoardView *lIChessBoard = dynamic_cast<IChessBoardView*> (_ChessBoardView);
    if (lIChessBoard)
        lIChessBoard->RegisterObserver(aObserver);
}

void ChessBoardViewWithRotation::setTitle(const QString& aTitle) {

    ui->label->setText(aTitle);
    ui->label->setVisible(aTitle.size() > 0);
    QFont f = QFont (ui->label->font());
    f.setPointSize(7);
    f.setBold(true);
    ui->label->setFont(f);
    update();
}

void ChessBoardViewWithRotation::DrawComments (const QString& aDescriptionMove, const QString& aCommentary) {

    ui->labelInfoCoup->setText(aDescriptionMove);
    ui->textComments->setPlainText(aCommentary);
    //met le hint sur le text au cas où message long
    ui->textComments->setToolTip(aCommentary);
}

void ChessBoardViewWithRotation::RegisterView (IChessBoardView& aView) {

    IChessBoardView *lIChessBoard = dynamic_cast<IChessBoardView*> (_ChessBoardView);
    if (lIChessBoard)
       lIChessBoard->RegisterView(aView);
}

void ChessBoardViewWithRotation::ChangeCommentary (const QString& aCommentary) const {

    IChessBoardView *lIChessBoard = dynamic_cast<IChessBoardView*> (_ChessBoardView);
    if (lIChessBoard)
       lIChessBoard->ChangeCommentary(aCommentary);
}

void ChessBoardViewWithRotation::DoOnModePlayAgainstComputer(eColorPiece aColorComputer) {

    ui->checkBoxRotate->setChecked(aColorComputer==WHITE);
    on_checkBoxRotate_clicked (ui->checkBoxRotate->isChecked());
}


IControleur& ChessBoardViewWithRotation::getControleur() const {

    IChessBoardView *lIChessBoard = dynamic_cast<IChessBoardView*> (_ChessBoardView);
    return lIChessBoard->getControleur();
}


void ChessBoardViewWithRotation::on_checkBoxRotate_clicked(bool checked)
{
    SetModeRotation (checked);
}

void ChessBoardViewWithRotation::on_checkBoxComments_toggled(bool checked)
{
    ui->labelInfoCoup->setVisible(checked);
    ui->textComments->setVisible(checked);
}


void ChessBoardViewWithRotation::on_textComments_textChanged()
{
    ChangeCommentary (ui->textComments->toPlainText());
}

bool ChessBoardViewWithRotation::RotationIsAsked() const {

    return ui->checkBoxRotate->isChecked();
}

void ChessBoardViewWithRotation::setUseCommentaries (bool aWithCommentaries) {

    ui->checkBoxComments->setVisible(aWithCommentaries);
}

void ChessBoardViewWithRotation::setUseInfosAllowMoves (bool aUseInfosAllowMoves) {

    ui->checkBoxInfoAllowedMoves->setVisible(aUseInfosAllowMoves);
}

void ChessBoardViewWithRotation::on_checkBoxInfoAllowedMoves_clicked(bool checked)
{
    setModeInfoAllowedMoves(checked);
}

void ChessBoardViewWithRotation::setPermettreRotation (bool aPermettre) {

    ui->checkBoxRotate->setEnabled(aPermettre);
}

