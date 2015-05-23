#include "commandplay.h"
#include "IChessManager.h"
#include "infosgraphiques.h"

#ifdef _DEBUG
#include <QDebug>
#endif

CommandPlay::CommandPlay(IChessManager& aChessManager):
    _ChessManager (aChessManager),
    _HasBeenExecuted(false),
    _HasEverComputedIfEnnemyIsChecked(false),
    _Description("")
{
}

CommandPlay::~CommandPlay(){

    //libere memoire
    for (QVector<IAction*>::const_iterator it = _ListeActions.begin();
         it != _ListeActions.end();
         ++it) {
        delete *it;
    }
}

/********************************************/
/* Action Move ******************************/
/********************************************/

CommandPlay::ActionMove::ActionMove (IChessManager &aChessManager,
                                     const ArticlePiece &aArticleFrom,
                                     int anewPosX,
                                     int anewPosY):
    IAction(aChessManager),
    _ArticleFrom (aArticleFrom),
    _ArticleTo (aArticleFrom),
    _ExecutionCorrecte(false)
{
    _ArticleTo._PosX = anewPosX;
    _ArticleTo._PosY = anewPosY;
}

bool CommandPlay::ActionMove::Execute() {
   // std::cout<<"ActionMove::Execute"<<_ChessManager.GetNomXPos(_ArticleFrom._PosX)<<_ArticleFrom._PosY<<" TO "<<_ChessManager.GetNomXPos(_ArticleTo._PosX)<<_ArticleTo._PosY<<std::endl;
    _ExecutionCorrecte = _ChessManager.ModifierPlacePiece (_ArticleFrom, _ArticleTo._PosX, _ArticleTo._PosY);
    return _ExecutionCorrecte;
}

bool CommandPlay::ActionMove::UnExecute() {
   // std::cout<<"ActionMove::UnExecute"<<_ArticleTo._PosX<<" "<<_ArticleTo._PosY<<" TO "<<_ArticleFrom._PosX<<" "<<_ArticleFrom._PosY<<std::endl;
    if (_ExecutionCorrecte)
        _ChessManager.ModifierPlacePiece (_ArticleTo, _ArticleFrom._PosX, _ArticleFrom._PosY, _ArticleFrom._HasEverMoved);
    _ExecutionCorrecte = false;
    return true;
}

eColorPiece CommandPlay::ActionMove::getColorPieceInvolved () const {

    return _ArticleFrom._Color;
}

void CommandPlay::AddActionMove (const ArticlePiece &aArticleFrom, int anewPosX, int anewPosY) {

    _ListeActions.append (new ActionMove (_ChessManager, aArticleFrom, anewPosX, anewPosY));
}

/********************************************/
/* Action Manger Piece **********************/
/********************************************/

CommandPlay::ActionMangerPiece::ActionMangerPiece (IChessManager& aChessManager,
                                                   const ArticlePiece& aArticleFrom,
                                                   const ArticlePiece& aArticleTo):
    IAction (aChessManager),
    _ArticleFrom (aArticleFrom),
    _ArticleTo (aArticleTo),
    _Execution1Correcte(false),
    _Execution2Correcte (false)
{
}

bool CommandPlay::ActionMangerPiece::Execute() {

   // std::cout<<"ActionMangerPiece::Execute"<<_ChessManager.GetNomXPos(_ArticleFrom._PosX)<<_ArticleFrom._PosY<<" TO "<<_ChessManager.GetNomXPos(_ArticleTo._PosX)<<_ArticleTo._PosY<<std::endl;
    _Execution1Correcte = _ChessManager.KillPiece(_ArticleTo);
    if (!_Execution1Correcte) return false;
    _Execution2Correcte = _ChessManager.ModifierPlacePiece(_ArticleFrom, _ArticleTo._PosX, _ArticleTo._PosY);
    return _Execution2Correcte;
}

