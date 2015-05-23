#include "infosgraphiques.h"
#include <QListWidgetItem>
#include <QListWidget>
#include "infosconfiguration.h"
#include "ClientGraphique/IUserPiecesGraphiques.h"
#include "ClientGraphique/IUserTraductions.h"

// Initialisation du singleton à 0
InfosGraphiques* InfosGraphiques::_instance = 0;

InfosGraphiques* InfosGraphiques::instance () {
    if (!_instance) {
        _instance   = new InfosGraphiques();
    }
    return _instance;
}

void InfosGraphiques::kill () {
    if (_instance) {
        delete _instance;
        _instance = 0;
    }
}

InfosGraphiques::InfosGraphiques() {

    //ce qui depends des traductions
    translate();

    //stocke notations seulement pour pieces autres que pion (car pion sans notation)
    _mapTypePieceNotation.insert (bimap_TypePieceChar::value_type (King,   'K'));
    _mapTypePieceNotation.insert (bimap_TypePieceChar::value_type (Queen,  'Q'));
    _mapTypePieceNotation.insert (bimap_TypePieceChar::value_type (Rook,   'R'));
    _mapTypePieceNotation.insert (bimap_TypePieceChar::value_type (Knight, 'N'));
    _mapTypePieceNotation.insert (bimap_TypePieceChar::value_type (Bishop, 'B'));

    //nom des preferences pieces
    _MapPrefPiecesName [ePiecesMerida]      = "&Merida";
    _MapPrefPiecesName [ePiecesBerlin]      = "&Berlin";
    _MapPrefPiecesName [ePiecesKid]         = "&Kids";
    _MapPrefPiecesName [ePiecesGabriel]     = "&Gabriel";
    _MapPrefPiecesName [ePiecesEnsifer]     = "&Ensifer";
    _MapPrefPiecesName [ePiecesAustria]     = "&Austria";

    //nom des preferences style
    _MapPrefStyleAppName [eStyleDefault]    = "&Merise";
    _MapPrefStyleAppName [eStyleKid]        = "&Kid";

}

void InfosGraphiques::translate() {

    _mapTypePieceTraduction.clear();
    _mapTypePieceTraduction.insert( bimap_TypePieceString::value_type(Rook,   QObject::tr ("Rook") ) );
    _mapTypePieceTraduction.insert( bimap_TypePieceString::value_type(Knight, QObject::tr ("Knight") ) );
    _mapTypePieceTraduction.insert( bimap_TypePieceString::value_type(Bishop, QObject::tr ("Bishop") ) );
    _mapTypePieceTraduction.insert( bimap_TypePieceString::value_type(Queen,  QObject::tr ("Queen") ) );
    _mapTypePieceTraduction.insert( bimap_TypePieceString::value_type(King,   QObject::tr ("King") ) );
    _mapTypePieceTraduction.insert( bimap_TypePieceString::value_type(Pawn,   QObject::tr ("Pawn") ) );

    _mapTypePieceNoTraduction.clear();
    _mapTypePieceNoTraduction.insert( bimap_TypePieceString::value_type(Rook,   "Rook"   ) );
    _mapTypePieceNoTraduction.insert( bimap_TypePieceString::value_type(Knight, "Knight" ) );
    _mapTypePieceNoTraduction.insert( bimap_TypePieceString::value_type(Bishop, "Bishop" ) );
    _mapTypePieceNoTraduction.insert( bimap_TypePieceString::value_type(Queen,  "Queen"  ) );
    _mapTypePieceNoTraduction.insert( bimap_TypePieceString::value_type(King,   "King"   ) );
    _mapTypePieceNoTraduction.insert( bimap_TypePieceString::value_type(Pawn,   "Pawn"   ) );
}

