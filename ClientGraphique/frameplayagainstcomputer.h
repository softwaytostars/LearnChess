#ifndef FRAMEPLAYAGAINSTCOMPUTER_H
#define FRAMEPLAYAGAINSTCOMPUTER_H

#include "Algo/IObserverAlgo.h"
#include "ClientGraphique/IActualisable.hpp"
#include "ClientGraphique/IUserTraductions.h"
#include <QFrame>
#include "TypePiece.hpp"

namespace Ui {
class FramePlayAgainstComputer;
}

class ChessBoardViewWithRotation;

class FramePlayAgainstComputer : public QFrame,
                                 public IObserverAlgo,
                                 public IActualisable,
                                 public IUserTraductions
{
    Q_OBJECT
    
public:
    explicit FramePlayAgainstComputer(QWidget *parent = 0);
    ~FramePlayAgainstComputer();
    virtual void DoOnStartPlay();
    virtual void DoOnEndPlay();
    virtual void DoOnCheckMate();
    virtual void DoOnPat();
    virtual void DoOnAlgoVictorious ();
    virtual void Actualiser();
    virtual void Translate();
    
private slots:
    void on_radioButtonWhite_clicked();

    void on_radioButtonBlack_clicked();

    void on_Button_Init_clicked();

    void on_Button_StopAlgo_clicked();

private:
    enum eEtatAlgo {
        eAlgoNone,
        eAlgoLost,
        eAlgoWins,
        eAlgoEqual
    };

    Ui::FramePlayAgainstComputer *ui;
    ChessBoardViewWithRotation *_ChessView;
    eColorPiece _ColorComputer;
    QString _MessageGagne, _MessagePerd, _MessageNulle;
    eEtatAlgo _EtatAlgo;
    void SetInfosMateOrStaleMate (const QString& aInfos);
};

#endif // FRAMEPLAYAGAINSTCOMPUTER_H
