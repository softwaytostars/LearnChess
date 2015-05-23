#ifndef DESCRIPTORDEMO_H
#define DESCRIPTORDEMO_H
#include <vector>
#include <QVector>
#include <QPointF>
#include "Data/ArticlePiece.hpp"

class IActionDemo;

/**
 * @brief Classe décrivant une démo : mouvements d'une pièce avec un certain nombre de pièces présentes
 *
 */
class DescriptorDemo
{
public:
    DescriptorDemo();
    ~DescriptorDemo();

    inline const QVector<IActionDemo*>& get_ListeActionDemo () const {return _ListeActionDemo;}
    inline const tlisteArticlesPieces& get_ListePieces () const {return _ListesPieces;}

    /**
     * @brief Ajoute une action de demo
     *
     * @param aRelativeTime
     * @param aPoint
     */
    inline void AddActionDemo (IActionDemo* aActionDemo) {
        _ListeActionDemo.push_back(aActionDemo);
    }
    inline void AddPiece (const ArticlePiece& aArticlePiece) {
        _ListesPieces.push_back (aArticlePiece);
    }
    typedef std::vector<DescriptorDemo*> tDescriptorDemo_vector;
    inline bool UseRotation() const {return _UseRotation;}
    inline void setUseRotation (bool aUseRotation) {_UseRotation = aUseRotation;}

    inline QString get_Explanation() const {return _Explanation;}
    inline void setExplanation (const QString& aExplanation) {_Explanation = aExplanation;}
private:
    QVector<IActionDemo*> _ListeActionDemo;
    tlisteArticlesPieces _ListesPieces;
    bool _UseRotation;
    QString _Explanation;
};

#endif // DESCRIPTORDEMO_H
