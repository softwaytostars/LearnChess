#ifndef DIALOGASKENTETEFICHIERPGN_H
#define DIALOGASKENTETEFICHIERPGN_H

#include <QDialog>
#include "TypeFichierPGN.h"
#include "ClientGraphique/IUserTraductions.h"

class QTextEdit;
class QDateEdit;

namespace Ui {
class DialogAskEnteteFichierPGN;
}

class DialogAskEnteteFichierPGN : public QDialog,
                                  public IUserTraductions

{
    Q_OBJECT
    
public:
    explicit DialogAskEnteteFichierPGN(const sDataTypeEntete& adataEntete, QWidget *parent = 0);
    ~DialogAskEnteteFichierPGN();
    void getEnTeteData (sDataTypeEntete &adataEntete) const;
    virtual void Translate();
    
private:
    Ui::DialogAskEnteteFichierPGN *ui;
    QTextEdit *_EventBrowser, *_SiteBrowser, *_RoundBrowser,
              *_WhiteBrowser, *_BlackBrowser, *_WhiteEloBrowser, *_BlackEloBrowser;
    QDateEdit *_DateBrowser;
    int _DefautYear, _DefaultMonth, _DefaultDay;
    bool _yearInvalid, _MonthInvalid, _DayInvalid;
    void ReadDateToStr (const QString& aStrDate,
                        QString &ayear,
                        QString &aMonth,
                        QString &aDay) const;
};

#endif // DIALOGASKENTETEFICHIERPGN_H
