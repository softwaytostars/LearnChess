#include "implinitiationtrainingmove.h"
#include "infosconfiguration.h"

ImplInitiationTrainingMove::ImplInitiationTrainingMove()
{
}

QString ImplInitiationTrainingMove::getConfigFileName(eTypeInitiation aTypeInitiation) const {

    return InfosConfiguration::instance()->getLastFileNameForTrainingMove(aTypeInitiation);
}

void ImplInitiationTrainingMove::setLastConfigFileName (const QString& aFileName, eTypeInitiation aTypeInitiation) const {

    InfosConfiguration::instance()->setLastFileNameForTrainingMove(aFileName, aTypeInitiation);
}

QString ImplInitiationTrainingMove::getTitleInitiation () const {

    return QString(QObject::tr("Creating configuration for training at moves"));
}

eTypeDemo ImplInitiationTrainingMove::getTypeConfig() const {

    return eTrainingMove;
}