bool InfosGraphiques::get_GraphicPiece (eColorPiece aColorPiece,
                                        eTypePiece aGenrePiece,
                                        QPixmap& aGraphicPiece,
                                        qreal aScaleSquare) const {

    bool lGraphicsOk = false;
    switch (InfosConfiguration::instance()->getLastTypePieces()) {

        case ePiecesMerida   :
        case ePiecesBerlin   : {lGraphicsOk = get_GraphicPieceFromSpriteType1 (aColorPiece, aGenrePiece, aGraphicPiece);break;}
        case ePiecesKid      : {lGraphicsOk = get_GraphicPieceFromKidsPieces  (aColorPiece, aGenrePiece, aGraphicPiece);break;}
        case ePiecesGabriel  : {lGraphicsOk = get_GraphicPieceFromGabrielPieces  (aColorPiece, aGenrePiece, aGraphicPiece);break;}
        case ePiecesEnsifer  : {lGraphicsOk = get_GraphicPieceFromSpriteEnsifer (aColorPiece, aGenrePiece, aGraphicPiece);break;}
        case ePiecesAustria  : {lGraphicsOk = get_GraphicPieceFromSpriteType2  (aColorPiece, aGenrePiece, aGraphicPiece);break;}

        default : return false;
    }

    if (lGraphicsOk && (aScaleSquare > 0)) {
        aGraphicPiece = aGraphicPiece.scaled(QSize (aScaleSquare, aScaleSquare),
                                             Qt::IgnoreAspectRatio,
                                             Qt::SmoothTransformation);
    }

    return (!aGraphicPiece.isNull());

}

bool InfosGraphiques::get_GraphicPieceFromSpriteType1 (eColorPiece aColorPiece, eTypePiece aGenrePiece, QPixmap& aGraphicPiece) const {

    //par defaut, met la big picture à Merida
    QPixmap lBigPicture;
    switch (InfosConfiguration::instance()->getLastTypePieces()) {

        case ePiecesMerida : {lBigPicture.load(QString (":/ressources/merida.png"));
                              break;
                             }
        case ePiecesBerlin : {lBigPicture.load(QString (":/ressources/berlin.png"));
                                break;
                               }
        default : break;

    }


    if (lBigPicture.isNull()) return false;

    int realsize = lBigPicture.height() / 2;

    switch (aGenrePiece) {

        case Rook   : if (aColorPiece == WHITE) {aGraphicPiece = lBigPicture.copy(0 * realsize, 0, realsize, realsize);}
                                       else {aGraphicPiece = lBigPicture.copy(0 * realsize, realsize, realsize, realsize);};
                    break;
        case Knight : if (aColorPiece == WHITE) {aGraphicPiece = lBigPicture.copy(1 * realsize, 0, realsize, realsize);}
                                       else {aGraphicPiece = lBigPicture.copy(1 * realsize, realsize, realsize, realsize);};
                    break;
        case Bishop : if (aColorPiece == WHITE) {aGraphicPiece = lBigPicture.copy(2 * realsize, 0, realsize, realsize);}
                                       else {aGraphicPiece = lBigPicture.copy(2 * realsize, realsize, realsize, realsize);};
                    break;
        case Queen : if (aColorPiece == WHITE) {aGraphicPiece = lBigPicture.copy(3 * realsize, 0, realsize, realsize);}
                                       else {aGraphicPiece = lBigPicture.copy(3 * realsize, realsize, realsize, realsize);};
                    break;
        case King : if (aColorPiece == WHITE) {aGraphicPiece = lBigPicture.copy(4 * realsize, 0, realsize, realsize);}
                                       else {aGraphicPiece = lBigPicture.copy(4 * realsize, realsize, realsize, realsize);};
                    break;
        case Pawn : if (aColorPiece == WHITE) {aGraphicPiece = lBigPicture.copy(5 * realsize, 0, realsize, realsize);}
                                       else {aGraphicPiece = lBigPicture.copy(5 * realsize, realsize, realsize, realsize);};
                    break;
        default : return false;

    }
    return true;
}//get_GraphicPieceFromSpriteType1