bool CommandPlay::ActionMangerPiece::UnExecute() {

    //std::cout<<"ActionMangerPiece::UnExecute"<<std::endl;
    if (_Execution2Correcte) {
        ArticlePiece lArticle = _ArticleFrom;
        lArticle._PosX = _ArticleTo._PosX;
        lArticle._PosY = _ArticleTo._PosY;
        _ChessManager.ModifierPlacePiece (lArticle, _ArticleFrom._PosX, _ArticleFrom._PosY,
                                          _ArticleFrom._HasEverMoved);
    }
    if (_Execution1Correcte)
        _ChessManager.ForcerAjoutPiece(_ArticleTo._TypePiece, _ArticleTo._Color,
                                       _ArticleTo._PosX, _ArticleTo._PosY,
                                       _ArticleTo._HasEverMoved);
    _Execution1Correcte = false;
    _Execution2Correcte = false;

    return true;
}

eColorPiece CommandPlay::ActionMangerPiece::getColorPieceInvolved () const {

    return _ArticleFrom._Color;
}

void CommandPlay::AddActionMangerPiece (const ArticlePiece &aArticleFrom, const ArticlePiece &aArticleTo) {

    _ListeActions.append (new ActionMangerPiece (_ChessManager, aArticleFrom, aArticleTo));
}

/********************************************/
/* Action Promotion *************************/
/********************************************/

CommandPlay::ActionPromotion::ActionPromotion (IChessManager &aChessManager,
                 const ArticlePiece &aArticleFrom, eTypePiece aGenrePromotion):
    IAction (aChessManager),
    _ArticleFrom (aArticleFrom),
    _GenrePromotion (aGenrePromotion),
    _Execution1Correcte(false),
    _Execution2Correcte (false)
{
}

bool CommandPlay::ActionPromotion::Execute() {

    _Execution1Correcte = _ChessManager.KillPiece(_ArticleFrom);
    if (!_Execution1Correcte) return false;

    _Execution2Correcte = _ChessManager.ForcerAjoutPiece(_GenrePromotion,
                                                         _ArticleFrom._Color,
                                                         _ArticleFrom._PosX,
                                                         _ArticleFrom._PosY,
                                                         true);

    return (_Execution2Correcte);
}

bool CommandPlay::ActionPromotion::UnExecute() {

   // std::cout<<"ActionPromotion::UnExecute"<<std::endl;
    if (_Execution2Correcte) {
        ArticlePiece lArticle = _ArticleFrom;
        lArticle._TypePiece   = _GenrePromotion;
        _ChessManager.KillPiece (lArticle);
    }
    if (_Execution1Correcte)
        _ChessManager.ForcerAjoutPiece (_ArticleFrom._TypePiece, _ArticleFrom._Color,
                                                  _ArticleFrom._PosX, _ArticleFrom._PosY,
                                                  true);
    _Execution1Correcte = false;
    _Execution2Correcte = false;
    return true;
}

eColorPiece CommandPlay::ActionPromotion::getColorPieceInvolved () const {

    return _ArticleFrom._Color;
}

void CommandPlay::AddActionPromotion (const ArticlePiece& aArticleFrom,
                                      eTypePiece aGenrePromotion) {

    _ListeActions.append (new ActionPromotion (_ChessManager, aArticleFrom, aGenrePromotion));
}

/********************************************/
/* Action Prise en Passant ******************/
/********************************************/

CommandPlay::ActionPriseEnPassant::ActionPriseEnPassant (IChessManager& aChessManager,
                                                         const ArticlePiece& aArticleFrom,
                                                         int anewPosX,
                                                         int anewPosY,
                                                         const ArticlePiece& aPawnToDelete):
        IAction (aChessManager),
        _ArticleFrom (aArticleFrom),
        _PawnToDelete (aPawnToDelete),
        _Execution1Correcte(false),
        _Execution2Correcte (false)
{
    _ArticleTo = _ArticleFrom;
    _ArticleTo._PosX = anewPosX;
    _ArticleTo._PosY = anewPosY;
}


bool CommandPlay::ActionPriseEnPassant::Execute() {

   // std::cout<<"ActionPriseEnPassant::Execute"<<_ChessManager.GetNomXPos(_ArticleFrom._PosX)<<_ArticleFrom._PosY<<" TO "<<_ChessManager.GetNomXPos(_ArticleTo._PosX)<<_ArticleTo._PosY<<std::endl;
    _Execution1Correcte = _ChessManager.ModifierPlacePiece(_ArticleFrom, _ArticleTo._PosX, _ArticleTo._PosY);
    if (!_Execution1Correcte) return false;
    _Execution2Correcte = _ChessManager.KillPiece(_PawnToDelete);
    return _Execution2Correcte;
}

