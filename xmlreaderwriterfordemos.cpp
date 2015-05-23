#include "xmlreaderwriterfordemos.h"
#include <sstream>
#include <QFile>
#include <QTextStream>
#include <boost/format.hpp>
#include "descriptordemo.h"
#include "infosgraphiques.h"
#include "Demos/actionsdemo.h"
#include "Demos/IFournisseurDemo.h"

const QChar CDelimiteur = '-';
const QString CKeyAutoMove    = "Auto";
const QString CKeyPlayerMove  = "Player";

XmlReaderWriterForDemos::XmlReaderWriterForDemos(eTypeDemo aTypeDemo, const IFournisseurDemo &aFournisseurDemo):
    _TypeDemo(aTypeDemo),
    _FournisseurDemo(aFournisseurDemo)
{

}


void XmlReaderWriterForDemos::get_MappingTypeDemoMoveListeDemos (tmapDemoMoveDescriptorDemo_vector& aMap,
                                                                const QString& afileName) const {

    aMap.clear();

    QDomDocument doc;

    //********************************
    // Read the DOM tree form file
    //********************************

    QString lFileName = afileName;

    //par defaut, prend ce fichier
    if (lFileName.size() == 0) {

        switch (_TypeDemo){
            case eSimuMove     : {lFileName = ":/xml/XML/Demos"+_FournisseurDemo.get_ExtensionXMLFileName()+".xml"; break;}
            case eTrainingMove : {lFileName = ":/xml/XML/Training"+_FournisseurDemo.get_ExtensionXMLFileName()+".xml"; break;}
        }
     }

    QFile f (lFileName);

    if (!f.open(QIODevice::ReadOnly)) return;
    if (!doc.setContent(&f)) {f.close();return;}
    f.close();

    //********************************
    // Parse the DOM tree
    //********************************

    // The root node is supposed to be a "ToutesDemos" tag
    QDomElement root = doc.documentElement();

    int lTypeDemoMove;

    for(QDomNode nchild = root.firstChild(); !nchild.isNull(); nchild = nchild.nextSibling()) {

        QDomElement child = nchild.toElement();

        if (!_FournisseurDemo.get_IndexDemoForXMLNodeName(child.tagName(), lTypeDemoMove))
            continue;

        //cree une liste de demos pour le genre de la piece
        tDescriptorDemo_vector lListeDemos;
        //parcourt les noeuds fils des DemosPiece
        for(QDomNode nDemo = child.firstChild(); !nDemo.isNull(); nDemo = nDemo.nextSibling()) {

            QDomElement Demo = nDemo.toElement();
            DescriptorDemo* lDemo = new DescriptorDemo ();
            for(QDomNode nChildDemo = Demo.firstChild(); !nChildDemo.isNull(); nChildDemo = nChildDemo.nextSibling()) {

                QDomElement ChildDemo = nChildDemo.toElement();
                if (ChildDemo.tagName() == "White") {

                    for(QDomNode nTypePiece = ChildDemo.firstChild(); !nTypePiece.isNull(); nTypePiece = nTypePiece.nextSibling()) {
                        LireNodePiece (WHITE, nTypePiece.toElement(), lDemo);
                    }

                } else if (ChildDemo.tagName() == "Black") {

                    for(QDomNode nTypePiece = ChildDemo.firstChild(); !nTypePiece.isNull(); nTypePiece = nTypePiece.nextSibling()) {
                        LireNodePiece (BLACK, nTypePiece.toElement(), lDemo);
                    }

                } else if (ChildDemo.tagName() == "Move") {

                    QString lData = ChildDemo.toElement().text();
                    int lPos = 0;

                    while (lPos != -1) {
                        int lPosEndAction = lData.indexOf (CDelimiteur.toAscii(), lPos);
                        if (lPosEndAction != -1) {
                            QString ltext = lData.mid(lPos, lPosEndAction - lPos);
                            if (ltext.indexOf (CKeyAutoMove) != -1)
                                LireActionDemoAutoMove (ltext, lDemo);
                            else if (ltext.indexOf(CKeyPlayerMove) != -1)
                                LireActionDemoPlayer (ltext, lDemo);
                            else if (ltext [0] == 'x')
                                LireActionDemoKill (ltext, lDemo);
                            else if (ltext [0] == '+')
                                LireActionDemoAjout (ltext, lDemo);

                            lPos = lPosEndAction +1;
                        }else
                            lPos = -1;
                    }

                }//if (ChildDemo.tagName() == "Move")
                else if (ChildDemo.tagName() == "Rotation") {
                    lDemo->setUseRotation(true);

                } else if (ChildDemo.tagName() == "Explanation") {
                    lDemo->setExplanation(ChildDemo.toElement().text());
                }

            }
            lListeDemos.push_back(lDemo);

        }//for(QDomNode nDemo...

        aMap [lTypeDemoMove] = lListeDemos;

    }
}

