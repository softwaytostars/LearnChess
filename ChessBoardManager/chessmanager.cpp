#include "chessmanager.h"
#include "Data/ArticlePiece.hpp"
#include "TypePiece.hpp"
#include "ObserverChessManager.h"
#include "FactoryRuleMovePiece.h"
#include "Irulemovepiece.h"
#include "RuleMoveKnight.h"
#include "commandplay.h"

#ifdef _DEBUG
#include <QDebug>
#endif



ChessManager::ChessManager(const tlisteArticlesPieces &aListesPieces)
{
    Init (aListesPieces);
}

ChessManager::ChessManager (bool aAvecInit) {
    tlisteArticlesPieces lListesPieces;
    Init (lListesPieces, aAvecInit);
}

void ChessManager::Init (const tlisteArticlesPieces &aListesPieces, bool aAvecInit) {

    _WhiteKingIsCastled = false;
    _BlackKingIsCastled = false;

    _ModePermissif = false;
    _DesactiverNotifications = false;
    _ModeAllowFirstPlayBlack = false;

    FactoryRuleMovePiece::Attach();
    //creation des donnees pieces de l'echiquier
    //par defaut, toutes les pieces, sinon seulement celles precisees
    if (aAvecInit && (aListesPieces.size() == 0)){
        CreationDesPieces();
    }else{
        for (tlisteArticlesPieces::const_iterator it=aListesPieces.begin();
             it != aListesPieces.end();
             ++it) {
            _tableDataPieces.CreerArticle(*it);
        }//for
    }
}


ChessManager::~ChessManager () {
    FactoryRuleMovePiece::Detach();
}

void ChessManager::CreationDesPieces() {
    try{
        tlisteArticlesPieces lListesPieces;
        ResetPieces (lListesPieces);//efface toutes les pieces et n'en repose aucune car liste vide
        CreationPiece (Rook, WHITE,   GetXPos ('A'), 1);
        CreationPiece (Knight, WHITE, GetXPos ('B'), 1);
        CreationPiece (Bishop, WHITE, GetXPos ('C'), 1);
        CreationPiece (Queen,  WHITE, GetXPos ('D'), 1);
        CreationPiece (King,   WHITE, GetXPos ('E'), 1);
        CreationPiece (Bishop, WHITE, GetXPos ('F'), 1);
        CreationPiece (Knight, WHITE, GetXPos ('G'), 1);
        CreationPiece (Rook ,  WHITE, GetXPos ('H'), 1);
        CreationPiece (Pawn,   WHITE, GetXPos ('A'), 2);
        CreationPiece (Pawn,   WHITE, GetXPos ('B'), 2);
        CreationPiece (Pawn,   WHITE, GetXPos ('C'), 2);
        CreationPiece (Pawn,   WHITE, GetXPos ('D'), 2);
        CreationPiece (Pawn,   WHITE, GetXPos ('E'), 2);
        CreationPiece (Pawn,   WHITE, GetXPos ('F'), 2);
        CreationPiece (Pawn ,  WHITE, GetXPos ('G'), 2);
        CreationPiece (Pawn ,  WHITE, GetXPos ('H'), 2);

        //for black pieces
        CreationPiece (Rook,   BLACK, GetXPos ('A'), 8);
        CreationPiece (Knight, BLACK, GetXPos ('B'), 8);
        CreationPiece (Bishop, BLACK, GetXPos ('C'), 8);
        CreationPiece (Queen,  BLACK, GetXPos ('D'), 8);
        CreationPiece (King,   BLACK, GetXPos ('E'), 8);
        CreationPiece (Bishop, BLACK, GetXPos ('F'), 8);
        CreationPiece (Knight, BLACK, GetXPos ('G'), 8);
        CreationPiece (Rook ,  BLACK, GetXPos ('H'), 8);
        CreationPiece (Pawn,   BLACK, GetXPos ('A'), 7);
        CreationPiece (Pawn,   BLACK, GetXPos ('B'), 7);
        CreationPiece (Pawn,   BLACK, GetXPos ('C'), 7);
        CreationPiece (Pawn,   BLACK, GetXPos ('D'), 7);
        CreationPiece (Pawn,   BLACK, GetXPos ('E'), 7);
        CreationPiece (Pawn,   BLACK, GetXPos ('F'), 7);
        CreationPiece (Pawn ,  BLACK, GetXPos ('G'), 7);
        CreationPiece (Pawn ,  BLACK, GetXPos ('H'), 7);
    }catch(...){

#ifdef _DEBUG
    qDebug()<<"PROBLEME LORS DE LA CreationDesPieces";
#endif

    }
}

