#include "fournisseurdemopatternsmat.h"

#include "TypeDemo.hpp"

#include <QtGlobal>

FournisseurDemoPatternsMat::FournisseurDemoPatternsMat():
    IFournisseurDemo()
{   

    AssociateIndexDemoAndXMLNodeName ((int) ePatternMatCouloir,   "DemosMatCouloir");
    AssociateIndexDemoAndXMLNodeName ((int) ePatternMatLolli,     "DemosMatLolli");
    AssociateIndexDemoAndXMLNodeName ((int) ePatternMatEtouffe,   "DemosMatEtouffe");
    AssociateIndexDemoAndXMLNodeName ((int) ePatternMatEpaulette, "DemosMatEpaulette");
    AssociateIndexDemoAndXMLNodeName ((int) ePatternMatArabes,    "DemosMatArabes");
    AssociateIndexDemoAndXMLNodeName ((int) ePatternMatGreco,     "DemosMatGreco");
    AssociateIndexDemoAndXMLNodeName ((int) ePatternMatBoden,     "DemosMatBoden");
    AssociateIndexDemoAndXMLNodeName ((int) ePatternMatAnastasie, "DemosMatAnastasie");
    AssociateIndexDemoAndXMLNodeName ((int) ePatternMatLegal,     "DemosMatLegal");

    AjouterTypeDemoNames ();

}

void FournisseurDemoPatternsMat::AjouterTypeDemoNames () {

    AssociateIndexDemoAndTypeDemoName ((int) ePatternMatCouloir,   QObject::tr("Mat du couloir"));
    AssociateIndexDemoAndTypeDemoName ((int) ePatternMatLolli,     QObject::tr("Baiser de la mort"));
    AssociateIndexDemoAndTypeDemoName ((int) ePatternMatEtouffe,   QObject::tr("Mat à l'étouffé"));
    AssociateIndexDemoAndTypeDemoName ((int) ePatternMatEpaulette, QObject::tr("Mat des épaulettes"));
    AssociateIndexDemoAndTypeDemoName ((int) ePatternMatArabes,    QObject::tr("Mat Arabe"));
    AssociateIndexDemoAndTypeDemoName ((int) ePatternMatGreco,     QObject::tr("Mat de Greco"));
    AssociateIndexDemoAndTypeDemoName ((int) ePatternMatBoden,     QObject::tr("Mat de Boden"));
    AssociateIndexDemoAndTypeDemoName ((int) ePatternMatAnastasie, QObject::tr("Mat d'Anastasie"));
    AssociateIndexDemoAndTypeDemoName ((int) ePatternMatLegal,     QObject::tr("Mat de Legal"));

}

FournisseurDemoPatternsMat::~FournisseurDemoPatternsMat() {

}

QString FournisseurDemoPatternsMat::get_SmallDescription(int aIndexDemo) const {

    QString lresult;
    //on prend le meme nom donné dans la liste
    if (this->get_TypeDemoNameForIndexDemo(aIndexDemo, lresult))
        return lresult;
    else return QString("");

}

QString FournisseurDemoPatternsMat::get_DetailedDescription(int aIndexDemo) const {

    eTypeDemoPatternMat lTypeDemoMat = eTypeDemoPatternMat (aIndexDemo) ;
    switch (lTypeDemoMat) {

        case ePatternMatCouloir : {
            return QString(QObject::tr("This mate occurs when the King is trapped on the back rank by its own Pawns and has no piece to interpose."));

        }

        case ePatternMatLolli : {
            return QString (QObject::tr("This mate occurs when the Queen mates the King beeing on a square just aside, like making a kiss of dead to the King."));
        }

        case ePatternMatEtouffe : {
            return QString (QObject::tr("This mate occurs when the King cannot escape from the attack from a Knight because it is trapped by it's own pieces."));
        }

        case ePatternMatEpaulette : {
            return QString (QObject::tr("This mate occurs when the King is on his back rank and is trapped between 2 Rooks. The 2 Rooks looks like the ornamental shoulder pieces worn on military uniforms."));
        }

        case ePatternMatArabes : {
            return QString (QObject::tr("The Arabian mate occurs when the King is trapped on a corner of the board and is attacked by a Rook and a Knight."));
        }

        case ePatternMatGreco : {
            return QString (QObject::tr("The Greco's mate involves the combination of a Bishop and a Queen (or a Rook) to mate a King trapped in a corner."));
        }

        case ePatternMatBoden : {
            return QString (QObject::tr("THe Boden's mate involves 2 Bishop for attacking the King's position. This last one being obstructed by a piece."));
        }

        case ePatternMatAnastasie : {
            return QString (QObject::tr("THe Anastasia's mate involves a Rook and a Knight to trap the opposing King between the side of the chessboard and a friendly piece."));
        }

        case ePatternMatLegal : {
            return QString (QObject::tr("The Knight moves into a position to check the King. The Bishop is guarded by the other Knight, and the enemy pieces block the King's escape."));
        }

        default : return QString("");

    }
}

QString FournisseurDemoPatternsMat::get_ExtensionXMLFileName () const {

    return QString ("PatternsMat");
}
