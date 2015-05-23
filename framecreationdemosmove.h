#ifndef FRAMECREATIONDEMOSMOVE_H
#define FRAMECREATIONDEMOSMOVE_H

#include <QFrame>
#include "ChessBoardManager/ObserverChessManager.h"
#include "Data/ArticlePiece.hpp"


namespace Ui {
class FrameCreationDemosMove;
}

class FrameCreationEchiquier;
class IActionDemo;
class ImplInitiation;
class IFournisseurDemo;

class FrameCreationDemosMove : public QFrame,  public ObserverChessManager
{
    Q_OBJECT
    
public:
    explicit FrameCreationDemosMove(const ImplInitiation& aImplInitiation,
                                    int aTypeDemoMove,
                                    const IFournisseurDemo& aFournisseurDemo,
                                    QWidget *parent = 0);
    ~FrameCreationDemosMove();
    virtual void DoOnAjouterPiece(eTypePiece , eColorPiece ,
                                  int , int ) {}
    virtual void DoOnResetPieces () {}
    virtual void DoOnModificationPlacePiece (eTypePiece , eColorPiece ,
                                             int , int , int , int ) {}
    virtual void DoOnKillPiece (eTypePiece , eColorPiece ,
                                int , int ) {}
    virtual void DoOnPlay (CommandPlay * aCommandPlay);
    
private slots:
    void on_Button_SelectPieceMode_clicked();

    void on_Button_CreateXml_clicked();

    void on_Button_ChoiceXmlFile_clicked();

    void on_Button_AddExplanation_clicked();

    void on_Button_ValidationConfig_clicked();

    void on_Button_RegisterPlayerMove_clicked();

private:
    const ImplInitiation& _ImplInitiation;
    const IFournisseurDemo& _FournisseurDemo;

    Ui::FrameCreationDemosMove *ui;
    FrameCreationEchiquier *_frameCreationEchiquier;

    QVector <IActionDemo*> _ListesActionsDemo;
    tlisteArticlesPieces _ListesPiecesWhite; /**< liste pieces blanches */
    tlisteArticlesPieces _ListesPiecesBlack; /**< liste pieces noires */
    int _TypeDemoMove;

    QString _CTextButtonModeSelectDefault, _CTextButtonModeSelectPlayerMoveDefault;
    QString _FileNameXmlGiven;
    QString _Explanation;
    void ActualiserLabel();
    void ClearActionsDemos ();
    void InitialiserEnabled ();
    void Actualiser();
};

#endif // FRAMECREATIONDEMOSMOVE_H