bool InfosGraphiques::get_GraphicPieceFromKidsPieces(eColorPiece aColorPiece, eTypePiece aGenrePiece, QPixmap& aGraphicPiece) const {

    switch (aGenrePiece) {

        case Rook : {if (aColorPiece == WHITE) {if (!aGraphicPiece.load(QString (":/KidsPieces/ImagesPieces/Rook/white-rook2.png"))) return false;}
                     else if (!aGraphicPiece.load(QString (":/KidsPieces/ImagesPieces/Rook/black-rook2.png"))) return false;
                      break;
                    }
        case Knight : { QPixmap lPictureKnight;
                       if ((aColorPiece == WHITE) && (!lPictureKnight.load(QString (":/KidsPieces/ImagesPieces/Knight/angryknights_white.png"))))
                           return false;
                       if ((aColorPiece == BLACK) && (!lPictureKnight.load(QString (":/KidsPieces/ImagesPieces/Knight/angryknights_black.png"))))
                           return false;

                        int realsizeWidth  = lPictureKnight.width();
                        int realsizeheight = lPictureKnight.height();
                        aGraphicPiece = lPictureKnight.copy(0,
                                                            0,
                                                            realsizeWidth,
                                                            realsizeheight);
                        break;
                      }
        case Bishop : { if (aColorPiece == WHITE) {if (!aGraphicPiece.load(QString (":/KidsPieces/ImagesPieces/Bishop/bishop_thinkingwhite.png"))) return false;}
                        else if (!aGraphicPiece.load(QString (":/KidsPieces/ImagesPieces/Bishop/black-bishop.png"))) return false;
                        break;
                       }
        case Queen : { if (aColorPiece == WHITE) {if (!aGraphicPiece.load(QString (":/KidsPieces/ImagesPieces/Queen/queenfightingwhite.png"))) return false;}
                        else if (!aGraphicPiece.load(QString (":/KidsPieces/ImagesPieces/Queen/black-queen2.png"))) return false;
                        break;
                      }
        case King : { if (aColorPiece == WHITE) {if (!aGraphicPiece.load(QString (":/KidsPieces/ImagesPieces/King/white-king2.png"))) return false;}
                       else if (!aGraphicPiece.load(QString (":/KidsPieces/ImagesPieces/King/black-king2.png"))) return false;
                       break;
                     }
        case Pawn : { if (aColorPiece == WHITE) {if (!aGraphicPiece.load(QString (":/KidsPieces/ImagesPieces/Pawn/white-pawn2.png"))) return false;}
                       else if (!aGraphicPiece.load(QString (":/KidsPieces/ImagesPieces/Pawn/pawn-black2.png"))) return false;
                       break;
                     }


    }
    return true;
}//get_GraphicPieceFromKidsPieces

bool InfosGraphiques::get_GraphicPieceFromGabrielPieces (eColorPiece aColorPiece, eTypePiece aGenrePiece, QPixmap& aGraphicPiece) const {

    switch (aGenrePiece) {

        case Rook : {if (aColorPiece == WHITE) {if (!aGraphicPiece.load(QString (":/Gabriel/Gabriel/Tour_blanc_1.png"))) return false;}
                     else if (!aGraphicPiece.load(QString (":/Gabriel/Gabriel/Tour_noire_1.png"))) return false;
                      break;
                    }
        case Knight : {if (aColorPiece == WHITE) {if (!aGraphicPiece.load(QString (":/Gabriel/Gabriel/Cavalier_blanc.png"))) return false;}
                       else if (!aGraphicPiece.load(QString (":/Gabriel/Gabriel/Cavalier_noir.png"))) return false;
                        break;
                      }
        case Bishop : { if (aColorPiece == WHITE) {if (!aGraphicPiece.load(QString (":/Gabriel/Gabriel/Fou_blanc.png"))) return false;}
                        else if (!aGraphicPiece.load(QString (":/Gabriel/Gabriel/Fou_noir.png"))) return false;
                        break;
                       }
        case Queen : { if (aColorPiece == WHITE) {if (!aGraphicPiece.load(QString (":/Gabriel/Gabriel/Dame_blanche.png"))) return false;}
                        else if (!aGraphicPiece.load(QString (":/Gabriel/Gabriel/Dame_noire.png"))) return false;
                        break;
                      }
        case King : { if (aColorPiece == WHITE) {if (!aGraphicPiece.load(QString (":/Gabriel/Gabriel/Roi_blanc.png"))) return false;}
                       else if (!aGraphicPiece.load(QString (":/Gabriel/Gabriel/Roi_noir.png"))) return false;
                       break;
                     }
        case Pawn : { if (aColorPiece == WHITE) {if (!aGraphicPiece.load(QString (":/Gabriel/Gabriel/Pion_blanc.png"))) return false;}
                       else if (!aGraphicPiece.load(QString (":/Gabriel/Gabriel/Pion_noir.png"))) return false;
                       break;
                     }


    }
    return true;

}