void ChessManager::CreationPiece (eTypePiece aTypePiece, eColorPiece aColor, int aPosX, int aPosY, bool aHasEverMoved) {
    ArticlePiece lArticlePiece (-1, aTypePiece, aColor, aPosX, aPosY, aHasEverMoved);

    int lId = _tableDataPieces.CreerArticle(lArticlePiece);
    if (lId < 0) throw "probleme creation piece";
    NotifierAjoutPiece (lArticlePiece._TypePiece, lArticlePiece._Color, lArticlePiece._PosX, lArticlePiece._PosY);


}


int ChessManager::GetXPos (const char ACol) const
{
    int lPos = (int) (ACol)- (int) ('A')+1;
    if (lPos < 0) return 0;
    return (lPos);
}

char ChessManager::GetNomXPos (int aPos, bool aUpper) const
{
    char lresult = ((const char)  ((int) (aPos) -1 + (int) ('A')));
    if (!aUpper)
        return QChar(lresult).toLower().toAscii();
    else
        return lresult;
}


void ChessManager::RegisterObserver (ObserverChessManager& aObserver) {

    //ajoute l'observateur dans la liste
    _Observers.push_back(&aObserver);
}

void ChessManager::ResetPieces (const tlisteArticlesPieces& aListesPieces) {
    _tableDataPieces.SupprimerArticles();
    NotifierResetPieces ();
    for (tlisteArticlesPieces::const_iterator it = aListesPieces.begin();
         it != aListesPieces.end();
         ++it) {
        try {
            CreationPiece ((*it)._TypePiece, (*it)._Color, (*it)._PosX, (*it)._PosY);
        }catch (...) {
#ifdef _DEBUG
    qDebug()<<"PROBLEME ResetPieces";
#endif
        }
    }
}

void ChessManager::NotifierResetPieces () {

    if (_DesactiverNotifications) return;
    for (std::list<ObserverChessManager*>::const_iterator it = _Observers.begin();
         it != _Observers.end();
         ++it) {
        (*it)->DoOnResetPieces ();
    }
}

void ChessManager::NotifierAjoutPiece (eTypePiece aTypePiece, eColorPiece aColor, int aPosX, int aPosY) {

    if (_DesactiverNotifications) return;

    for (std::list<ObserverChessManager*>::const_iterator it = _Observers.begin();
         it != _Observers.end();
         ++it) {
        (*it)->DoOnAjouterPiece (aTypePiece, aColor, aPosX, aPosY);
    }
}

void ChessManager::NotifierModificationPlace (eTypePiece aGenrePiece,
                                              eColorPiece aColor,
                                              int aOldPosX, int aOldPosY,
                                              int anewPosX, int anewPosY) {

    if (_DesactiverNotifications) return;
    for (std::list<ObserverChessManager*>::const_iterator it = _Observers.begin();
         it != _Observers.end();
         ++it) {
        (*it)->DoOnModificationPlacePiece (aGenrePiece, aColor, aOldPosX, aOldPosY, anewPosX, anewPosY);
    }
}

void ChessManager::NotifierKillPiece (eTypePiece aGenrePiece, eColorPiece aColorPiece,
                                      int aPosX, int aPosY) {

    if (_DesactiverNotifications) return;
    for (std::list<ObserverChessManager*>::const_iterator it = _Observers.begin();
         it != _Observers.end();
         ++it) {
        (*it)->DoOnKillPiece (aGenrePiece, aColorPiece, aPosX, aPosY);
    }
}

