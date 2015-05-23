#ifndef IMPLINITIATIONDEMOMOVE_H
#define IMPLINITIATIONDEMOMOVE_H

#include "implinitiation.h"

class ImplInitiationDemoMove : public ImplInitiation
{
public:
    ImplInitiationDemoMove();
    virtual bool PossibiliteRejeu() const  {return true;}
    virtual QString getConfigFileName(eTypeInitiation aTypeInitiation) const;
    virtual void setLastConfigFileName (const QString& aFileName, eTypeInitiation aTypeInitiation) const;
    virtual QString getTitleInitiation () const;
    virtual eTypeDemo getTypeConfig() const;
    virtual bool AllowChessBoardInteraction() const {return false;}
};

#endif // IMPLINITIATIONDEMOMOVE_H
