#ifndef FOURNISSEURDEMOPATTERNSMAT_H
#define FOURNISSEURDEMOPATTERNSMAT_H

#include "IFournisseurDemo.h"

class FournisseurDemoPatternsMat : public IFournisseurDemo
{
public:
    FournisseurDemoPatternsMat();
    virtual ~FournisseurDemoPatternsMat();
    virtual QString get_SmallDescription(int aIndexDemo) const;
    virtual QString get_DetailedDescription(int aIndexDemo) const;
    virtual QString get_ExtensionXMLFileName () const;
    virtual eTypeInitiation get_TypeInitiation() const {return eInitierPatternsMat;}
protected:
    virtual void AjouterTypeDemoNames ();
};

#endif // FOURNISSEURDEMOPATTERNSMAT_H
