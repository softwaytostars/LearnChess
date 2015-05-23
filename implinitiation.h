#ifndef IMPLINITIATION_H
#define IMPLINITIATION_H

#include <QString>
#include <QFrame>
#include "TypeDemo.hpp"

class ImplInitiation
{
public:
    ImplInitiation() {}
    virtual bool PossibiliteRejeu() const  = 0;
    virtual QString getConfigFileName(eTypeInitiation aTypeInitiation) const = 0;
    virtual void setLastConfigFileName (const QString& aFileName, eTypeInitiation aTypeInitiation) const = 0;
    virtual QString getTitleInitiation () const = 0;
    virtual eTypeDemo getTypeConfig() const = 0;
    virtual bool AllowChessBoardInteraction() const = 0;
};

#endif // IMPLINITIATION_H
