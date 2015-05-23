#ifndef TABLEPIECE_H
#define TABLEPIECE_H

//#if !defined(NDEBUG)
//#define BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING
//#define BOOST_MULTI_INDEX_ENABLE_SAFE_MODE
//#endif


#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <vector>
#include "ArticlePiece.hpp"
#include <exception>

using boost::multi_index_container;
using namespace boost::multi_index;


/**
 * @brief Cette table va servir à donner l'état à temps t de l'ensemble des pieces sur l'echiquier
 *
 */
class TablePiece
{
public:
    TablePiece();

    /**
     * @brief Pour supprimer un article de la table
     *
     * @param aArticle
     */
    bool SupprimerArticle (const ArticlePiece& aArticle);

    bool SupprimerArticle (int aPosX, int aPosY);

    /**
     * @brief Pour supprimer tous les articles
     *
     */
    void SupprimerArticles ();

    /**
     * @brief Pour creer un article
     *
     * @param aArticle
     * @return int est le numero identifiant de l'article
     */
    int CreerArticle (const ArticlePiece& aArticle);


    /**
     * @brief Pour modifier un article
     *
     * @param aId
     * @param aArticle
     * @return bool
     */
    bool ModifierArticle (int aId, const ArticlePiece& aArticle);


    bool Existe (int aPosX, int aPosY, ArticlePiece &aArticle) const;


    /**
     * @brief Méthode pour avoir l'article correspondant à l'identifiant article
     *
     * @param anum
     * @param aArticle
     * @return bool renvoie true ou false selon que l'article existe ou pas
     */
    bool Get (int anum, ArticlePiece &aArticle) const;


    /**
     * @brief Méthode pour avoir tous les numeros d'articles existants
     *
     * @param aNumerosArticles
     * @return bool renvoie false si pas d'articles
     */
    bool getNumerosArticles (std::vector<int>& aNumerosArticles) const;

    bool getArticlesDeCouleur (eColorPiece aColorPiece, tlisteArticlesPieces& aListeArticles) const;

    bool getArticlesDeCouleurEtType (eColorPiece aColorPiece,
                                     eTypePiece aTypePiece,
                                     tlisteArticlesPieces& aListeArticles,
                                     bool aModeAppend=false) const;

private:
    typedef multi_index_container<
       ArticlePiece,
        indexed_by<//clé unique sur identifiant
                            ordered_unique<
                                BOOST_MULTI_INDEX_MEMBER(ArticlePiece,int,_Id)
                            >,
                            //clé unique sur la position sur l'echiquier (only on piece by coordinate)
                            ordered_unique<
                              composite_key<
                                ArticlePiece,
                                BOOST_MULTI_INDEX_MEMBER(ArticlePiece,int,_PosX),
                                BOOST_MULTI_INDEX_MEMBER(ArticlePiece,int,_PosY)
                              >
                            >,
                            ordered_non_unique<
                              composite_key<
                                ArticlePiece,
   // const_mem_fun<ArticlePiece,int,&ArticlePiece::ColorToInt>,
                               BOOST_MULTI_INDEX_CONST_MEM_FUN(ArticlePiece,int,ColorToInt),
  //  const_mem_fun<ArticlePiece,int,&ArticlePiece::TypePieceToInt>
                               BOOST_MULTI_INDEX_CONST_MEM_FUN(ArticlePiece,int,TypePieceToInt)
                               >
                            >
                         >
      > Type_table;

    Type_table _table;
    typedef Type_table::nth_index<0>::type::const_iterator TypeIteratorIndexById;
    typedef Type_table::nth_index<0>::type TypeIndexByIdType;
    const TypeIndexByIdType& get_IndexById() const {return _table.get<0>();}

    typedef Type_table::nth_index<1>::type::const_iterator TypeIteratorIndexByPos;
    typedef Type_table::nth_index<1>::type TypeIndexByPosType;
    const TypeIndexByPosType& get_IndexByPos() const {return _table.get<1>();}

    typedef Type_table::nth_index<2>::type::const_iterator TypeIteratorIndexByColorAndTypePiece;
    typedef Type_table::nth_index<2>::type TypeIndexByColorAndTypePieceType;
    const TypeIndexByColorAndTypePieceType& get_IndexByColorAndTypePiece() const {return _table.get<2>();}

    bool AjouterArticle (const ArticlePiece& aArticle);

    class exceptionCoherence : public std::exception
    {
        public:
            virtual const char* what() const throw() {return "Probleme de Coherence pour l'article Piece";}
    };

    void verifCoherence (const ArticlePiece& aArticle);
};

#endif // TABLEPIECE_H