void ChessManager::NotifierPlay (CommandPlay* aCommandPlay) const {

    if (_DesactiverNotifications) return;

    for (std::list<ObserverChessManager*>::const_iterator it = _Observers.begin();
         it != _Observers.end();
         ++it) {
        (*it)->DoOnPlay (aCommandPlay);
    }
}

bool ChessManager::UnePieceExiste (int aPosX, int aPosY, ArticlePiece &aArticlePiece) const {
   return _tableDataPieces.Existe(aPosX, aPosY, aArticlePiece);
}

bool ChessManager::ModifierPlacePiece (const ArticlePiece& aArticleFrom,
                                       int anewPosX,
                                       int anewPosY,
                                       bool aHasEverMoved) {

    //si meme position, fait rienq
    if ((anewPosX == aArticleFrom._PosX) && (anewPosY == aArticleFrom._PosY)) return false;

    ArticlePiece lArticle;
    if (!_tableDataPieces.Existe(aArticleFrom._PosX, aArticleFrom._PosY, lArticle)) {
        return false;
    }


    lArticle._PosX = anewPosX;
    lArticle._PosY = anewPosY;
    lArticle._HasEverMoved = aHasEverMoved;//flag piece a bougé

    //si base pas modifiee, sort
    if (!_tableDataPieces.ModifierArticle (lArticle._Id, lArticle)) {
        return false;
    }


    //REGLE : modification pas possible si le roi de meme couleur est attaqué suite à ce mouvement
    //Fait ici pour éviter développement transaction

//    if (!aForReplay && KingIsAttacked (aArticleFrom._Color)) {
//        //revient en arrière (rollback)
//        if (_tableDataPieces.ModifierArticle (lArticle._Id, aArticleFrom))
//            return false;//ne modifie rien
//    }

    NotifierModificationPlace (aArticleFrom._TypePiece, aArticleFrom._Color,
                               aArticleFrom._PosX, aArticleFrom._PosY,
                               anewPosX, anewPosY);
    return true;
}

bool ChessManager::AllowedMove (int aOldPosX, int aOldPosY,
                                int aNewPosX, int aNewPosY,
                                const CommandPlay* const aPreviousPlay,
                                bool aVerifIfKingIsCheck,
                                bool aDoNotCheckFriendFinalSquare) const {


    ArticlePiece lArticleFrom;
    //si piece n'existe pas à la position d'origine, pas possible
    if (!_tableDataPieces.Existe(aOldPosX, aOldPosY, lArticleFrom)) return false;

    IRuleMovePiece *lrule = FactoryRuleMovePiece::GetRulePiece(lArticleFrom._TypePiece);
    //attention, on ne fera pas le verif de roi en echec sur le mvt de piece autre que roi...!
    //il faudrait parcourir toutes les autres pieces à chaque fois...
    ArticlePiece lPieceEnnemy;
    bool lCanGoToSquare =  lrule
                            && lrule->AllowMove(*this,
                                                aOldPosX, aOldPosY,
                                                aNewPosX, aNewPosY,
                                                aPreviousPlay,
                                                aVerifIfKingIsCheck,
                                                aDoNotCheckFriendFinalSquare)
                            && (!PieceEstClouee (lArticleFrom, lPieceEnnemy)  || lPieceEnnemy.EstALaPosition(aNewPosX, aNewPosY));
    return lCanGoToSquare;
}

bool ChessManager::getArticlesPiecesDeCouleur (eColorPiece aColorPiece, tlisteArticlesPieces& aListeArticles) const {

    bool lListeNonVide = _tableDataPieces.getArticlesDeCouleur(aColorPiece, aListeArticles);
    return lListeNonVide;
}

bool ChessManager::ForcerAjoutPiece (eTypePiece aTypePiece,
                                     eColorPiece aColor,
                                     int aPosX, int aPosY,
                                     bool aHasEverMoved) {

    std::vector<int> lNumerosArticles;
    _tableDataPieces.getNumerosArticles(lNumerosArticles);
    if (lNumerosArticles.size() >= 32 ) return false;//si dejà 32 pièces, plus rien à ajouter

    //il faudrait aussi regarder par piece
    try{
        CreationPiece (aTypePiece, aColor, aPosX, aPosY, aHasEverMoved);
        return true;
    }catch(...){
        return false;
    }
}

