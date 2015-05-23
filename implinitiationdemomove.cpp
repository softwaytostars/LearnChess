#include "implinitiationdemomove.h"
#include "infosconfiguration.h"

ImplInitiationDemoMove::ImplInitiationDemoMove()
{
}

QString ImplInitiationDemoMove::getConfigFileName(eTypeInitiation aTypeInitiation) const {

    return InfosConfiguration::instance()->getLastFileNameForDemo(aTypeInitiation);
}

void ImplInitiationDemoMove::setLastConfigFileName (const QString& aFileName, eTypeInitiation aTypeInitiation) const {

    InfosConfiguration::instance()->setLastFileNameForDemo(aFileName, aTypeInitiation);
}

QString ImplInitiationDemoMove::getTitleInitiation () const {

    return QString(QObject::tr("Creating move demonstration"));
}

eTypeDemo ImplInitiationDemoMove::getTypeConfig() const {

    return eSimuMove;
}
