#ifndef XmlReaderWriterForDemos_H
#define XmlReaderWriterForDemos_H

#include <QtXml/QDomDocument>
#include <vector>
#include <map>
#include "TypePiece.hpp"
#include "Data/ArticlePiece.hpp"
#include "TypeDemo.hpp"

class DescriptorDemo;
class IActionDemo;
class IFournisseurDemo;

class XmlReaderWriterForDemos
{
public:
    XmlReaderWriterForDemos(eTypeDemo aTypeDemo, const IFournisseurDemo& aFournisseurDemo);
    typedef std::vector<DescriptorDemo*> tDescriptorDemo_vector;
    typedef std::map<int, tDescriptorDemo_vector> tmapDemoMoveDescriptorDemo_vector;
    void get_MappingTypeDemoMoveListeDemos (tmapDemoMoveDescriptorDemo_vector &aMap, const QString &afileName="") const;
    void WriteDemoFile (const QString &afile,
                        int aTypeDemoMove,
                        const tlisteArticlesPieces& aListePiecesWhite,
                        const tlisteArticlesPieces& aListePiecesBlack,
                        bool aUseRotation,
                        const QString& aExplanation,
                        const QVector<IActionDemo*>& aListeActionsDemos = QVector<IActionDemo*> ());
private:
    void LireNodePiece (eColorPiece aColor, const QDomElement& aElementPiece, DescriptorDemo* aDemo) const;
    void EcrireNodesPieces (QDomDocument& adoc, QDomElement& aParentNode,
                            eColorPiece aColor,
                            const tlisteArticlesPieces& aListePieces);

    void AjoutDansNodeTypePieceDemo (QDomDocument& aDoc, QDomElement& aDemoPieceNode,
                                     const tlisteArticlesPieces& aListePiecesWhite,
                                     const tlisteArticlesPieces& aListePiecesBlack,
                                     bool aUseRotation,
                                     const QString &aExplanation, const QVector<IActionDemo *> &aListeActionsDemos);

    void EcrireActionDemoAutoMove (const eTypePiece aTypePiece,
                               const tListMovesdemo& aMovesDemo,
                               QString& aTextPositions);
    void LireActionDemoAutoMove (const QString& aText,
                             DescriptorDemo* aDescriptorDemo) const;

    void EcrireActionDemoKill (const eTypePiece aTypePiece,
                               int aPosX,
                               int aPosY,
                               QString& aTextPositions);
    void LireActionDemoKill (const QString& aText,
                             DescriptorDemo* aDescriptorDemo) const;

    void EcrireActionDemoAjout (const eTypePiece aTypePiece,
                                eColorPiece aColorPiece,
                                int aPosX,
                                int aPosY,
                                QString& aTextPositions);
    void LireActionDemoAjout (const QString& aText,
                               DescriptorDemo* aDescriptorDemo) const;

    void EcrireActionDemoPlayer (const eTypePiece aTypePiece,
                                 int aOldPosX, int aOldPosY,
                                 int anewPosX, int anewPosY,
                                 const eTypePiece aTypePieceFinale,
                                 QString &aTextPositions) const;
    void LireActionDemoPlayer (const QString& aText,
                               DescriptorDemo* aDescriptorDemo) const;
    eTypeDemo _TypeDemo;
    const IFournisseurDemo& _FournisseurDemo;
};

#endif // XmlReaderWriterForDemos_H