//pour le kill d'une piece, on la supprime de la DB et on notifie aux observateurs.
//eux, comme la scene pourront simplement cacher la piece plutot que la detruire puis reconstruire car plus couteux
bool ChessManager::KillPiece (const ArticlePiece& aArticle) {

    if (!_tableDataPieces.SupprimerArticle (aArticle._PosX, aArticle._PosY)) return false;
    NotifierKillPiece (aArticle._TypePiece, aArticle._Color, aArticle._PosX, aArticle._PosY);
    return true;
}


bool ChessManager::ColorIsUnderAttackForPosition(eColorPiece aColor,
                                                 int aPosX, int aPosY,
                                                 tlisteArticlesPieces& alistePiecesEnnemies,
                                                 const tQVectorTypePiece &aTypesPiecesConsiderees) const {

    tlisteArticlesPieces llistePieces;
    eColorPiece lColorEnnemy = WHITE;
    if (aColor == WHITE)
        lColorEnnemy = BLACK;

    //parcours toutes les pieces de couleur opposée et regarde si elles peuvent se deplacer à la piece
    if (alistePiecesEnnemies.size () == 0) {
        if (aTypesPiecesConsiderees.count() == 0)
            getArticlesPiecesDeCouleur (lColorEnnemy, llistePieces);
        else {
            for (tQVectorTypePiece::const_iterator itTypePiece = aTypesPiecesConsiderees.begin();
                 itTypePiece != aTypesPiecesConsiderees.end();
                 ++itTypePiece) {
                _tableDataPieces.getArticlesDeCouleurEtType(lColorEnnemy, *itTypePiece, llistePieces, true);
            }
        }
    }

    for (tlisteArticlesPieces::const_iterator it=llistePieces.begin();
         it!=llistePieces.end();
         ++it) {
        //si pièce ennemie peut venir à la position voulue, le roi n'y a pas droit
        if (PieceAttacksThePosition ((*it), aPosX, aPosY))
            alistePiecesEnnemies.push_back((*it));
    }

    return (alistePiecesEnnemies.count() > 0);
}

bool ChessManager::getKing (eColorPiece aColorPiece, ArticlePiece& aKing) const {

    tlisteArticlesPieces lListeArticles;
    if (!_tableDataPieces.getArticlesDeCouleurEtType(aColorPiece, King, lListeArticles) || (lListeArticles.size() != 1)) return false;
    aKing = lListeArticles [0];
    return true;
}