bool InfosGraphiques::get_GraphicPieceFromSpriteEnsifer (eColorPiece aColorPiece, eTypePiece aGenrePiece, QPixmap& aGraphicPiece) const {

    //par defaut, met la big picture achetee en licence etendue
   // QPixmap lBigPicture (QString(":/KidsPieces/ImagesPieces/vectorstock_546011/ensifer.png"), "PNG");
   // QPixmap lBigPicture ("D:/LearnChess/ImagesPieces/vectorstock_546011/vectorstock_546011/vectorstock_546011.jpg", "JPG");


   // if (lBigPicture.isNull()) return false;

//    float lwidth  = lBigPicture.width() /4;
//    float lheight = lBigPicture.height()/3;

    switch (aGenrePiece) {

        case Rook   : if (aColorPiece == WHITE) {aGraphicPiece.load(":/KidsPieces/ImagesPieces/vectorstock_546011/ensifer_Rook_White.png");}//aGraphicPiece = lBigPicture.copy(3 * lwidth, 2*lheight, lwidth, lheight);}
                                       else {aGraphicPiece.load(":/KidsPieces/ImagesPieces/vectorstock_546011/ensifer_Rook_Black.png");};
                    break;
        case Knight : if (aColorPiece == WHITE) {aGraphicPiece.load(":/KidsPieces/ImagesPieces/vectorstock_546011/ensifer_Knight_White.png");}//aGraphicPiece = lBigPicture.copy(2 * lwidth, 2*lheight, lwidth, lheight);}
                                       else {aGraphicPiece.load(":/KidsPieces/ImagesPieces/vectorstock_546011/ensifer_Knight_Black.png");};
                    break;
        case Bishop : if (aColorPiece == WHITE) {aGraphicPiece.load(":/KidsPieces/ImagesPieces/vectorstock_546011/ensifer_Bishop_White.png");}
                                       else {aGraphicPiece.load(":/KidsPieces/ImagesPieces/vectorstock_546011/ensifer_Bishop_Black.png");};
                    break;
        case Queen : if (aColorPiece == WHITE) {aGraphicPiece.load(":/KidsPieces/ImagesPieces/vectorstock_546011/ensifer_Queen_White.png");}
                                       else {aGraphicPiece.load(":/KidsPieces/ImagesPieces/vectorstock_546011/ensifer_Queen_Black.png");};
                    break;
        case King : if (aColorPiece == WHITE) {aGraphicPiece.load(":/KidsPieces/ImagesPieces/vectorstock_546011/ensifer_King_White.png");}
                                       else {aGraphicPiece.load(":/KidsPieces/ImagesPieces/vectorstock_546011/ensifer_King_Black.png");};
                    break;
        case Pawn : if (aColorPiece == WHITE) {aGraphicPiece.load(":/KidsPieces/ImagesPieces/vectorstock_546011/ensifer_Pawn_White.png");}
                                       else {aGraphicPiece.load(":/KidsPieces/ImagesPieces/vectorstock_546011/ensifer_Pawn_Black.png");};
                    break;
        default : return false;

    }
    return true;
}//get_GraphicPieceFromSpriteEnsifer