void XmlReaderWriterForDemos::LireNodePiece (eColorPiece aColor, const QDomElement &aElementPiece, DescriptorDemo* aDemo) const {
    int X,Y;
    sscanf (aElementPiece.text().toStdString().c_str(), "%i %i", &X, &Y);
    eTypePiece lGenrePiece;

    if (aElementPiece.tagName() == "Rook") {
        lGenrePiece = Rook;
    } else if (aElementPiece.tagName() == "Knight") {
        lGenrePiece = Knight;
    } else if (aElementPiece.tagName() == "Bishop") {
        lGenrePiece = Bishop;
    } else if (aElementPiece.tagName() == "Queen") {
        lGenrePiece = Queen;
    } else if (aElementPiece.tagName() == "King") {
        lGenrePiece = King;
    } else if (aElementPiece.tagName() == "Pawn") {
        lGenrePiece = Pawn;
    } else return;

    aDemo->AddPiece(ArticlePiece (-1, lGenrePiece, aColor, X, Y, true));
}

void XmlReaderWriterForDemos::WriteDemoFile (const QString& afile,
                                  int aTypeDemoMove,
                                  const tlisteArticlesPieces& aListePiecesWhite,
                                  const tlisteArticlesPieces& aListePiecesBlack,
                                  bool aUseRotation,
                                  const QString& aExplanation,
                                  const QVector<IActionDemo*>& aListeActionsDemos){

    QFile f (afile);

    bool lToutEcrire = false;
    if (!f.open(QIODevice::ReadOnly)) lToutEcrire = true;
    QDomDocument doc;
    if (!doc.setContent(&f)) lToutEcrire = true;
    f.close();

    QDomElement rootNode;

    if (lToutEcrire) {

        QDomImplementation impl = QDomDocument().implementation();
        // document with document type
        QString name = "demos";
        QString publicId = "";
        QString systemId = "";
        doc = QDomDocument (impl.createDocumentType(name,publicId,systemId));

        // add some XML comment at the beginning
        doc.appendChild(doc.createComment("This file describe demos"));
        doc.appendChild(doc.createTextNode("\n")); // for nicer output

        // root node
        rootNode = doc.createElement("ToutesDemos");
        doc.appendChild(rootNode);

    }else
        rootNode = doc.documentElement();

    //sinon, ajouter dans la node de type piece si existe
    QString lNameNodeDemo;

    if (!_FournisseurDemo.get_XMLNodeNameForIndexDemo(aTypeDemoMove, lNameNodeDemo))
        return;


    QDomElement DemoPieceNode;
    if (doc.elementsByTagName (lNameNodeDemo).length () == 0) {
        DemoPieceNode = doc.createElement(lNameNodeDemo);
        rootNode.appendChild(DemoPieceNode);
    }else
      DemoPieceNode = doc.elementsByTagName (lNameNodeDemo).at(0).toElement();


    //ajout d'un noeud Demo pour la nouvelle demo
    QDomElement DemoNode = doc.createElement("Demo");

    AjoutDansNodeTypePieceDemo (doc,
                                DemoNode,
                                aListePiecesWhite, aListePiecesBlack,
                                aUseRotation,
                                aExplanation,
                                aListeActionsDemos);
    DemoPieceNode.appendChild(DemoNode);

    QFile fileOut (afile);
    if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    //ouvre unstream pour ecrire
    QTextStream outStream(&fileOut);
    outStream << doc.toString();
    fileOut.close();

}

