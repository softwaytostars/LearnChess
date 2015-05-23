#include "fournisseurdemomovepiece.h"
#include "qlistepieces.h"

#include "TypeDemo.hpp"
#include "infosgraphiques.h"

FournisseurDemoMovePiece::FournisseurDemoMovePiece()
    : IFournisseurDemo ()
{

    _TypesPieces.push_back(Rook);
    _TypesPieces.push_back(Knight);
    _TypesPieces.push_back(Bishop);
    _TypesPieces.push_back(Queen);
    _TypesPieces.push_back(King);
    _TypesPieces.push_back(Pawn);

    for (int i=0; i < _TypesPieces.count(); i++) {

        switch (_TypesPieces [i]) {

            case Rook   : {AssociateIndexDemoAndXMLNodeName ((int)(Rook),   "DemosRook");break;}
            case Knight : {AssociateIndexDemoAndXMLNodeName ((int)(Knight), "DemosKnight");break;}
            case Bishop : {AssociateIndexDemoAndXMLNodeName ((int)(Bishop), "DemosBishop");break;}
            case Queen  : {AssociateIndexDemoAndXMLNodeName ((int)(Queen),  "DemosQueen");break;}
            case King   : {AssociateIndexDemoAndXMLNodeName ((int)(King),   "DemosKing");break;}
            case Pawn   : {AssociateIndexDemoAndXMLNodeName ((int)(Pawn),   "DemosPawn");break;}
        }//switch
    }//for

    AjouterTypeDemoNames ();
}

void FournisseurDemoMovePiece::AjouterTypeDemoNames () {

    for (int i=0; i < _TypesPieces.count(); i++) {

        AssociateIndexDemoAndTypeDemoName((int)_TypesPieces [i],
                                          InfosGraphiques::instance()->get_NomPiece(_TypesPieces [i] ));
    }
}

FournisseurDemoMovePiece::~FournisseurDemoMovePiece () {

}

QString FournisseurDemoMovePiece::get_SmallDescription(int aIndexDemo) const {

    eTypePiece lTypeDemoMovePiece = eTypePiece (aIndexDemo);

    switch (lTypeDemoMovePiece) {
        case Rook   : return (QObject::tr("The Rook"));
        case Knight : return (QObject::tr("The Knight"));
        case Bishop : return (QObject::tr("The Bishop"));
        case King   : return (QObject::tr("The King")); ;
        case Queen  : return (QObject::tr("The Queen"));
        case Pawn   : return (QObject::tr("The Pawn"));
        default     : return QString ("");
    }
}

QString FournisseurDemoMovePiece::get_DetailedDescription(int aIndexDemo) const {

    eTypePiece lTypeDemoMovePiece = eTypePiece (aIndexDemo) ;
    QString lRegleOr = QObject::tr("\n GOLD RULE : a piece can move only if after the move, the friend King is not in check!");

    switch (lTypeDemoMovePiece) {
        case Rook : {
           return QString (QObject::tr("The rook moves horizontally or vertically, through any number of unoccupied squares.\n")
                   + QObject::tr("As with captures by other pieces, the rook captures by occupying the square on which the enemy piece sits."))
                   + lRegleOr;
        }

        case Knight : {
            return (QObject::tr ("The knight move is unusual among chess pieces. When it moves, it can move two squares horizontally and one square vertically, or two squares vertically and one square horizontally.\n")
                    + QObject::tr("The complete move therefore looks like the letter 'L'.\n")
                    + QObject::tr("Unlike all other standard chess pieces, the knight can 'jump over' all other pieces (of either color) to its destination square.\n")
                    + QObject::tr("It captures an enemy piece by moving into its square.")
                    + lRegleOr);
        }

        case Bishop : {
            return (QObject::tr ("The bishop has no restrictions in distance for each move, but is limited to diagonal movement.\n")
                    + QObject::tr("Bishops, like all other pieces except the knight, cannot jump over other pieces.\n")
                         + QObject::tr("A bishop captures by occupying the square on which an enemy piece sits.\n")
                         + QObject::tr("As a consequence of its diagonal movement, each bishop always remains on either the white or black squares, and so it is also common to refer to them as light-squared or dark-squared bishops.")
                    + lRegleOr);
        }

        case King : {
            return (QObject::tr("A king can move one square in any direction (horizontally, vertically, or diagonally) unless the square is already occupied by a friendly piece or the move would place the king in check (ie under attack).")
                    + lRegleOr);
        }

        case Queen : {
            return (QObject::tr("The queen can be moved any number of unoccupied squares in a straight line vertically, horizontally, or diagonally, thus combining the moves of the rook and bishop. The queen captures by occupying the square on which an enemy piece sits.")
                    + lRegleOr);
        }

        case Pawn : {
            return (QObject::tr("Pawns are unusual in how they move. Unlike the other pieces, pawns may not move backwards. Normally a pawn moves by advancing a single square, but the first time a pawn is moved, it has the option of advancing two squares. Pawns may not use the initial two-square advance to jump over an occupied square, or to capture. Any piece directly in front of a pawn, friend or foe, blocks its advance.")
                    + lRegleOr);
        }

        default : return QString("");

    }
}

bool FournisseurDemoMovePiece::setStaticPieceAndShowItsPossiblePath (int aIndexDemo, ArticlePiece& aArticlePiece) const {

   eTypePiece lTypeDemoMovePiece = eTypePiece (aIndexDemo) ;

   switch (lTypeDemoMovePiece) {

       case Rook : {
           aArticlePiece =  ArticlePiece (0, Rook, WHITE, 4, 4, false);
           return true;
       }
       case Knight : {
            aArticlePiece =  ArticlePiece (0, Knight, WHITE, 4, 4, false);;
            return true;
       }
       case Bishop : {
           aArticlePiece =  ArticlePiece (0, Bishop, WHITE, 4, 4, false);
           return true;
       }
       case King : {
           aArticlePiece =  ArticlePiece (0, King, WHITE, 4, 4, false);
           return true;
       }
       case Queen : {
           aArticlePiece =  ArticlePiece (0, Queen, WHITE, 4, 4, false);
           return true;
       }

       case Pawn : {
           aArticlePiece =  ArticlePiece (0, Pawn, WHITE, 4, 2, false);
           return true;
       }

       default : return false;

   }//switch (lTypeDemoMovePiece)

}

QString FournisseurDemoMovePiece::get_ExtensionXMLFileName () const {

    return QString ("PiecesMoves");
}