bool CommandPlay::ActionPriseEnPassant::UnExecute() {

   // std::cout<<"ActionPriseEnPassant::UnExecute"<<std::endl;
    if (_Execution1Correcte) {
        ArticlePiece lArticle = _ArticleFrom;
        lArticle._PosX = _ArticleTo._PosX;
        lArticle._PosY = _ArticleTo._PosY;
        _ChessManager.ModifierPlacePiece (lArticle, _ArticleFrom._PosX, _ArticleFrom._PosY, _ArticleFrom._HasEverMoved);
    }
    if (_Execution2Correcte)
        _ChessManager.ForcerAjoutPiece(_PawnToDelete._TypePiece, _PawnToDelete._Color,
                                       _PawnToDelete._PosX, _PawnToDelete._PosY,
                                       _PawnToDelete._HasEverMoved);
    _Execution1Correcte = false;
    _Execution2Correcte = false;
    return true;
}

eColorPiece CommandPlay::ActionPriseEnPassant::getColorPieceInvolved () const {

    return _ArticleFrom._Color;
}

void CommandPlay::AddActionPriseEnPassant (const ArticlePiece& aArticleFrom,
                                           int aNewPosX,
                                           int aNewPosY,
                                           const ArticlePiece& aPawnToDelete) {

    _ListeActions.append (new ActionPriseEnPassant (_ChessManager, aArticleFrom, aNewPosX, aNewPosY, aPawnToDelete));
}


/********************************************/
/* Action Castling ******************/
/********************************************/

CommandPlay::ActionCastling::ActionCastling (IChessManager& aChessManager,
                                            const ArticlePiece& aKing,
                                            const ArticlePiece& aRook,
                                            int anewPosXKing,
                                            int anewPosXRook) :
    IAction (aChessManager),
    _King (aKing),
    _Rook (aRook) ,
    _newPosXKing (anewPosXKing),
    _newPosXRook (anewPosXRook),
    _Execution1Correcte(false),
    _Execution2Correcte (false)

{

}

bool CommandPlay::ActionCastling::Execute() {

//    if (_ChessManager.getInfosSmallCastling()._PosKingDest == _newPosXKing)
//        std::cout<<"O-O"<<std::endl;
//    else
//       std::cout<<"O-O-O"<<std::endl;
    _ChessManager.SetKingIsCastled(_King._Color, true);
    _Execution1Correcte = _ChessManager.ModifierPlacePiece (_King, _newPosXKing, _King._PosY);
    if (!_Execution1Correcte) return false;
    _Execution2Correcte = _ChessManager.ModifierPlacePiece (_Rook, _newPosXRook, _Rook._PosY);
    return _Execution2Correcte;
}

bool CommandPlay::ActionCastling::UnExecute() {

    _ChessManager.SetKingIsCastled(_King._Color, false);

    if (_Execution1Correcte) {
        ArticlePiece lKing = _King;
        lKing._PosX = _newPosXKing;
        _ChessManager.ModifierPlacePiece (lKing, _King._PosX, _King._PosY, _King._HasEverMoved);
    }
    if (_Execution2Correcte) {
        ArticlePiece lRook = _Rook;
        lRook._PosX = _newPosXRook;
        _ChessManager.ModifierPlacePiece (lRook, _Rook._PosX, _Rook._PosY, _Rook._HasEverMoved);
    }
    _Execution1Correcte = false;
    _Execution2Correcte = false;
    return true;
}

eColorPiece CommandPlay::ActionCastling::getColorPieceInvolved () const {

    return _King._Color;
}

void CommandPlay::AddActionCastling (eColorPiece aColor, bool aSmallCastle) {

    int lPosYRoque = 1;
    if (aColor == BLACK)
        lPosYRoque = 8;

    sInfosCastling lInfosCastling = _ChessManager.getInfosSmallCastling();
    if (!aSmallCastle)
        lInfosCastling = _ChessManager.getInfosBigCastling();

    ArticlePiece lKing, lRook;
    if (_ChessManager.UnePieceExiste(lInfosCastling._PosKingToCastle, lPosYRoque, lKing)) {
        if (_ChessManager.UnePieceExiste(lInfosCastling._PosRookToCastle, lPosYRoque, lRook)){
            _ListeActions.append (new ActionCastling (_ChessManager,
                                                      lKing,
                                                      lRook,
                                                      lInfosCastling._PosKingDest,
                                                      lInfosCastling._PosRookDest));

        }
    }//if King

}