bool InfosGraphiques::get_GraphicPieceFromSpriteType2 (eColorPiece aColorPiece, eTypePiece aGenrePiece, QPixmap& aGraphicPiece) const {

    QPixmap lBigPictureBlack, lBigPictureWhite;

    if (!lBigPictureBlack.load(QString (":/ressources/ImagesPieces/Austria/Austria_Black.png"))) return false;
    if (!lBigPictureWhite.load(QString (":/ressources/ImagesPieces/Austria/Austria_White.png"))) return false;

    int lPosYRef = 0;

    if (lBigPictureBlack.isNull()) return false;
    if (lBigPictureWhite.isNull()) return false;

    int realsizeWidth  = lBigPictureBlack.width () / 6;


    switch (aGenrePiece) {

        case Rook   : if (aColorPiece == BLACK) {aGraphicPiece = lBigPictureBlack.copy(0 * realsizeWidth, lPosYRef*lBigPictureBlack.height(), realsizeWidth, lBigPictureBlack.height());}
                                       else {aGraphicPiece = lBigPictureWhite.copy(0 * realsizeWidth, lPosYRef*lBigPictureWhite.height(), realsizeWidth, lBigPictureWhite.height());};
                     break;
        case Knight : if (aColorPiece == BLACK) {aGraphicPiece = lBigPictureBlack.copy(4 * realsizeWidth, lPosYRef*lBigPictureBlack.height(), realsizeWidth, lBigPictureBlack.height());}
                                            else {aGraphicPiece = lBigPictureWhite.copy(4 * realsizeWidth, lPosYRef*lBigPictureWhite.height(), realsizeWidth, lBigPictureWhite.height());};
                     break;
        case Bishop : if (aColorPiece == BLACK) {aGraphicPiece = lBigPictureBlack.copy(1 * realsizeWidth, lPosYRef*lBigPictureBlack.height(), realsizeWidth, lBigPictureBlack.height());}
                                            else {aGraphicPiece = lBigPictureWhite.copy(1 * realsizeWidth, lPosYRef*lBigPictureWhite.height(), realsizeWidth, lBigPictureWhite.height());};
                    break;
        case Queen : if (aColorPiece == BLACK) {aGraphicPiece = lBigPictureBlack.copy(2 * realsizeWidth, lPosYRef*lBigPictureBlack.height(), realsizeWidth, lBigPictureBlack.height());}
                                            else {aGraphicPiece = lBigPictureWhite.copy(2 * realsizeWidth, lPosYRef*lBigPictureWhite.height(), realsizeWidth, lBigPictureWhite.height());};
                    break;
        case King : if (aColorPiece == BLACK) {aGraphicPiece = lBigPictureBlack.copy(3 * realsizeWidth, lPosYRef*lBigPictureBlack.height(), realsizeWidth, lBigPictureBlack.height());}
                                            else {aGraphicPiece = lBigPictureWhite.copy(3 * realsizeWidth, lPosYRef*lBigPictureWhite.height(), realsizeWidth, lBigPictureWhite.height());};
                    break;
        case Pawn : if (aColorPiece == BLACK) {aGraphicPiece = lBigPictureBlack.copy(5 * realsizeWidth, lPosYRef*lBigPictureBlack.height(), realsizeWidth, lBigPictureBlack.height());}
                                            else {aGraphicPiece = lBigPictureWhite.copy(5 * realsizeWidth, lPosYRef*lBigPictureWhite.height(), realsizeWidth, lBigPictureWhite.height());};
                    break;
        default : return false;

    }
    return true;

}//get_GraphicPieceFromSpriteType2

QString InfosGraphiques::get_NomPiece(eTypePiece aGenrePiece) const {

    bimap_TypePieceString::left_map::const_iterator it = _mapTypePieceTraduction.left.find(aGenrePiece);

    if (it != _mapTypePieceTraduction.left.end()) {
        return (*it).second;
    }else
        return "";
}

