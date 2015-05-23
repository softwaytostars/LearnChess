#include "applicationlanguage.h"
#include <QCoreApplication>
#include "ClientGraphique/mainapplicationlearnchess.h"

ApplicationLanguage::ApplicationLanguage()
{
}

void ApplicationLanguage::SetLanguage (const QString& aLangueToStr) const {

    MainApplicationLearnChess* lMainApp = static_cast<MainApplicationLearnChess*> (qApp);
    lMainApp->InstallTranslatorFor(aLangueToStr);
}
