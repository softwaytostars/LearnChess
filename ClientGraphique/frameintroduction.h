#ifndef FRAMEINTRODUCTION_H
#define FRAMEINTRODUCTION_H

#include <QFrame>
#include "ClientGraphique/IUserTraductions.h"

namespace Ui {
class FrameIntroduction;
}

class FrameIntroduction : public QFrame,
                          public IUserTraductions
{
    Q_OBJECT
    
public:
    explicit FrameIntroduction(QWidget *parent = 0);
    ~FrameIntroduction();
    virtual void Translate();
    
private:
    Ui::FrameIntroduction *ui;
};

#endif // FRAMEINTRODUCTION_H
