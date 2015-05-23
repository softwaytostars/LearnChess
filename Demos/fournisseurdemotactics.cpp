#include "fournisseurdemotactics.h"

#include "TypeDemo.hpp"

FournisseurDemoTactics::FournisseurDemoTactics()
    :IFournisseurDemo ()
{

    AssociateIndexDemoAndXMLNodeName ((int) eTacticDoubleAttack,         "DemosTacticDoubleAttack");
    AssociateIndexDemoAndXMLNodeName ((int) eTacticDiscoveredAttack,     "DemosTacticDiscoveredAttack");
    AssociateIndexDemoAndXMLNodeName ((int) eTacticPinAndSkewer,         "DemosTacticPinAndSkewer");
    AssociateIndexDemoAndXMLNodeName ((int) eTacticRemovingGuard,        "DemosTacticRemovingGuard");
   // AssociateIndexDemoAndXMLNodeName ((int) eTacticInterception,         "DemosTacticInterception");
    AssociateIndexDemoAndXMLNodeName ((int) eTacticOverLoading,          "DemosTacticOverLoading");
    AssociateIndexDemoAndXMLNodeName ((int) eTacticDeflection,           "DemosTacticDeflection");
    AjouterTypeDemoNames ();
}

void FournisseurDemoTactics::AjouterTypeDemoNames () {

    AssociateIndexDemoAndTypeDemoName ((int) eTacticDoubleAttack,      QObject::tr("The Double Attack"));
    AssociateIndexDemoAndTypeDemoName ((int) eTacticDiscoveredAttack,  QObject::tr("The Discovered Attack"));
    AssociateIndexDemoAndTypeDemoName ((int) eTacticPinAndSkewer,      QObject::tr("The Pin and the skewer"));
    AssociateIndexDemoAndTypeDemoName ((int) eTacticRemovingGuard,     QObject::tr("Removing defenser"));
    //AssociateIndexDemoAndTypeDemoName ((int) eTacticInterception,      QObject::tr("Interception"));
    AssociateIndexDemoAndTypeDemoName ((int) eTacticOverLoading,       QObject::tr("OverLoading"));
    AssociateIndexDemoAndTypeDemoName ((int) eTacticDeflection,        QObject::tr("Deflection"));
}

FournisseurDemoTactics::~FournisseurDemoTactics() {

}

QString FournisseurDemoTactics::get_SmallDescription(int aIndexDemo) const {

    QString lresult;
    //on prend le meme nom donné dans la liste
    if (this->get_TypeDemoNameForIndexDemo(aIndexDemo, lresult))
        return lresult;
    else return QString("");

}

QString FournisseurDemoTactics::get_DetailedDescription(int aIndexDemo) const {

    eTypeDemoTactics lTypeDemoTactic = eTypeDemoTactics (aIndexDemo) ;

    switch (lTypeDemoTactic) {
            case eTacticDoubleAttack     : return QObject::tr("This procedure involves attacking two (or more) pieces or squares simultaneously.");
            case eTacticDiscoveredAttack : return QObject::tr ("In chess, a discovered attack is an attack revealed when one piece moves out of the way of another.\n Discovered attacks can be extremely powerful, as the piece moved can make a threat independently of the piece it reveals. Like many chess tactics, they succeed because the opponent is unable to meet two threats at once");
            case eTacticPinAndSkewer     : return QObject::tr("The Pin and skewer involves the participation of three pieces: a piece is attacking another one which is being protected by a third one placed between the two.\n The Pin occurs when a piece cannot move without exposing a more valuable piece. The skewer occurs when a piece attacks another one which has to move (for example King in check) and then exposes another one.");
            case eTacticRemovingGuard    : return QObject::tr("This procedure is based on suppressing a piece which is protecting another piece or another square on which a potential attack could occur.");
            //case eTacticInterception     : return QObject::tr("It involves the blocking of a direction (rank, file or diagonal) in the idea of reducing the range of action of an enemy piece. It is often done with a piece sacrifice.");
            case eTacticOverLoading      : return QObject::tr("It often happens that a piece may protect two or more objectives at the same time. When this happens we refer to that piece as being overloaded. Usually, when your opponent has an overloaded piece you can use it in your favor by performing powerful attacks on his position (more exactly on the overloaded piece).");
            case eTacticDeflection       : return QObject::tr("Deflection is a widely used tactical procedure. It is a maneuver that seeks to guide away an enemy piece, which defends a position, in order to perform a checkmate or win a piece.");
            default : return QString("");
        }//switch (lTypeDemoTactic);
}

QString FournisseurDemoTactics::get_ExtensionXMLFileName () const {

    return QString ("Tactics");
}