QString InfosGraphiques::get_NomPieceSansTr(eTypePiece aGenrePiece) const {

    bimap_TypePieceString::left_map::const_iterator it = _mapTypePieceNoTraduction.left.find(aGenrePiece);

    if (it != _mapTypePieceNoTraduction.left.end()) {
        return (*it).second;
    }else
        return "";
}

bool InfosGraphiques::get_GenrePiece (const QString& aNomPiece, eTypePiece& aGenrePiece) const {

    bimap_TypePieceString::right_map::const_iterator it = _mapTypePieceTraduction.right.find(aNomPiece);
    if (it != _mapTypePieceTraduction.right.end()) {
        aGenrePiece = (*it).second;
        return true;
    }else {
        return false;
    }

}

bool InfosGraphiques::get_GenrePieceSansTr (const QString& aNomPieceSansTr, eTypePiece& aGenrePiece) const {

    bimap_TypePieceString::right_map::const_iterator it = _mapTypePieceNoTraduction.right.find(aNomPieceSansTr);
    if (it != _mapTypePieceNoTraduction.right.end()) {
        aGenrePiece = (*it).second;
        return true;
    }else {
        return false;
    }

}

void InfosGraphiques::AjouterImagePieceDansListe (eTypePiece aTypePiece, eColorPiece aColor, QListWidget *aListWidget) const {
    QPixmap lImage;
    get_GraphicPiece (aColor, aTypePiece, lImage);
    new QListWidgetItem(QIcon(lImage), get_NomPiece (aTypePiece), aListWidget);
}

bool InfosGraphiques::get_TypePieceDeNotation (const char aNotation, eTypePiece &aGenrePiece) const {

    bimap_TypePieceChar::right_map::const_iterator it = _mapTypePieceNotation.right.find(aNotation);
    if (it != _mapTypePieceNotation.right.end()) {
        aGenrePiece = (*it).second;
        return true;
    }else {
        aGenrePiece = Pawn;
        return false;
    }
}

char InfosGraphiques::get_NotationDePiece (eTypePiece aGenrePiece) const {

    bimap_TypePieceChar::left_map::const_iterator it = _mapTypePieceNotation.left.find(aGenrePiece);
    if (it != _mapTypePieceNotation.left.end()) {
        return (*it).second;
    }else {
        return ' ';
    }
}

void InfosGraphiques::get_ColorsBackground (QColor& aColorBlack, QColor& aColorWhite) const {

    if (InfosConfiguration::instance()->getLastTypePieces() == (int)ePiecesGabriel)
        aColorBlack = QColor ("#DF7401");
    else
        aColorBlack = QColor ("#B45F04");//QColor (136, 66, 29);

    aColorWhite = QColor(Qt::lightGray).lighter (130);

}

void InfosGraphiques::AbonnerUtilisateurPiecesGraphiques (IUserPiecesGraphiques *const aUser) {

    _ListUsersPiecesGraphiques.push_back(aUser);

}

void InfosGraphiques::DesabonnerUtilisateurPiecesGraphiques (IUserPiecesGraphiques *const aUser) {

    _ListUsersPiecesGraphiques.removeOne(aUser);
}

void InfosGraphiques::RechargerPiecesGraphiquesDesAffichages() const {

    for (QList<IUserPiecesGraphiques*>::const_iterator it = _ListUsersPiecesGraphiques.begin();
         it != _ListUsersPiecesGraphiques.end();
         ++it) {
        (*it)->RechargerGraphiquesPieces ();
    }
}

void InfosGraphiques::AbonnerUtilisateurTraductions (IUserTraductions * const aUser) {

    _ListUsersTraductions.push_back(aUser);
}

void InfosGraphiques::DesabonnerUtilisateurTraductions(IUserTraductions * const aUser) {

    _ListUsersTraductions.removeOne(aUser);
}

void InfosGraphiques::RechargerTraductions() {

    this->translate();

    for (QList<IUserTraductions*>::const_iterator it = _ListUsersTraductions.begin();
         it != _ListUsersTraductions.end();
         ++it) {
        (*it)->Translate ();
    }
}


