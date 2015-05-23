#ifndef IMPLINITIATIONTRAININGMOVE_H
#define IMPLINITIATIONTRAININGMOVE_H

#include "implinitiation.h"

class ImplInitiationTrainingMove : public ImplInitiation
{
public:
    ImplInitiationTrainingMove();
    virtual bool PossibiliteRejeu() const  {return true;}
    virtual QString getConfigFileName(eTypeInitiation aTypeInitiation) const;
    virtual void setLastConfigFileName (const QString& aFileName, eTypeInitiation aTypeInitiation) const;
    virtual QString getTitleInitiation () const;
    virtual eTypeDemo getTypeConfig() const;
    virtual bool AllowChessBoardInteraction() const {return true;}
};

#endif // IMPLINITIATIONTRAININGMOVE_H