bool ChessManager::KingIsAttacked (eColorPiece aColor,
                                   tlisteArticlesPieces& alistePiecesEnnemies,
                                   bool aStopFirstOne) const {
    ArticlePiece lKing;
    alistePiecesEnnemies.clear();

    if (getKing (aColor, lKing)) {

        //1) Chercher si cavalier ennemy autour
        tVecPairPos lSquaresKnight;
        RuleMoveKnight::getSquaresKnightFromPos (lKing._PosX, lKing._PosY, lSquaresKnight);
        for (int i=0; i < lSquaresKnight.count(); i++) {

            ArticlePiece lPiece;
            if (UnePieceExiste(lSquaresKnight[i].first, lSquaresKnight[i].second, lPiece)
                    && (lPiece._Color != aColor)
                    && (lPiece._TypePiece == Knight)) {
               alistePiecesEnnemies.push_back(lPiece);
               if (aStopFirstOne) return true;
            }
        }//for

        //2) Chercher si 2 pions ennemis bien places (en fait, tout sauf Cavalier et Tour peut attaquer)
        int lPosX1 = lKing._PosX-1;
        int lPosX2 = lKing._PosX+1;
        int lPosY  = lKing._PosY;
        if (lKing._Color == BLACK)
           lPosY -= 1;
        else
           lPosY += 1;
        ArticlePiece lPiece;
        if ( (UnePieceExiste(lPosX1, lPosY, lPiece) && (lPiece._Color != aColor))
             || (UnePieceExiste(lPosX2, lPosY, lPiece) && (lPiece._Color != aColor))) {

            if (lPiece._TypePiece == Pawn) {
                alistePiecesEnnemies.push_back(lPiece);
                if (aStopFirstOne) return true;
            }
        }
        //3) Chercher pieces ennemies qui donnent echec à distance (bon axe + pas de pieces entres)

        tlisteArticlesPieces listePiecesCouldCheck, lPieces;
        findPieces(this->get_ColorEnnemy(aColor), Rook, listePiecesCouldCheck);
        if (findPieces(this->get_ColorEnnemy(aColor), Bishop, lPieces))
            listePiecesCouldCheck += lPieces;
        if (findPieces(this->get_ColorEnnemy(aColor), Queen, lPieces))
            listePiecesCouldCheck += lPieces;
        for (int i=0; i < listePiecesCouldCheck.count(); i++) {

            DirectionsSearchPinningPiece *lSearch = DirectionsSearchPinningPiece::FournirDirectionSearch (*this, lKing, listePiecesCouldCheck[i]);
            if (lSearch) {
                if (!lSearch->UnePieceExisteEntreLeRoiEtPieceEvaluee())
                    alistePiecesEnnemies.push_back(listePiecesCouldCheck[i]);
                delete lSearch;
                lSearch = 0;
                if (aStopFirstOne && (alistePiecesEnnemies.count()>0)) return true;
            }
        }

//pas bon car considere que piece peut se deplacer (elle peut ne pas bouger car etre clouee mais mettre en echec qd meme)
//        if (ColorIsUnderAttackForPosition (lKing._Color, lKing._PosX, lKing._PosY, alistePiecesEnnemies, aTypesPiecesAConsiderer)) {
//            return true;
//        }
    }
    return (alistePiecesEnnemies.count() > 0);

}

bool ChessManager::KingIsAttacked (eColorPiece aColor) const {

    tlisteArticlesPieces llistePiecesEnnemies;
    return KingIsAttacked(aColor, llistePiecesEnnemies, true);
}

bool ChessManager::Remove (int aPosX, int aPosY) {

    bool lSupprime = _tableDataPieces.SupprimerArticle(aPosX, aPosY);
    return lSupprime;
}

bool ChessManager::ConditionCouleurAJouerRespectee (const CommandPlay* const aPreviousPlay,
                                                    eColorPiece aColorToPlay) const {

    if (_ModePermissif) return true;

    if (aPreviousPlay) {
        //pas droit de jouer 2 fois la même couleur
        if (aColorToPlay == aPreviousPlay->getColor()) return false;
    }
    else {
        //le premier coup doit être blanc
        if (!_ModeAllowFirstPlayBlack && (aColorToPlay != WHITE)) return false;
    }
    return true;
}

bool ChessManager::Play (int aOldPosX,
                         int aOldPosY,
                         int aNewPosX,
                         int aNewPosY,
                         CommandPlay*&aCommandPlay,
                         const CommandPlay* const aPreviousPlay) {

    aCommandPlay = 0;

    ArticlePiece lArticleFrom;
    //si piece n'existe pas à la position d'origine, pas possible
    if (!_tableDataPieces.Existe(aOldPosX, aOldPosY, lArticleFrom)) return false;

    if (!ConditionCouleurAJouerRespectee (aPreviousPlay, lArticleFrom._Color)) return false;

    IRuleMovePiece *lrule = FactoryRuleMovePiece::GetRulePiece(lArticleFrom._TypePiece);
    bool lDoPlay =  lrule && (lrule->DoPlay(*this, aOldPosX, aOldPosY, aNewPosX, aNewPosY, aCommandPlay, aPreviousPlay)
                     && aCommandPlay && aCommandPlay->Execute());
    if (lDoPlay)
        NotifierPlay (aCommandPlay);
    else {
        //cas ne devrait pas arriver mais au cas où tests pas tous faits!!!
        if (aCommandPlay) {
            aCommandPlay->UnExecute();
            delete aCommandPlay;
        }
    }
    return lDoPlay;
}

