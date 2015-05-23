#ifndef FACTORYFOURNISSEURDEMO_H
#define FACTORYFOURNISSEURDEMO_H

#include "TypeDemo.hpp"
#include "IFournisseurDemo.h"
#include <QListWidget>
#include <boost/shared_ptr.hpp>

class FactoryFournisseurDemo
{
public:
    static boost::shared_ptr<IFournisseurDemo> getFournisseur (eTypeInitiation aTypeInitiation);
    static QListWidget *getListeDemo (eTypeInitiation aTypeInitiation,
                                      QWidget* aParent,
                                      const IFournisseurDemo& aFournisseur);
    static void Translate(eTypeInitiation aTypeInitiation,
                          QListWidget *aListeDemo,
                          IFournisseurDemo& aFournisseur);
private:
    FactoryFournisseurDemo();
    static boost::shared_ptr<IFournisseurDemo> _instanceFournisseurPiecesMoves;
    static boost::shared_ptr<IFournisseurDemo> _instanceFournisseurSpecialMoves;
    static boost::shared_ptr<IFournisseurDemo> _instanceFournisseurPatternsMat;
    static boost::shared_ptr<IFournisseurDemo> _instanceFournisseurTactiques;
};

#endif // FACTORYFOURNISSEURDEMO_H
