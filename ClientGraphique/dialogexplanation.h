#ifndef DIALOGEXPLANATION_H
#define DIALOGEXPLANATION_H

#include <QDialog>

namespace Ui {
class DialogExplanation;
}

class DialogExplanation : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogExplanation(const QString& aExplanation, bool aAvecModif, QWidget *parent = 0);
    ~DialogExplanation();
    QString getExplanation () const;
    static QString getExplanation (const QString &aInitExplanation, bool aAvecModifs);
private:
    Ui::DialogExplanation *ui;
};

#endif // DIALOGEXPLANATION_H
