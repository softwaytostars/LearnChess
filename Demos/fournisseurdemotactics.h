#ifndef FOURNISSEURDEMOTACTICS_H
#define FOURNISSEURDEMOTACTICS_H

#include "IFournisseurDemo.h"

class FournisseurDemoTactics : public IFournisseurDemo
{
public:
    FournisseurDemoTactics();
    virtual ~FournisseurDemoTactics();
    virtual QString get_SmallDescription(int aIndexDemo) const;
    virtual QString get_DetailedDescription(int aIndexDemo) const;
    virtual QString get_ExtensionXMLFileName () const;
    virtual eTypeInitiation get_TypeInitiation() const {return eInitierTactiques;}
protected:
    virtual void AjouterTypeDemoNames ();
};

#endif // FOURNISSEURDEMOTACTICS_H