void XmlReaderWriterForDemos::EcrireActionDemoAutoMove (const eTypePiece aTypePiece,
                                                    const tListMovesdemo& aMovesDemo,
                                                    QString& aTextPositions) {

    aTextPositions += CKeyAutoMove + QString(" ") + InfosGraphiques::instance()->get_NomPieceSansTr (aTypePiece) + QString(" ");

    for (tListMovesdemo::const_iterator it = aMovesDemo.begin();
         it != aMovesDemo.end();
         ++it) {

        float rt = ((*it)._tempsRelatif)*1.0;

        if (rt == 0.0) {

            //note position depart
            aTextPositions += QString::number ((*it)._posX) + QString(" ") + QString::number ((*it)._posY) + QString(" ");

        }

        if (rt == 1.0) {

            //note position arrivee
            aTextPositions += QString::number ((*it)._posX) + QString(" ") + QString::number ((*it)._posY);

        }
    }
    aTextPositions += QString(CDelimiteur);
}

void XmlReaderWriterForDemos::LireActionDemoAutoMove (const QString& aText,
                                                  DescriptorDemo* aDescriptorDemo) const {


    std::stringstream lstream;
    lstream << aText.toStdString();
    std::string lNomPiece, lKey;
    eTypePiece lTypePiece;
    int lOldPosX, lOldPosY, lnewPosX, lnewPosY;

    lstream >> lKey;
    lstream >> lNomPiece;
    lstream >> lOldPosX;
    lstream >> lOldPosY;
    lstream >> lnewPosX;
    lstream >> lnewPosY;

    if (aDescriptorDemo && InfosGraphiques::instance()->get_GenrePieceSansTr(QString(lNomPiece.c_str()), lTypePiece))
        aDescriptorDemo->AddActionDemo(new ActionDemoAutoMove (lTypePiece, lOldPosX, lOldPosY, lnewPosX, lnewPosY));

}

void XmlReaderWriterForDemos::EcrireActionDemoKill (const eTypePiece aTypePiece,
                                                    int aPosX,
                                                    int aPosY,
                                                    QString& aTextPositions) {

    aTextPositions += QString("x ") + InfosGraphiques::instance()->get_NomPieceSansTr (aTypePiece) + QString(" ")
                        + QString::number(aPosX) + QString(" ") + QString::number(aPosY) + QString(" ");
    aTextPositions += QString(CDelimiteur);
}

void XmlReaderWriterForDemos::LireActionDemoKill (const QString &aText,
                                                  DescriptorDemo* aDescriptorDemo) const {

    std::stringstream lstream;
    lstream << aText.toStdString();
    std::string lNomPiece;
    eTypePiece lTypePiece;
    int lPosX, lPosY;

    lstream >> lNomPiece;//pour le "x "
    lstream >> lNomPiece;
    lstream >> lPosX;
    lstream >> lPosY;
    if (aDescriptorDemo && InfosGraphiques::instance()->get_GenrePieceSansTr(QString(lNomPiece.c_str()), lTypePiece))
        aDescriptorDemo->AddActionDemo(new ActionDemoKill (lTypePiece, lPosX, lPosY));
}

void XmlReaderWriterForDemos::EcrireActionDemoAjout (const eTypePiece aTypePiece,
                                                     eColorPiece aColorPiece,
                                                     int aPosX,
                                                     int aPosY,
                                                     QString& aTextPositions) {

    aTextPositions += QString("+ ") + QString::number (int(aColorPiece)) + QString(" ")
                        + InfosGraphiques::instance()->get_NomPieceSansTr (aTypePiece) + QString(" ")
                        + QString::number(aPosX) + QString(" ") + QString::number(aPosY) + QString(" ");
    aTextPositions += QString(CDelimiteur);
}

