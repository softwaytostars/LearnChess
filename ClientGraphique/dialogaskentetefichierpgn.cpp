#include "dialogaskentetefichierpgn.h"
#include "ui_dialogaskentetefichierpgn.h"
#include <QLabel>
#include <QTextBrowser>
#include <QDateEdit>
#include "infosgraphiques.h"

DialogAskEnteteFichierPGN::DialogAskEnteteFichierPGN(const sDataTypeEntete &adataEntete, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAskEnteteFichierPGN),
    _DefautYear (QDate::currentDate().year()),
    _DefaultMonth (QDate::currentDate().month()),
    _DefaultDay (QDate::currentDate().day()),
    _yearInvalid (false), _MonthInvalid (false), _DayInvalid (false)
{
    ui->setupUi(this);
    InfosGraphiques::instance()->AbonnerUtilisateurTraductions(this);

    setMinimumWidth(500);//empirique
    setLayout(ui->MainVLayout);

    //Event
    ui->gridLayout->addWidget(new QLabel (tr("Event :"), this), 0, 0);
    _EventBrowser = new QTextEdit (adataEntete._Event, this);
    _EventBrowser->setMaximumHeight(30);//empirique
    ui->gridLayout->addWidget(_EventBrowser, 0, 1);
    //Site
    ui->gridLayout->addWidget(new QLabel (tr("Site :"), this), 0, 2);
    _SiteBrowser = new QTextEdit (adataEntete._Site, this);
    _SiteBrowser->setMaximumHeight(30);//empirique
    ui->gridLayout->addWidget(_SiteBrowser, 0, 3);
    //Date
    ui->gridLayout->addWidget(new QLabel (tr("Date :"), this), 1, 0);
    _DateBrowser = new QDateEdit (this);
    _DateBrowser->setDisplayFormat("yyyy.MM.dd");
    QDate lDate = QDate::fromString(adataEntete._Date, _DateBrowser->displayFormat());
    if (!lDate.isValid())
    {
        QString lyear  = "????";
        QString lMonth = "??";
        QString lDay   = "??";

        ReadDateToStr (adataEntete._Date, lyear, lMonth, lDay);


        _yearInvalid  = lyear.contains ('?');
        _MonthInvalid = lMonth.contains ('?');
        _DayInvalid   = lDay.contains ('?');
        if (_yearInvalid)  lyear  = QString("%1").arg(_DefautYear);
        if (_MonthInvalid) {
            if (_DefaultMonth <= 9)
                lMonth = QString("0%1").arg(_DefaultMonth);
            else
                lMonth = QString("%1").arg(_DefaultMonth);
        }
        if (_DayInvalid)   {
            if (_DefaultDay <= 9)
                lDay   = QString("0%1").arg(_DefaultDay);
            else
                lDay   = QString("%1").arg(_DefaultDay);
        }
        lDate = QDate::fromString(lyear + '.' + lMonth + '.' + lDay, _DateBrowser->displayFormat());
    }//if (!lDate.isValid())

    _DateBrowser->setDate(lDate);

    ui->gridLayout->addWidget(_DateBrowser, 1, 1);
    //Round
    ui->gridLayout->addWidget(new QLabel (tr("Round :"), this), 1, 2);
    _RoundBrowser = new QTextEdit (adataEntete._Round, this);
    _RoundBrowser->setMaximumHeight(30);//empirique
    ui->gridLayout->addWidget(_RoundBrowser, 1, 3);
    //White
    ui->gridLayout->addWidget(new QLabel (tr("White :"), this), 2, 0);
    _WhiteBrowser = new QTextEdit (adataEntete._White, this);
    _WhiteBrowser->setMaximumHeight(30);//empirique
    ui->gridLayout->addWidget(_WhiteBrowser, 2, 1);
    //Black
    ui->gridLayout->addWidget(new QLabel (tr("Black :"), this), 2, 2);
    _BlackBrowser = new QTextEdit (adataEntete._Black, this);
    _BlackBrowser->setMaximumHeight(30);//empirique
    ui->gridLayout->addWidget(_BlackBrowser, 2, 3);
    //White ELO facultatif
    ui->gridLayout->addWidget(new QLabel (tr("White ELO :"), this), 3, 0);
    _WhiteEloBrowser = new QTextEdit (adataEntete._WhiteElo, this);
    _WhiteEloBrowser->setMaximumHeight(30);//empirique
    ui->gridLayout->addWidget(_WhiteEloBrowser, 3, 1);
    //Black ELO facultatif
    ui->gridLayout->addWidget(new QLabel (tr("Black ELO :"), this), 3, 2);
    _BlackEloBrowser = new QTextEdit (adataEntete._BlackElo, this);
    _BlackEloBrowser->setMaximumHeight(30);//empirique
    ui->gridLayout->addWidget(_BlackEloBrowser, 3, 3);

    ui->groupBox->setLayout(ui->horizontalLayout);
    ui->groupBox->setTitle(tr("Result :"));
    ui->radio_WhiteWin->setText("1-0");
    ui->radio_BlackWin->setText("0-1");
    ui->radio_nulle->setText("1/2-1/2");

    if (adataEntete._Resultat == "1-0") {
        ui->radio_WhiteWin->setChecked(true);
    }
    else if (adataEntete._Resultat == "0-1") {
            ui->radio_BlackWin->setChecked(true);
    }
    else
        ui->radio_nulle->setChecked(true);

    ui->MainVLayout->addWidget(ui->buttonBox);

    Translate();
}