bool CommandPlay::Execute(bool aComputeIfEnnemyIsChecked) {

    if (_HasBeenExecuted) return false;

    if (_Description.size() == 0)
        _Description = ComputeDescription();

    _HasBeenExecuted = true;
    for (QVector<IAction*>::const_iterator it = _ListeActions.begin();
         it != _ListeActions.end();
         ++it) {
        if (!(*it))
            return false;
        if (!(*it)->Execute ())
            return false;
    }

    //calcul echec apres le move
    if (aComputeIfEnnemyIsChecked && !_HasEverComputedIfEnnemyIsChecked) {

        _HasEverComputedIfEnnemyIsChecked = true;
        _PiecesGivingCheck.clear();
        //calcule pieces qui donnent echec
        _ChessManager.KingIsAttacked (_ChessManager.get_ColorEnnemy(getColor()),
                                                          _PiecesGivingCheck);
        //verifie si roi ennemi en echec
        if ((_Description.size() > 0) && this->KingEnnemyIsChecked())
            _Description += QString ("+");
    }

    return true;
}

bool CommandPlay::UnExecute() {

    if (!_HasBeenExecuted) return false;
    _HasBeenExecuted = false;
    //unexecute dans l'autre sens (il faudrait des trsnactions pour faire rollback e cas d'echec d'un des actions!)
    QVector<IAction*>::const_iterator it = _ListeActions.end();
    while (it != _ListeActions.begin()) {
        --it;
        if (!(*it)) {
#ifdef _DEBUG
    qDebug()<<"PROBLEME CommandPlay::UnExecute *it=0" <<_Description ;
#endif
            continue;
        }
        if (!(*it)->UnExecute ()){
#ifdef _DEBUG
    qDebug()<<"PROBLEME CommandPlay::UnExecute action" <<_Description ;
#endif
        }
    }

    return true;
}

QString CommandPlay::AmbiguiteSurPiece (eColorPiece aColor, eTypePiece aTypePiece,
                                     int aFromPosX, int aFromPosY,
                                     int aToPosX, int aToPosY) const {
    tlisteArticlesPieces lPieces;
    if (_ChessManager.findAllPiecesThatCanMoveTo(aColor, aTypePiece, aToPosX, aToPosY, lPieces))
    {
        if (lPieces.count() >= 2) {
            for (tlisteArticlesPieces::const_iterator it = lPieces.begin();
                 it != lPieces.end();
                 ++it) {
                //si une autre piece peut s'y deplacer, marquer qu'il y a ambiguite
                if ( ((*it)._PosX != aFromPosX) || ((*it)._PosY != aFromPosY))
                {
                    if ((*it)._PosX != aFromPosX)
                        return QString(QChar (_ChessManager.GetNomXPos(aFromPosX, false)));
                    else
                        return QString ("%1").arg(aFromPosY);
                }
            }
        }
    }
    return QString("");
}