void XmlReaderWriterForDemos::LireActionDemoAjout (const QString& aText,
                                                   DescriptorDemo* aDescriptorDemo) const {

    std::stringstream lstream;
    lstream << aText.toStdString();
    std::string lNomPiece;
    eTypePiece lTypePiece;
    int lnumColor;
    eColorPiece lColorPiece;
    int lPosX, lPosY;

    lstream >> lNomPiece;//pour le "+ "
    lstream >> lnumColor;
    lColorPiece = eColorPiece (lnumColor);
    lstream >> lNomPiece;
    lstream >> lPosX;
    lstream >> lPosY;
    if (aDescriptorDemo && InfosGraphiques::instance()->get_GenrePieceSansTr(QString(lNomPiece.c_str()), lTypePiece))
        aDescriptorDemo->AddActionDemo(new ActionDemoAjout (lTypePiece, lColorPiece, lPosX, lPosY));
}

void XmlReaderWriterForDemos::EcrireActionDemoPlayer (const eTypePiece aTypePiece,
                                                      int aOldPosX, int aOldPosY,
                                                      int anewPosX, int anewPosY,
                                                      const eTypePiece aTypePieceFinale,
                                                      QString& aTextPositions) const {

    aTextPositions += CKeyPlayerMove + QString(" ") + InfosGraphiques::instance()->get_NomPieceSansTr (aTypePiece);

    //note position depart
    aTextPositions += QString(" ") + QString::number (aOldPosX) + QString(" ") + QString::number (aOldPosY);

    //note position arrivee
    aTextPositions += QString(" ") + QString::number (anewPosX) + QString(" ") + QString::number (anewPosY);

    if (aTypePieceFinale != aTypePiece)
      aTextPositions += QString(" ") + InfosGraphiques::instance()->get_NomPieceSansTr (aTypePieceFinale);

    aTextPositions += QString(CDelimiteur);
}

void XmlReaderWriterForDemos::LireActionDemoPlayer (const QString& aText,
                                                   DescriptorDemo* aDescriptorDemo) const {

    eTypePiece lTypePiece, lTypePieceFinale;
    int lOldPosX, lOldPosY, lnewPosX, lnewPosY;


    QStringList lWords = aText.split(" ");
    if (lWords.count() < 6) return;

    QString lKey  = lWords [0];
    if (lKey != CKeyPlayerMove) return;

    QString lNomPiece = lWords [1];
    QString lNomPieceFinale = lNomPiece;

    QString lStrOldPosX  = lWords [2];
    bool  lOk;
    lOldPosX = lStrOldPosX.toInt(&lOk);
    if (!lOk) return;

    QString lStrOldPosY  = lWords [3];
    lOldPosY = lStrOldPosY.toInt(&lOk);
    if (!lOk) return;

    QString lStrNewPosX  = lWords [4];
    lnewPosX = lStrNewPosX.toInt(&lOk);
    if (!lOk) return;

    QString lStrNewPosY  = lWords [5];
    lnewPosY = lStrNewPosY.toInt(&lOk);
    if (!lOk) return;


    if (lWords.count() >= 7) {
        lNomPieceFinale  = lWords [6];
    }

    if (aDescriptorDemo && InfosGraphiques::instance()->get_GenrePieceSansTr(lNomPiece, lTypePiece)) {

        lTypePieceFinale = lTypePiece;
        if (lNomPieceFinale != lNomPiece)
            InfosGraphiques::instance()->get_GenrePieceSansTr(lNomPieceFinale, lTypePieceFinale);

        aDescriptorDemo->AddActionDemo(new ActionDemoPlayer (lTypePiece, lOldPosX, lOldPosY, lnewPosX, lnewPosY, lTypePieceFinale));
    }

}