DialogAskEnteteFichierPGN::~DialogAskEnteteFichierPGN()
{
    delete ui;
    InfosGraphiques::instance()->DesabonnerUtilisateurTraductions(this);
}

void DialogAskEnteteFichierPGN::ReadDateToStr (const QString& aStrDate,
                                               QString &ayear,
                                               QString &aMonth,
                                               QString &aDay) const {

    int lPosYear = aStrDate.indexOf('.');
    if (lPosYear != -1) {

        ayear  = aStrDate.left(lPosYear);
        int lPosMonth = aStrDate.indexOf('.', lPosYear+1);
        if (lPosMonth != -1)
        {
            aMonth = aStrDate.left(lPosMonth);
            aMonth = aMonth.right(aMonth.size()-(lPosYear+1));
            aDay   = aStrDate.right (aStrDate.size()-(lPosMonth+1));
        }
    }
}

void DialogAskEnteteFichierPGN::getEnTeteData (sDataTypeEntete &adataEntete) const {

    QString lresultat;
    if (ui->radio_WhiteWin->isChecked())
        lresultat = "1-0";
    else if (ui->radio_BlackWin->isChecked())
        lresultat = "0-1";
    else
        lresultat = "1/2-1/2";

    QDate lDate      = _DateBrowser->date();
    QString lStrdate = lDate.toString(_DateBrowser->displayFormat());

    if (_yearInvalid || _MonthInvalid || _DayInvalid) {
        QString lYear, lMonth, lDay;
        ReadDateToStr (lStrdate, lYear, lMonth, lDay);
        if (_yearInvalid && (lDate.year() == _DefautYear))
            lYear = "????";
        if (_MonthInvalid && (lDate.month() == _DefaultMonth))
            lMonth = "??";
        if (_DayInvalid && (lDate.day() == _DefaultDay))
            lDay = "??";
        lStrdate = lYear + '.' + lMonth + '.' + lDay;
    }

    adataEntete = sDataTypeEntete (_EventBrowser->toPlainText(),
                                   _SiteBrowser->toPlainText(),
                                   _RoundBrowser->toPlainText(),
                                   lStrdate,
                                   _WhiteBrowser->toPlainText(),
                                   _BlackBrowser->toPlainText(),
                                   _WhiteEloBrowser->toPlainText(),
                                   _BlackEloBrowser->toPlainText(),
                                   lresultat
                                   );

}

void DialogAskEnteteFichierPGN::Translate() {

    setWindowTitle(tr("Informations on the game"));
}