QString CommandPlay::ComputeDescription() const {

    eTypePiece lPieceFrom, lPiecePromote;
    int lPosXFrom, lPosYFrom, lPosXTo, lPosYTo;
    bool lAttaque;
    eColorPiece lColorPiece;

    QString lDescription;
    if (EstUnSimpleMove(lPieceFrom, lPosXFrom, lPosYFrom, lPosXTo, lPosYTo, lColorPiece)) {

        if (lPieceFrom == Pawn) {

            lDescription =  (QString (QChar (_ChessManager.GetNomXPos(lPosXTo, false))) +  QString::number(lPosYTo));

        } else {

            QString lRenseignement = AmbiguiteSurPiece (lColorPiece, lPieceFrom,
                                                        lPosXFrom, lPosYFrom,
                                                        lPosXTo, lPosYTo);
            lDescription =  (InfosGraphiques::instance()->get_NotationDePiece(lPieceFrom)+ lRenseignement + QString (QChar (_ChessManager.GetNomXPos(lPosXTo, false))) + QString::number(lPosYTo));

        }
    } else if (EstUneSimpleAttaque(lPieceFrom, lPosXFrom, lPosYFrom, lPosXTo, lPosYTo, lColorPiece)) {

        if (lPieceFrom == Pawn) {

            lDescription =  (QString (QChar (_ChessManager.GetNomXPos(lPosXFrom, false))) + QString("x") + QString (QChar (_ChessManager.GetNomXPos(lPosXTo, false))) + QString::number(lPosYTo));

        } else {

            QString lRenseignement = AmbiguiteSurPiece (lColorPiece, lPieceFrom,
                                                        lPosXFrom, lPosYFrom,
                                                        lPosXTo, lPosYTo);
            lDescription =  (InfosGraphiques::instance()->get_NotationDePiece(lPieceFrom) + lRenseignement + QString("x") + QString (QChar (_ChessManager.GetNomXPos(lPosXTo, false))) + QString::number(lPosYTo));

        }
    } else if (EstUnePromotion(lPosXFrom, lPosYFrom, lPosXTo, lPosYTo, lPiecePromote, lAttaque, lColorPiece)) {

        if (lAttaque)
            lDescription =  (QString (QChar (_ChessManager.GetNomXPos(lPosXFrom, false))) + QString("x") + QString (QChar (_ChessManager.GetNomXPos(lPosXTo, false))) + QString::number(lPosYTo) + QString("=") + InfosGraphiques::instance()->get_NotationDePiece(lPiecePromote));
        else
            lDescription =  (QString (QChar (_ChessManager.GetNomXPos(lPosXTo, false))) + QString::number(lPosYTo) + QString("=") + InfosGraphiques::instance()->get_NotationDePiece(lPiecePromote));

    } else if (EstUnGrandRoque(lColorPiece, lPosXTo, lPosYTo)) {

        lDescription =  QString ("O-O-O");

    } else if (EstUnPetitRoque(lColorPiece, lPosXTo, lPosYTo)) {

        lDescription =  QString ("O-O");

    } else if (EstUnePriseEnPassant (lPosXFrom, lPosYFrom, lPosXTo, lPosYTo))
    {
        lDescription = (QString (QChar (_ChessManager.GetNomXPos(lPosXFrom, false))) + QString("x") + QString (QChar (_ChessManager.GetNomXPos(lPosXTo, false))) + QString::number(lPosYTo)) + QString(" e.p.");

    } else return (QString (""));

    eColorPiece lEnnemyColor;
    if (lColorPiece == WHITE) lEnnemyColor = BLACK;
    else lEnnemyColor = WHITE;

    return lDescription;
}


/******************************************************/


bool CommandPlay::EstUnSimpleMove (eTypePiece& aPiece,
                                   int &aOldPosX,
                                   int &aOldPosY,
                                   int &aNewPosX,
                                   int &aNewPosY,
                                   eColorPiece& aColorPiece) const {

    if (_ListeActions.size() <=0 ) return false;//erreur metier

    if (_ListeActions.size() > 1 ) return false;

    ActionMove *lAction = dynamic_cast<ActionMove *>(_ListeActions [0]);
    if (!lAction) return false;

    aPiece      = lAction->_ArticleFrom._TypePiece;
    aOldPosX    = lAction->_ArticleFrom._PosX;
    aOldPosY    = lAction->_ArticleFrom._PosY;
    aNewPosX    = lAction->_ArticleTo._PosX;
    aNewPosY    = lAction->_ArticleTo._PosY;
    aColorPiece = lAction->_ArticleFrom._Color;
    return true;
}

bool CommandPlay::EstUneSimpleAttaque (eTypePiece& aPiece,
                                      int &aOldPosX,
                                      int &aOldPosY,
                                      int &aNewPosX,
                                      int &aNewPosY,
                                      eColorPiece& aColorPiece) const {

    if (_ListeActions.size() <=0 ) return false;//erreur metier

    if (_ListeActions.size() > 1 ) return false;

    ActionMangerPiece *lAction = dynamic_cast<ActionMangerPiece *>(_ListeActions [0]);
    if (!lAction) return false;

    aPiece      = lAction->_ArticleFrom._TypePiece;
    aOldPosX    = lAction->_ArticleFrom._PosX;
    aOldPosY    = lAction->_ArticleFrom._PosY;
    aNewPosX    = lAction->_ArticleTo._PosX;
    aNewPosY    = lAction->_ArticleTo._PosY;
    aColorPiece = lAction->_ArticleFrom._Color;

    return true;
}