bool ChessManager::SimulationPlayOk (int aOldPosX,
                                     int aOldPosY,
                                     int aNewPosX,
                                     int aNewPosY) {
    CommandPlay* lCommandPlay = 0;

    ArticlePiece lArticleFrom;
    //si piece n'existe pas à la position d'origine, pas possible
    if (!_tableDataPieces.Existe(aOldPosX, aOldPosY, lArticleFrom)) return false;
    IRuleMovePiece *lrule = FactoryRuleMovePiece::GetRulePiece(lArticleFrom._TypePiece);
    bool lDoPlay =  lrule && lrule->DoPlay(*this, aOldPosX, aOldPosY, aNewPosX, aNewPosY, lCommandPlay, 0);
    if (lCommandPlay)  delete lCommandPlay;
    return lDoPlay;
}

bool ChessManager::findPieceAtPosThatCanMoveTo (int aPosXdepart,
                                                int aPosYdepart,
                                                eColorPiece aColor,
                                                eTypePiece  aTypePiece,
                                                int aPosX,
                                                int aPosY,
                                                ArticlePiece& aArticle) const {

    tlisteArticlesPieces lPieces;
    getPiecesThatCanMoveTo (aPosXdepart, aPosYdepart, aColor, aTypePiece, aPosX, aPosY, lPieces, true);
    if (lPieces.count() > 0)
        aArticle = lPieces [0];
    return (lPieces.count() > 0);
}

bool ChessManager::findPieceThatCanMoveTo (eColorPiece aColor,
                                           eTypePiece  aTypePiece,
                                           int aPosX,
                                           int aPosY,
                                           ArticlePiece& aArticle) const {
    tlisteArticlesPieces lPieces;
    getPiecesThatCanMoveTo (0, 0, aColor, aTypePiece, aPosX, aPosY, lPieces, true);
    if (lPieces.count() > 0)
        aArticle = lPieces [0];
    return (lPieces.count() > 0);
}

bool ChessManager::findAllPiecesThatCanMoveTo (eColorPiece aColor,
                                           eTypePiece  aTypePiece,
                                           int aPosX,
                                           int aPosY,
                                           tlisteArticlesPieces& aPieces) const {

    getPiecesThatCanMoveTo (0, 0, aColor, aTypePiece, aPosX, aPosY, aPieces, false);
    return (aPieces.count() > 0);
}


void ChessManager::getPiecesThatCanMoveTo (int aPosXdepart,
                                           int aPosYdepart,
                                           eColorPiece aColor,
                                           eTypePiece  aTypePiece,
                                           int aPosX,
                                           int aPosY,
                                           tlisteArticlesPieces& aListPieces,
                                           bool aStopForTheFirstOne) const {

    aListPieces.clear();

    tlisteArticlesPieces lListArticles;
    _tableDataPieces.getArticlesDeCouleurEtType (aColor, aTypePiece, lListArticles);

    IRuleMovePiece *lrule = FactoryRuleMovePiece::GetRulePiece(aTypePiece);
    if (!lrule) return;

    for (tlisteArticlesPieces::const_iterator it =lListArticles.begin();
         it != lListArticles.end();
         ++it) {
        if ((aPosXdepart > 0) && ((*it)._PosX != aPosXdepart)) continue;
        if ((aPosYdepart > 0) && ((*it)._PosY != aPosYdepart)) continue;
        if (lrule->AllowMove(*this, (*it)._PosX, (*it)._PosY, aPosX, aPosY, 0)) {
            aListPieces.push_back(*it);
            if (aStopForTheFirstOne) return;
        }
    }
}

bool ChessManager::get_SquaresForMoves (const ArticlePiece& aPiece,
                                        const CommandPlay *const aPreviousPlay,
                                        tQVectorInfosSquare & aListSquares,
                                        bool aCalculDefenseursAttaquants,
                                        bool aRayonX) const {

    IRuleMovePiece *lrule = FactoryRuleMovePiece::GetRulePiece(aPiece._TypePiece);
    if (lrule)
        return (lrule->get_SquaresForMoves(*this, aPiece, aPreviousPlay, aListSquares, aCalculDefenseursAttaquants, aRayonX));
    return false;
}

