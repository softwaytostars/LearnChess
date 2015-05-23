#ifndef APPLICATIONLANGUAGE_H
#define APPLICATIONLANGUAGE_H

#include <QString>
class QTranslator;

class ApplicationLanguage
{
public:
    ApplicationLanguage();
    void SetLanguage (const QString& aLangueToStr) const;
};

#endif // APPLICATIONLANGUAGE_H
