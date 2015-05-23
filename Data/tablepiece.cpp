#include "tablepiece.h"
#include "TypeInfoChessBoard.hpp"

#ifdef _DEBUG
#include <QDebug>
#endif

TablePiece::TablePiece()
{
}


bool TablePiece::Existe (int aPosX, int aPosY, ArticlePiece &aArticle) const {

   TypeIteratorIndexByPos it = get_IndexByPos().find (boost::make_tuple(aPosX,
                                                                        aPosY));
   bool lArticlePresent = (it != get_IndexByPos().end());
   if (lArticlePresent) aArticle = *it;
   return lArticlePresent;

}

bool TablePiece::Get (int anum, ArticlePiece &aArticle) const {

   TypeIteratorIndexById it = get_IndexById().find (anum);
   bool lArticlePresent = (it != get_IndexById().end());
   if (lArticlePresent) aArticle = *it;
   return lArticlePresent;

}

bool TablePiece::getNumerosArticles (std::vector<int>& aNumerosArticles) const {
    aNumerosArticles.clear();
    for (TypeIteratorIndexById it = get_IndexById().begin(); it != get_IndexById().end(); ++it) {
        aNumerosArticles.push_back ((*it)._Id);
    }
    return aNumerosArticles.size();
}

bool TablePiece::getArticlesDeCouleur (eColorPiece aColorPiece, tlisteArticlesPieces& aListeArticles) const {

    aListeArticles.clear();
    aListeArticles.reserve(16);

    std::pair<TypeIteratorIndexByColorAndTypePiece,TypeIteratorIndexByColorAndTypePiece> p;
    p = get_IndexByColorAndTypePiece().equal_range(boost::make_tuple(ArticlePiece::ColorToInt (aColorPiece)));

    while (p.first != p.second)
    {
        aListeArticles.push_back(*p.first);
        ++(p.first);
    }
    return (aListeArticles.size() > 0);

}



bool TablePiece::getArticlesDeCouleurEtType (eColorPiece aColorPiece,
                                             eTypePiece aTypePiece,
                                             tlisteArticlesPieces& aListeArticles,
                                             bool aModeAppend) const {

    if (!aModeAppend)
        aListeArticles.clear();

    int lColorPiece = ArticlePiece::ColorToInt (aColorPiece);
    int lTypePiece  = ArticlePiece::TypePieceToInt (aTypePiece);
    std::pair<TypeIteratorIndexByColorAndTypePiece,TypeIteratorIndexByColorAndTypePiece> p;
    p = get_IndexByColorAndTypePiece().equal_range(boost::make_tuple(lColorPiece, lTypePiece));

    while (p.first != p.second)
    {
        aListeArticles.push_back(*p.first);
        ++(p.first);
    }
    return (aListeArticles.size() > 0);
}

void TablePiece::verifCoherence (const ArticlePiece& aArticle) {

    ArticlePiece lArticle;
    //si une piece existe deja à la position, c'est une erreur
    if ( (aArticle._PosX > constants::GLOBAL_CONST_DIMENSION) ||  (aArticle._PosY > constants::GLOBAL_CONST_DIMENSION) )
        throw (exceptionCoherence ());
    if (Existe (aArticle._PosX, aArticle._PosY, lArticle))
        throw (exceptionCoherence ());
 }


bool TablePiece::AjouterArticle (const ArticlePiece& aArticle) {

    try{
        verifCoherence (aArticle);
         std::pair<Type_table::iterator,bool> p=_table.insert(aArticle);
         return p.second;
    }catch (const exceptionCoherence &e) {

#ifdef _DEBUG
    qDebug()<<e.what();
#endif
        return false;

    } catch(...) {
        return false;
    }
}

bool TablePiece::SupprimerArticle (const ArticlePiece& aArticle) {

    TypeIteratorIndexById it = get_IndexById().find (aArticle._Id);
    if (it != get_IndexById().end())
        _table.erase(it);
    return true;

}

bool TablePiece::SupprimerArticle (int aPosX, int aPosY) {

    ArticlePiece lArticle;
    if (!Existe (aPosX, aPosY, lArticle)) return false;

    SupprimerArticle (lArticle);
    return true;
}

void TablePiece::SupprimerArticles () {

    _table.erase (get_IndexById().begin(), get_IndexById().end());
//    for (TypeIteratorIndexById it = get_IndexById().begin();
//         it != get_IndexById().end();
//         ++it) {
//        _table.erase(it);
//    }
//    std::vector<int> lNumeros;
//    getNumerosArticles (lNumeros);
//    for (std::vector<int>::const_iterator it = lNumeros.begin();
//         it != lNumeros.end();
//         ++it) {
//        TypeIteratorIndexById itId = get_IndexById().find (*it);
//        _table.erase(itId);
//    }

}

int TablePiece::CreerArticle (const ArticlePiece& aArticle) {

    ArticlePiece lArticle = aArticle;
    lArticle._Id = 0;
    if (_table.size() > 0) {
        TypeIteratorIndexById it = get_IndexById().end();//prend le ID le plus eleve
        it--;
        lArticle._Id = (*it)._Id+1;//  _table.size();ne pas confondre numero d'article et identifiant!
    }
    if (AjouterArticle (lArticle)) {
        return lArticle._Id;
    }
    else
      return -1;
}

bool TablePiece::ModifierArticle (int aId, const ArticlePiece& aArticle) {

    TypeIteratorIndexById it = get_IndexById().find(aId);
    bool lArticlePresent = (it != get_IndexById().end());
    if (!lArticlePresent) return false;
    ArticlePiece lArticle = aArticle;
    lArticle._Id = aId;//modifie pas l'ID
    bool lOk = _table.get<0>().replace(it, lArticle);
    return lOk; // update
}

