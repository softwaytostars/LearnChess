#include "applicationstyle.h"

#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QColor>

ApplicationStyle::ApplicationStyle():
    _FileStyle ("")
{

    //jeu style par defaut
    _sDefaultStyle = sColorStyle ("#F9FCAF",
                                  "#8A0808",
                                  "#DF0101",
                                  "black",
                                  "#F4FA58",
                                  "#F7BE81");//QColor ("#DF0101").lighter(130).name());

    _sStyleKid = sColorStyle ("#CEF6F5",
                              "#F5A9F2",
                              "#2EFE9A",
                              "#FFFF00",
                              "#0000FF",
                              "#F7BE81");//couleur pour menu


    QFile stylesheet(":/Style/MainStyle.qss");//fichier de ressource
    if (stylesheet.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream lTextStream (&stylesheet);
        _FileStyle = QLatin1String (lTextStream.readAll().toStdString().c_str());
        stylesheet.close();
    }

}

void ApplicationStyle::SetStyle (ePreferenceStyle aStyle) const {

    if (_FileStyle.size() == 0) return;

    sColorStyle lStyle;
    switch (aStyle) {
        case eStyleDefault : lStyle = _sDefaultStyle; break;
        case eStyleKid     : lStyle = _sStyleKid; break;
        default : lStyle = _sDefaultStyle;
    }

    QString lresultFile = _FileStyle;
    lresultFile.replace("sColorMainWindow", lStyle._ColorMainWindow);
    lresultFile.replace("sColorBackground", lStyle._ColorBackground);
    lresultFile.replace("sColorHover", lStyle._ColorHover);
    lresultFile.replace("sColorSelection", lStyle._ColorSelection);
    lresultFile.replace("sColorText", lStyle._ColorText);
    lresultFile.replace("sColorDisabled", lStyle._ColorDisabled);
    qApp->setStyleSheet(lresultFile);

}
