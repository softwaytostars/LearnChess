#include "fournisseurdemospecialmoves.h"
#include "TypeDemo.hpp"

FournisseurDemoSpecialMoves::FournisseurDemoSpecialMoves()
    :IFournisseurDemo ()
{

    AssociateIndexDemoAndXMLNodeName ((int) ePetitRoqueMove,     "DemosLittleCastling");
    AssociateIndexDemoAndXMLNodeName ((int) eGrandRoqueMove,     "DemosBigCastling");
    AssociateIndexDemoAndXMLNodeName ((int) ePriseEnPassantMove, "DemosEnPassant");
    AssociateIndexDemoAndXMLNodeName ((int) ePromotionMove,      "DemosPromotion");

    AjouterTypeDemoNames ();

}

void FournisseurDemoSpecialMoves::AjouterTypeDemoNames () {

    AssociateIndexDemoAndTypeDemoName ((int) ePetitRoqueMove,    QObject::tr ("Little Castling"));
    AssociateIndexDemoAndTypeDemoName ((int) eGrandRoqueMove,     QObject::tr ("Big Castling"));
    AssociateIndexDemoAndTypeDemoName ((int) ePriseEnPassantMove, QObject::tr ("En passant"));
    AssociateIndexDemoAndTypeDemoName ((int) ePromotionMove,      QObject::tr ("Promotion"));
}

FournisseurDemoSpecialMoves::~FournisseurDemoSpecialMoves() {

}

QString FournisseurDemoSpecialMoves::get_SmallDescription (int aIndexDemo) const {

   eTypeDemoSpecialMove lTypeDemoSpecialMove = eTypeDemoSpecialMove (aIndexDemo);

   switch (lTypeDemoSpecialMove) {
        case ePetitRoqueMove : return (QObject::tr("The Little castling move"));
        case eGrandRoqueMove : return (QObject::tr("The Big castling move"));
        case ePriseEnPassantMove : return (QObject::tr("The EnPassant capture"));
        case ePromotionMove : return (QObject::tr("The promotion"));
        default : return QString ("");
    }

}

QString FournisseurDemoSpecialMoves::get_DetailedDescription(int aIndexDemo) const {


    eTypeDemoSpecialMove lTypeDemoSpecialMove = eTypeDemoSpecialMove (aIndexDemo) ;

    switch (lTypeDemoSpecialMove) {
            case ePetitRoqueMove :
            case eGrandRoqueMove :
                {
                return (QObject::tr ("Castling is permissible if and only if all of the following conditions hold :\n")
                                   + QObject::tr ("The king has not previously moved.\n")
                                   + QObject::tr ("The chosen rook has not previously moved.\n")
                                   + QObject::tr ("There are no pieces between the king and the chosen rook.\n")
                                   + QObject::tr ("The king is not currently in check.\n")
                                   + QObject::tr ("The king does not pass through a square that is under attack by enemy pieces.\n")
                                   + QObject::tr ("The king does not end up in check (true of any legal move).\n")
                                   + QObject::tr ("The king and the chosen rook are on the same rank."));

                }
            case ePriseEnPassantMove : {
                return (QObject::tr("It is a special pawn capture which can occur immediately after a player moves a pawn two squares forward from its starting position, and an enemy pawn could have captured it had it moved only one square forward. The opponent captures the just-moved pawn as if taking it 'as it passes' through the first square. The resulting position is the same as if the pawn had moved only one square forward and the enemy pawn had captured normally.\n")
                        + QObject::tr("The en passant capture must be done on the very next turn, or the right to do so is lost. Such a move is the only occasion in chess in which a piece captures but does not move to the square of the captured piece."));
            }
            case ePromotionMove : {
                return (QObject::tr("Promotion is a chess rule describing the transformation of a pawn that reaches its eighth rank into the player's choice of a queen, knight, rook, or bishop of the same color. The new piece replaces the pawn on the same square. Promotion is not limited to pieces that have already been captured. Every pawn that reaches its eighth rank must be promoted"));
            }

            default : return QString("");
        }//switch (lTypeDemoSpecialMove)
}

QString FournisseurDemoSpecialMoves::get_ExtensionXMLFileName () const {

    return QString ("SpecialMoves");
}