bool ChessManager::findPieces (eColorPiece aColor,
                                eTypePiece  aTypePiece,
                                tlisteArticlesPieces &alistePieces) const {

    return _tableDataPieces.getArticlesDeCouleurEtType (aColor, aTypePiece, alistePieces);
}

bool ChessManager::RooksAreAutoProtected (eColorPiece aColor, bool &aAxeHoriz) const {

    tlisteArticlesPieces llistePieces;
    if (!_tableDataPieces.getArticlesDeCouleurEtType (aColor, Rook, llistePieces) || (llistePieces.count()<2))
        return false;

    return RooksAreAutoProtected (llistePieces[0]._PosX, llistePieces[0]._PosY,
                                  llistePieces[1]._PosX, llistePieces[1]._PosY,
                                  aAxeHoriz);

}

bool ChessManager::RooksAreAutoProtected (int aPosX1, int aPosY1,
                                          int aPosX2, int aPosY2,
                                          bool& aAxeHoriz) const {


    bool lAligneesVert  = (aPosX1 == aPosX2);
    bool lAligneesHoriz = (aPosY1 == aPosY2);

    if (!lAligneesVert && !lAligneesHoriz) return false;

    aAxeHoriz = lAligneesHoriz;

    int lPosMin, lPosMax;
    ArticlePiece lPiece;

    //si une piece entre les 2 tours , ne sont pas interprotegees
    if (lAligneesVert) {
        lPosMin = qMin<int> (aPosY1, aPosY2);
        lPosMax = qMax<int> (aPosY1, aPosY2);
        for (int lPos = lPosMin+1; lPos < lPosMax; lPos++) {
            if (UnePieceExiste(aPosX1, lPos, lPiece))
                return false;
        }
    } else if (lAligneesHoriz) {
        lPosMin = qMin<int> (aPosX1, aPosX2);
        lPosMax = qMax<int> (aPosX1, aPosX2);
        for (int lPos = lPosMin+1; lPos < lPosMax; lPos++) {
            if (UnePieceExiste(lPos, aPosY1, lPiece))
                return false;
        }
    }
    return true;

}




bool ChessManager::PieceEstClouee(const ArticlePiece& aPiece, ArticlePiece& agetPieceEnnemy) const {

    ArticlePiece lKing;
    if (aPiece._TypePiece == King) return false;
    if (!this->getKing(aPiece._Color, lKing)) return false;

    DirectionsSearchPinningPiece *lSearch = DirectionsSearchPinningPiece::FournirDirectionSearch (*this, lKing, aPiece);

    bool lPieceClouee = false;
    if (lSearch) {
        lPieceClouee = lSearch->UnePieceEnnemieCloueLaPieceEvalue(agetPieceEnnemy);
        delete lSearch;
    }

   return lPieceClouee;

}


bool ChessManager::PieceAttacksThePosition (const ArticlePiece& aPiece, int aPosX, int aPosY) const {


    bool lDoNotCheckFriendFinalSquare = true;//si piece amie occupe la place, on la defend
    //exception du pion....il n'attaque que 2 cases
    if (aPiece._TypePiece == Pawn) {

        bool lIsBlackAndCanAttack = (aPiece._Color == BLACK)
                                    && ( (((aPiece._PosX-1) == aPosX) && ((aPiece._PosY-1) == aPosY))
                                                                || (((aPiece._PosX+1) == aPosX) && ((aPiece._PosY-1) == aPosY)));
        bool lIsWhiteAndCanAttack = (aPiece._Color == WHITE)
                                    && ( (((aPiece._PosX-1) == aPosX) && ((aPiece._PosY+1) == aPosY))
                                                                || (((aPiece._PosX+1) == aPosX) && ((aPiece._PosY+1) == aPosY)));

        ArticlePiece lPieceEnnemy;
        if (lIsBlackAndCanAttack || lIsWhiteAndCanAttack)
          return (!PieceEstClouee (aPiece, lPieceEnnemy) || lPieceEnnemy.EstALaPosition(aPosX, aPosY));
    }else if (AllowedMove(aPiece._PosX, aPiece._PosY, aPosX, aPosY, 0, false, lDoNotCheckFriendFinalSquare)) {//si pièce peut venir à la position voulue, on peut l'attaquer
        return true;
    }

    return false;
}

