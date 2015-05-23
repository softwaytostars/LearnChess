#ifndef FOURNISSEURDEMOSPECIALMOVES_H
#define FOURNISSEURDEMOSPECIALMOVES_H

#include "IFournisseurDemo.h"

class FournisseurDemoSpecialMoves : public IFournisseurDemo
{
public:
    FournisseurDemoSpecialMoves();
    virtual ~FournisseurDemoSpecialMoves();
    virtual QString get_SmallDescription(int aIndexDemo) const;
    virtual QString get_DetailedDescription(int aIndexDemo) const;
    virtual QString get_ExtensionXMLFileName () const;
    virtual eTypeInitiation get_TypeInitiation() const {return eInitierSpecialMoves;}
protected:
    virtual void AjouterTypeDemoNames ();
};

#endif // FOURNISSEURDEMOSPECIALMOVES_H