void XmlReaderWriterForDemos::AjoutDansNodeTypePieceDemo (QDomDocument& aDoc, QDomElement& aDemoPieceNode,
                                                          const tlisteArticlesPieces& aListePiecesWhite,
                                                          const tlisteArticlesPieces& aListePiecesBlack,
                                                          bool aUseRotation,
                                                          const QString& aExplanation,
                                                          const QVector<IActionDemo*>& aListeActionsDemos) {

    //ecrit les nodes des pieces
    EcrireNodesPieces (aDoc, aDemoPieceNode, WHITE, aListePiecesWhite);
    EcrireNodesPieces (aDoc, aDemoPieceNode, BLACK, aListePiecesBlack);

    //si rotation demandee, cree simplement le node sans autre information
    if (aUseRotation) {
       QDomElement RotationNode = aDoc.createElement("Rotation");
       aDemoPieceNode.appendChild(RotationNode);
    }

    //ecrire un node pour l'explication
    if (aExplanation.size()> 0) {
        QDomElement ExplanationNode = aDoc.createElement("Explanation");
        ExplanationNode.appendChild(aDoc.createTextNode(aExplanation));
        aDemoPieceNode.appendChild(ExplanationNode);
    }

    //ecrit le node des moves
    tListMovesdemo lMovesDemo;
    eTypePiece lTypePiece, lTypePieceFinale;
    int lOldPosX, lOldPosY, lPosX, lPosY;
    eColorPiece lColorPiece;

    QString lTextPositions = "";
    for (QVector<IActionDemo*>::const_iterator it = aListeActionsDemos.begin();
         it != aListeActionsDemos.end();
         ++it) {
        IActionDemo* lActionDemo = (*it);
        if (lActionDemo->isActionDemoAutoMove(lTypePiece, lMovesDemo))
            EcrireActionDemoAutoMove (lTypePiece, lMovesDemo, lTextPositions);
        else if (lActionDemo->isActionDemoKill(lTypePiece, lPosX, lPosY))
            EcrireActionDemoKill (lTypePiece, lPosX, lPosY, lTextPositions);
        else if (lActionDemo->isActionDemoAjout(lTypePiece, lColorPiece, lPosX, lPosY))
            EcrireActionDemoAjout (lTypePiece, lColorPiece, lPosX, lPosY, lTextPositions);
        else if (lActionDemo->isActionDemoPlayer(lTypePiece,lOldPosX, lOldPosY, lPosX, lPosY, lTypePieceFinale))
            EcrireActionDemoPlayer (lTypePiece, lOldPosX, lOldPosY, lPosX, lPosY, lTypePieceFinale, lTextPositions);
        else continue;

    }

    if (lTextPositions.size()>0) {
        QDomElement MoveNode = aDoc.createElement("Move");
        MoveNode.appendChild(aDoc.createTextNode(lTextPositions));
        aDemoPieceNode.appendChild(MoveNode);
    }
}

void XmlReaderWriterForDemos::EcrireNodesPieces (QDomDocument& adoc,
                                                 QDomElement& aParentNode,
                                           eColorPiece aColor,
                                           const tlisteArticlesPieces& aListePieces) {

    QDomElement ColorNode;
    if (aListePieces.size() != 0) {
        if (aColor == WHITE)
          ColorNode = adoc.createElement("White");
        else
          ColorNode = adoc.createElement("Black");
        aParentNode.appendChild(ColorNode);
    }
    for (tlisteArticlesPieces::const_iterator it = aListePieces.begin();
         it != aListePieces.end();
         ++it) {

        QString lNameTypePiece;
        switch ((*it)._TypePiece) {
            case Rook   : {lNameTypePiece = QString("Rook");break;}
            case Knight : {lNameTypePiece = QString("Knight");break;}
            case Bishop : {lNameTypePiece = QString("Bishop");break;}
            case Queen  : {lNameTypePiece = QString("Queen");break;}
            case King   : {lNameTypePiece = QString("King");break;}
            case Pawn   : {lNameTypePiece = QString("Pawn");break;}
            default     : continue;break;
        }
        QDomElement TypePieceNode = adoc.createElement(lNameTypePiece);
        std::ostringstream lstream;
        lstream.clear();
        lstream.str("");

        lstream << (*it)._PosX;
        lstream << " ";
        lstream << (*it)._PosY;

        QString lTextPositions = QString(lstream.str().c_str());

        TypePieceNode.appendChild(adoc.createTextNode(lTextPositions));
        ColorNode.appendChild(TypePieceNode);

    }
}
