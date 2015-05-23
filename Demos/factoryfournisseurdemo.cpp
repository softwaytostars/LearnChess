#include "factoryfournisseurdemo.h"
#include "fournisseurdemomovepiece.h"
#include "fournisseurdemospecialmoves.h"
#include "fournisseurdemopatternsmat.h"
#include "fournisseurdemotactics.h"
#include "qlistepieces.h"

boost::shared_ptr<IFournisseurDemo> FactoryFournisseurDemo::_instanceFournisseurPiecesMoves;
boost::shared_ptr<IFournisseurDemo> FactoryFournisseurDemo::_instanceFournisseurSpecialMoves;
boost::shared_ptr<IFournisseurDemo> FactoryFournisseurDemo::_instanceFournisseurPatternsMat;
boost::shared_ptr<IFournisseurDemo> FactoryFournisseurDemo::_instanceFournisseurTactiques;

FactoryFournisseurDemo::FactoryFournisseurDemo()
{
}

boost::shared_ptr<IFournisseurDemo> FactoryFournisseurDemo::getFournisseur (eTypeInitiation aTypeInitiation) {

    switch (aTypeInitiation) {
        case eInitierPiecesMoves  : { if (_instanceFournisseurPiecesMoves.get()==0)
                                        _instanceFournisseurPiecesMoves = boost::shared_ptr<IFournisseurDemo> (new FournisseurDemoMovePiece ());
                                      return _instanceFournisseurPiecesMoves;
                                     }
        case eInitierSpecialMoves : { if (_instanceFournisseurSpecialMoves.get()==0)
                                        _instanceFournisseurSpecialMoves = boost::shared_ptr<IFournisseurDemo> (new FournisseurDemoSpecialMoves ());
                                      return _instanceFournisseurSpecialMoves;
                                     }
        case eInitierPatternsMat  : { if (_instanceFournisseurPatternsMat.get()==0)
                                        _instanceFournisseurPatternsMat = boost::shared_ptr<IFournisseurDemo> (new FournisseurDemoPatternsMat ());
                                      return _instanceFournisseurPatternsMat;
                                     }
        case eInitierTactiques    : { if (_instanceFournisseurTactiques.get()==0)
                                        _instanceFournisseurTactiques = boost::shared_ptr<IFournisseurDemo> (new FournisseurDemoTactics ());
                                      return _instanceFournisseurTactiques;
                                     }
    }
}

//cree la liste et la remplit
QListWidget *FactoryFournisseurDemo::getListeDemo (eTypeInitiation aTypeInitiation,
                                                   QWidget* aParent,
                                                   const IFournisseurDemo& aFournisseur) {

    switch (aTypeInitiation) {

        case eInitierPiecesMoves : {

            QListePieces *lListe = new QListePieces (WHITE, false, aParent);
            lListe->ReInitListe(aFournisseur.get_ListeItemNames());
            return ((QListWidget *)lListe);
        }
        case eInitierSpecialMoves :
        case eInitierPatternsMat  :
        case eInitierTactiques    : {

            QListWidget *lListe = new QListWidget (aParent);
            for (int i=0; i < aFournisseur.get_ListeItemNames().count(); i++)
                lListe->addItem(aFournisseur.get_ListeItemNames()[i]);
            return lListe;
        }

        default : {return 0;}

    }
}

void FactoryFournisseurDemo::Translate(eTypeInitiation aTypeInitiation,
                                              QListWidget *aListeDemo,
                                              IFournisseurDemo& aFournisseur) {

    aFournisseur.translate();

    switch (aTypeInitiation) {

        case eInitierPiecesMoves : {

        QListePieces *lListe = dynamic_cast<QListePieces*> (aListeDemo);
        if (lListe)
            lListe->ReInitListe(aFournisseur.get_ListeItemNames());
        }
        case eInitierSpecialMoves :
        case eInitierPatternsMat  :
        case eInitierTactiques    : {

            aListeDemo->clear();
            for (int i=0; i < aFournisseur.get_ListeItemNames().count(); i++)
                aListeDemo->addItem(aFournisseur.get_ListeItemNames()[i]);
        }

        default : {break;}

    }
}