bool CommandPlay::EstUnePromotion (int &aOldPosX,
                                   int &aOldPosY,
                                   int &aNewPosX,
                                   int &aNewPosY,
                                   eTypePiece& aPiecePromotion,
                                   bool &aAvecAttaque,
                                   eColorPiece& aColorPiece) const {

    if (_ListeActions.size() != 2 ) return false;

    ActionMangerPiece *lActionManger = dynamic_cast<ActionMangerPiece *>(_ListeActions [0]);
    aAvecAttaque = (lActionManger != 0);
    if (!lActionManger) {
        ActionMove *lActionMove = dynamic_cast<ActionMove *>(_ListeActions [0]);
        if (!lActionMove) return false;//erreur metier
        aOldPosX   = lActionMove->_ArticleFrom._PosX;
        aOldPosY   = lActionMove->_ArticleFrom._PosY;
        aNewPosX   = lActionMove->_ArticleTo._PosX;
        aNewPosY   = lActionMove->_ArticleTo._PosY;
    }else {
        aOldPosX   = lActionManger->_ArticleFrom._PosX;
        aOldPosY   = lActionManger->_ArticleFrom._PosY;
        aNewPosX   = lActionManger->_ArticleTo._PosX;
        aNewPosY   = lActionManger->_ArticleTo._PosY;
    }

    ActionPromotion *lActionPromote = dynamic_cast<ActionPromotion *>(_ListeActions [1]);
    if (!lActionPromote) return false;//erreur metier

    aPiecePromotion = lActionPromote->_GenrePromotion;
    aColorPiece     = lActionPromote->_ArticleFrom._Color;
    return true;

}


bool CommandPlay::EstUnRoque(bool &aEstPetitRoque, eColorPiece& aColorPiece, int& anewPosXKing, int &anewPosYKing) const {

    if (_ListeActions.size() != 1 ) return false;

    ActionCastling *lActionCastling = dynamic_cast<ActionCastling *>(_ListeActions [0]);

    if (!lActionCastling) return false;

    aColorPiece    = lActionCastling->getColorPieceInvolved();
    anewPosXKing   = lActionCastling->_newPosXKing;
    anewPosYKing   = lActionCastling->_King._PosY;
    aEstPetitRoque = (lActionCastling->_newPosXKing == _ChessManager.getInfosSmallCastling()._PosKingDest);

    return true;

}


bool CommandPlay::EstUnGrandRoque(eColorPiece& aColorPiece, int &aPosXKing, int &aPosYKing) const {

    bool lPetitRoque;
    return EstUnRoque (lPetitRoque, aColorPiece, aPosXKing, aPosYKing) && (!lPetitRoque);
}

bool CommandPlay::EstUnPetitRoque(eColorPiece& aColorPiece, int &aPosXKing, int &aPosYKing) const {

    bool lPetitRoque;
    return EstUnRoque (lPetitRoque, aColorPiece, aPosXKing, aPosYKing) && lPetitRoque;
}

bool CommandPlay::EstUnePriseEnPassant (int &aOldPosX,
                                        int &aOldPosY,
                                        int &aNewPosX,
                                        int &aNewPosY) const {

    if (_ListeActions.size() <=0 ) return false;//erreur metier

    if (_ListeActions.size() > 1 ) return false;

    ActionPriseEnPassant *lAction = dynamic_cast<ActionPriseEnPassant *>(_ListeActions [0]);
    if (lAction) {
        aOldPosX = lAction->_ArticleFrom._PosX;
        aOldPosY = lAction->_ArticleFrom._PosY;
        aNewPosX = lAction->_ArticleTo._PosX;
        aNewPosY = lAction->_ArticleTo._PosY;
        return true;
    }
    else return false;
}

bool CommandPlay::GetInfosPieceDeplacee (int &aOldPosX, int &aOldPosY,
                                         int &aNewPosX, int &aNewPosY) const {

    eTypePiece  lTypePiece;
    eColorPiece lColorPiece;
    if (EstUnSimpleMove (lTypePiece, aOldPosX, aOldPosY, aNewPosX, aNewPosY, lColorPiece))
        return true;
    if (EstUneSimpleAttaque(lTypePiece, aOldPosX, aOldPosY, aNewPosX, aNewPosY, lColorPiece))
        return true;
    if (EstUnPetitRoque(lColorPiece, aNewPosX, aNewPosY) || EstUnGrandRoque(lColorPiece, aNewPosX, aNewPosY))
        return true;
    bool lAttack;
    if (EstUnePromotion(aOldPosX, aOldPosY, aNewPosX, aNewPosY, lTypePiece, lAttack, lColorPiece))
        return true;
    if (EstUnePriseEnPassant(aOldPosX, aOldPosY, aNewPosX, aNewPosY))
       return true;
    return false;
}

