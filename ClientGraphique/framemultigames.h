#ifndef FRAMEMULTIGAMES_H
#define FRAMEMULTIGAMES_H

#include <QFrame>
#include "ClientGraphique/IUserTraductions.h"

namespace Ui {
class FrameMultiGames;
}


class FrameMultiGames : public QFrame,
                        public IUserTraductions
{
    Q_OBJECT
    
public:
    explicit FrameMultiGames(QWidget *parent = 0);
    ~FrameMultiGames();
    void ReInit ();
    virtual void Translate();
    
private slots:
    void on_comboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::FrameMultiGames *ui;
};

#endif // FRAMEMULTIGAMES_H