sInfosCastling ChessManager::getInfosSmallCastling () const {

    return sInfosCastling (GetXPos('E'), GetXPos('G'), GetXPos('H'), GetXPos('F'));

}

sInfosCastling ChessManager::getInfosBigCastling () const {

    return sInfosCastling (GetXPos('E'), GetXPos('C'), GetXPos('A'), GetXPos('D'));
}

bool ChessManager::CastlingIsPossibleForKing (const ArticlePiece &aKing,
                                              int aPosX,
                                              int aPosY,
                                              sDiagnostiqueCastling& aDiagnostiqueCastling) const {

    if (!aKing._TypePiece == King) return false;

    int lPosYRoque = 1;
    if (aKing._Color == BLACK)
        lPosYRoque = 8;

    if (aPosY != lPosYRoque) return false;


    bool lAskPetitRoque = (aPosX == getInfosSmallCastling()._PosKingDest)
                            && (aKing._PosX == getInfosSmallCastling()._PosKingToCastle)
                            && (lPosYRoque == aPosY)
                            && (lPosYRoque == aKing._PosY);
    bool lAskGrandRoque = (aPosX == getInfosBigCastling()._PosKingDest)
                            && (aKing._PosX == getInfosBigCastling()._PosKingToCastle)
                            && (lPosYRoque == aPosY)
                            && (lPosYRoque == aKing._PosY);

    if (!lAskPetitRoque && !lAskGrandRoque)  return false;

    aDiagnostiqueCastling._IsSmallCastling = lAskPetitRoque;

    /* roi et tour opposees ne doivent pas avoir bougé */
    if (aKing._HasEverMoved) return false;

    int lPosXTourARoquer;
    if (lAskPetitRoque) {
         lPosXTourARoquer    = getInfosSmallCastling()._PosRookToCastle;
     }
     else {
        lPosXTourARoquer     = getInfosBigCastling()._PosRookToCastle;
    }

    ArticlePiece lRookARoquer;
    if (!UnePieceExiste(lPosXTourARoquer, lPosYRoque, lRookARoquer)) return false;
    if (lRookARoquer._TypePiece != Rook) return false;
    if (lRookARoquer._HasEverMoved) return false;

    int lDiff = aPosX - aKing._PosX;
    int lDirection = lDiff/std::abs(1.0*lDiff);


    ArticlePiece lPieceGenante;
    /* il ne doit pas y avoir de piece entre le roi et la tour */
    for (int lPosX = aKing._PosX+lDirection; lPosX != lPosXTourARoquer; lPosX+=lDirection) {
        if (UnePieceExiste(lPosX, lPosYRoque, lPieceGenante)) return false;
    }

    tlisteArticlesPieces llistePiecesEnnemies;
    /* chemin du roi ne doit pas etre attaque par piece ennemie */
    for (int lPosX = aKing._PosX; lPosX != aPosX+lDirection; lPosX+=lDirection) {
        if (ColorIsUnderAttackForPosition (aKing._Color, lPosX, lPosYRoque, llistePiecesEnnemies)) {
            aDiagnostiqueCastling._PathIsAttacked = true;
            return false;
        }
    }//for

    return true;
}


int ChessManager::qHashKey (int aPosX, int aPosY) {

    return (aPosX*8 + aPosY);
}

void ChessManager::setInterdireAskPiecePromotionFor (const tQVectorColorPiece& aColorsWithoutAsk) {

    _ColorsForbiddenForAskPromotion.clear();
    _ColorsForbiddenForAskPromotion = aColorsWithoutAsk;
}



