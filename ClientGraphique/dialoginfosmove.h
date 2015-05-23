#ifndef DIALOGINFOSMOVE_H
#define DIALOGINFOSMOVE_H

#include <QDialog>
#include "TypeDemo.hpp"

namespace Ui {
class DialogInfosMove;
}

class ChessBoardView;
class IFournisseurDemo;

class DialogInfosMove : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogInfosMove(int aTypeDemoMove,
                             const IFournisseurDemo& aFournisseurDemo,
                             QWidget *parent = 0);
    ~DialogInfosMove();
    static void ShowInfosForDemoMove (int aTypeDemoMove, const IFournisseurDemo& aFournisseurDemo);
    
private:
    Ui::DialogInfosMove *ui;
    ChessBoardView *_ChessView;
};

#endif // DIALOGINFOSMOVE_H
