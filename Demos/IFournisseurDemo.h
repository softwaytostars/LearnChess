#ifndef IFOURNISSEURDEMO_H
#define IFOURNISSEURDEMO_H

#include <boost/bimap.hpp>
#include "Data/ArticlePiece.hpp"
#include "TypeDemo.hpp"

class IFournisseurDemo
{
public:
    IFournisseurDemo();
    virtual ~IFournisseurDemo();

    /**
     * @brief Pour accès à la liste dans la frame initiation
     *
     * @return QVector<QString>
     */
    const QVector<QString>& get_ListeItemNames() const {return _ListeNames;}

    /**
     * @brief Pour un petit descriptif titre lorsque un element de la liste est selectionné
     *
     * @param aIndexDemo
     * @return QString
     */
    virtual QString get_SmallDescription(int aIndexDemo) const=0;

    /**
     * @brief Pour un message descriptif lorsque bouton info de element de la liste cliqué
     *
     * @param aIndexDemo
     * @return QString
     */
    virtual QString get_DetailedDescription(int aIndexDemo) const=0;

    /**
     * @brief Ajoute une piece dans la frame de description (lorsque bouton info de element de la liste cliqué) avec ses directions possibles
     *
     * @param int
     * @param
     * @return bool
     */
    virtual bool setStaticPieceAndShowItsPossiblePath (int , ArticlePiece& ) const {return false;}

    /**
     * @brief Pour Mettre des pieces dans la frame de description (lorsque bouton info de element de la liste cliqué)
     *
     * @param
     * @return bool
     */
    virtual bool setStaticPieces (const tlisteArticlesPieces&) const {return false;}

    virtual QString get_ExtensionXMLFileName () const =0;

    virtual eTypeInitiation get_TypeInitiation() const =0;

    bool get_IndexDemoForXMLNodeName (const QString& aXMLNodeName, int& aIndexDemo) const;
    bool get_XMLNodeNameForIndexDemo (int aIndexDemo, QString& aXMLNodeName) const;

    bool get_IndexDemoForTypeDemoName (const QString& aTypeDemoName, int& aIndexDemo) const;
    bool get_TypeDemoNameForIndexDemo (int aIndexDemo, QString& aTypeDemoName) const;

    /**
     * @brief fait ici ce qui dépends des traductions
     *
     */
    void translate();

    //artifice pour pas s'embeter à récupérer la row correspondant à l'item selectionné
    int _SelectedRow; /**< correspond à la ligne selectionnée de la liste pour selectionner demo */
protected:
    void AssociateIndexDemoAndXMLNodeName (int aIndex, const QString& aXMLNodeName);
    void AssociateIndexDemoAndTypeDemoName (int aIndex, const QString& aTypeDemoName);
    virtual void AjouterTypeDemoNames ()=0;
private:
    typedef boost::bimap <int, QString> bimap_IndexDemotoXMLNameNodeName;
    bimap_IndexDemotoXMLNameNodeName _mapIndexDemotoXMLNameNodeName;
    typedef boost::bimap <int, QString> bimap_IndexDemotoTypeDemoName;
    bimap_IndexDemotoTypeDemoName _mapIndexDemotoTypeDemoName;
    QVector<QString> _ListeNames;
};

#endif // IFOURNISSEURDEMO_H