eColorPiece CommandPlay::getColor() const {

    if (_ListeActions.size() <=0 ) return WHITE;
    return _ListeActions [0]->getColorPieceInvolved();
}

QString CommandPlay::getCommentaryMiseEnForme() const {

    //recherche de NAG:
    QString lNewComment = _Commentary;
    lNewComment.replace("$1", "!");
    lNewComment.replace("$2", "?");
    lNewComment.replace("$3", "!!");
    lNewComment.replace("$4", "??");
    lNewComment.replace("$5", "!?");
    lNewComment.replace("$6", "?!");
    lNewComment.remove('{');
    lNewComment.remove('}');

    return lNewComment;
}

void CommandPlay::FillActionsForDemo (QVector <IActionDemo*>& aListeActionsDemo, bool aAllDescriptions, bool aActionPlayer) const {
//ATTENTION, BUF PREVU SI AUTOMOVE avecaAllDescriptions=false sur la promotion CAR PIECE PROMUE pas codee
    if (!aAllDescriptions) {

        eTypePiece lPiece;
        int lOldPosX;
        int lOldPosY;
        int lNewPosX;
        int lNewPosY;
        eColorPiece lColorPiece;

        if (EstUnSimpleMove (lPiece,
                             lOldPosX,
                             lOldPosY,
                             lNewPosX,
                             lNewPosY,
                             lColorPiece)
            || EstUneSimpleAttaque (lPiece,
                                  lOldPosX,
                                  lOldPosY,
                                  lNewPosX,
                                  lNewPosY,
                                  lColorPiece)) {
            if (aActionPlayer)
                aListeActionsDemo.push_back(new ActionDemoPlayer (lPiece,
                                                                  lOldPosX,
                                                                  lOldPosY,
                                                                  lNewPosX,
                                                                  lNewPosY,
                                                                  lPiece));
            else
                aListeActionsDemo.push_back(new ActionDemoAutoMove (lPiece,
                                                                  lOldPosX,
                                                                  lOldPosY,
                                                                  lNewPosX,
                                                                  lNewPosY));
            return;
        }

        eTypePiece lPiecePromotion;
        bool lAttack;
        if (EstUnePromotion (lOldPosX,
                             lOldPosY,
                             lNewPosX,
                             lNewPosY,
                             lPiecePromotion,
                             lAttack,
                             lColorPiece)) {

            if (aActionPlayer)
                aListeActionsDemo.push_back(new ActionDemoPlayer (lPiece,
                                                                  lOldPosX,
                                                                  lOldPosY,
                                                                  lNewPosX,
                                                                  lNewPosY,
                                                                  lPiecePromotion));
            else
                aListeActionsDemo.push_back(new ActionDemoAutoMove (lPiece,
                                                                  lOldPosX,
                                                                  lOldPosY,
                                                                  lNewPosX,
                                                                  lNewPosY));
            return;
        }

        bool lPetitRoque = false;
        if (EstUnRoque(lPetitRoque, lColorPiece, lNewPosX, lNewPosY)) {

            if (aActionPlayer)
                aListeActionsDemo.push_back(new ActionDemoPlayer (King,
                                                                  _ChessManager.getInfosSmallCastling()._PosKingToCastle,
                                                                  lNewPosY,
                                                                  lNewPosX,
                                                                  lNewPosY,
                                                                  King));
            else
               aListeActionsDemo.push_back(new ActionDemoAutoMove (King,
                                                                 _ChessManager.getInfosSmallCastling()._PosKingToCastle,
                                                                 lNewPosY,
                                                                 lNewPosX,
                                                                 lNewPosY));
            return;
        }

        if (EstUnePriseEnPassant (lOldPosX, lOldPosY,
                                  lNewPosX, lNewPosY)) {

            if (aActionPlayer)
                aListeActionsDemo.push_back(new ActionDemoPlayer (Pawn,
                                                                  lOldPosX,
                                                                  lOldPosY,
                                                                  lNewPosX,
                                                                  lNewPosY,
                                                                  Pawn));
            else
                aListeActionsDemo.push_back(new ActionDemoAutoMove (Pawn,
                                                                  lOldPosX,
                                                                  lOldPosY,
                                                                  lNewPosX,
                                                                  lNewPosY));

            return;
        }
    } else {

        for (QVector<IAction*>::const_iterator it = _ListeActions.begin();
             it != _ListeActions.end();
             ++it) {
            if(*it) {

                if (ActionMove* lActionMove = dynamic_cast<ActionMove *>(*it)) {

                    aListeActionsDemo.push_back(new ActionDemoAutoMove (lActionMove->_ArticleFrom._TypePiece,
                                                                        lActionMove->_ArticleFrom._PosX,
                                                                        lActionMove->_ArticleFrom._PosY,
                                                                        lActionMove->_ArticleTo._PosX,
                                                                        lActionMove->_ArticleTo._PosY));
                }

                if (ActionMangerPiece* lActionMangerPiece = dynamic_cast<ActionMangerPiece *>(*it)) {

                    aListeActionsDemo.push_back(new ActionDemoAutoMove (lActionMangerPiece->_ArticleFrom._TypePiece,
                                                                        lActionMangerPiece->_ArticleFrom._PosX,
                                                                        lActionMangerPiece->_ArticleFrom._PosY,
                                                                        lActionMangerPiece->_ArticleTo._PosX,
                                                                        lActionMangerPiece->_ArticleTo._PosY));

                    aListeActionsDemo.push_back(new ActionDemoKill (lActionMangerPiece->_ArticleTo._TypePiece,
                                                                        lActionMangerPiece->_ArticleTo._PosX,
                                                                        lActionMangerPiece->_ArticleTo._PosY));
                }

                if (ActionPromotion* lActionPromote = dynamic_cast<ActionPromotion *>(*it)) {

                    aListeActionsDemo.push_back(new ActionDemoKill (lActionPromote->_ArticleFrom._TypePiece,
                                                                    lActionPromote->_ArticleFrom._PosX,
                                                                    lActionPromote->_ArticleFrom._PosY));
                    aListeActionsDemo.push_back(new ActionDemoAjout (lActionPromote->_GenrePromotion,
                                                                         lActionPromote->getColorPieceInvolved(),
                                                                         lActionPromote->_ArticleFrom._PosX,
                                                                         lActionPromote->_ArticleFrom._PosY));
                }

                if (ActionPriseEnPassant* lActionPriseEnPassant = dynamic_cast<ActionPriseEnPassant *>(*it)) {

                    aListeActionsDemo.push_back(new ActionDemoAutoMove (lActionPriseEnPassant->_ArticleFrom._TypePiece,
                                                                        lActionPriseEnPassant->_ArticleFrom._PosX,
                                                                        lActionPriseEnPassant->_ArticleFrom._PosY,
                                                                        lActionPriseEnPassant->_ArticleTo._PosX,
                                                                        lActionPriseEnPassant->_ArticleTo._PosY));

                    aListeActionsDemo.push_back(new ActionDemoKill (lActionPriseEnPassant->_PawnToDelete._TypePiece,
                                                                        lActionPriseEnPassant->_PawnToDelete._PosX,
                                                                        lActionPriseEnPassant->_PawnToDelete._PosY));
                }


                if (ActionCastling* lActionCastle = dynamic_cast<ActionCastling *>(*it)) {

                    aListeActionsDemo.push_back(new ActionDemoAutoMove (lActionCastle->_King._TypePiece,
                                                                        lActionCastle->_King._PosX,
                                                                        lActionCastle->_King._PosY,
                                                                        lActionCastle->_newPosXKing,
                                                                        lActionCastle->_King._PosY));

                    aListeActionsDemo.push_back(new ActionDemoAutoMove (lActionCastle->_Rook._TypePiece,
                                                                        lActionCastle->_Rook._PosX,
                                                                        lActionCastle->_Rook._PosY,
                                                                        lActionCastle->_newPosXRook,
                                                                        lActionCastle->_Rook._PosY));

                }

            }
        }

    } //else if (!aAllDescriptions)

}
