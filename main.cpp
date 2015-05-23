#include "mainwindow.h"
#include "ClientGraphique/mainapplicationlearnchess.h"
#include <QTextCodec>
#include <QDesktopWidget>
#include "Preferences/applicationstyle.h"
#include "Preferences/applicationlanguage.h"
#include "infosconfiguration.h"

int main(int argc, char *argv[])
{

    MainApplicationLearnChess a(argc, argv);
    Q_INIT_RESOURCE(Ressources);

    ApplicationStyle lAppStyle;
    lAppStyle.SetStyle (InfosConfiguration::instance()->getLastStyleApp());

    ApplicationLanguage lAppLangue;
    lAppLangue.SetLanguage(InfosConfiguration::instance()->getLastLanguageAppToStr());

    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForLocale (QTextCodec::codecForName("utf8"));


    int WIDTH  = 650;
    int HEIGHT = 600;


    MainWindow::instance()->resize(WIDTH, HEIGHT);

    //pour centrer l'application
    QDesktopWidget *desktop = QApplication::desktop();
    QRect rectscreen = desktop->screenGeometry(desktop->primaryScreen());
    if (WIDTH > rectscreen.width())   WIDTH  = rectscreen.width();
    if (HEIGHT > rectscreen.height()) HEIGHT = rectscreen.height();
    MainWindow::instance()->move(rectscreen.left()+((rectscreen.width()-WIDTH)/2),
                                 rectscreen.top() +((rectscreen.height()-HEIGHT)/2));
    MainWindow::instance()->show();
    int lexec = a.exec();
    MainWindow::kill();
    return lexec;

}
