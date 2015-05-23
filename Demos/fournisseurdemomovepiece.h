#ifndef FOURNISSEURDEMOMOVEPIECE_H
#define FOURNISSEURDEMOMOVEPIECE_H

#include "IFournisseurDemo.h"

class QListePieces;

class FournisseurDemoMovePiece : public IFournisseurDemo
{
public:
    FournisseurDemoMovePiece();
    virtual ~FournisseurDemoMovePiece();
    virtual QString get_SmallDescription(int aIndexDemo) const;
    virtual QString get_DetailedDescription(int aIndexDemo) const;
    virtual bool setStaticPieceAndShowItsPossiblePath (int aIndexDemo, ArticlePiece& aArticlePiece) const;
    virtual QString get_ExtensionXMLFileName () const;
    virtual eTypeInitiation get_TypeInitiation() const {return eInitierPiecesMoves;}
protected:
    virtual void AjouterTypeDemoNames ();
private:
    tQVectorTypePiece _TypesPieces;
};

#endif // FOURNISSEURDEMOMOVEPIECE_H
