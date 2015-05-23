#ifndef INFOSGRAPHIQUES_H
#define INFOSGRAPHIQUES_H

#include <QPixmap>
#include "TypePiece.hpp"
#include "TypeDemo.hpp"
#include "Types/TypePreferences.h"
#include <boost/bimap.hpp>

class QListWidget;
class IUserPiecesGraphiques;
class IUserTraductions;

/**
 * @brief Classe singleton pour les infos graphiques
 *
 */

class InfosGraphiques
{
public:
    static InfosGraphiques* instance ();
    static void kill ();
    bool get_GraphicPiece (eColorPiece aColorPiece,
                           eTypePiece aGenrePiece,
                           QPixmap& aGraphicPiece,
                           qreal aScaleSquare=0) const;
    QString get_NomPiece (eTypePiece aGenrePiece) const;
    bool get_GenrePiece(const QString& aNomPiece, eTypePiece &aGenrePiece) const;
    QString get_NomPieceSansTr(eTypePiece aGenrePiece) const;
    bool get_GenrePieceSansTr (const QString& aNomPieceSansTr, eTypePiece& aGenrePiece) const;
    void AjouterImagePieceDansListe (eTypePiece aTypePiece, eColorPiece aColor, QListWidget *aListWidget) const;
    bool get_TypePieceDeNotation (const char aNotation, eTypePiece &aGenrePiece) const;
    char get_NotationDePiece (eTypePiece aGenrePiece) const;

    /**
     * @brief Préférences des types de pièces à utiliser
     *
     * @return const tMapPrefPieceName
     */
    const tMapPrefPieceName& get_MapPrefTypePiecesName () const { return _MapPrefPiecesName;}

    /**
     * @brief Préférences des styles de couleours pour appli
     *
     * @return const tMapPrefStyleAppName
     */
    const tMapPrefStyleAppName& get_MapPrefStyleAppName () const { return _MapPrefStyleAppName;}

    void get_ColorsBackground (QColor& aColorBlack, QColor& aColorWhite) const;

    void AbonnerUtilisateurPiecesGraphiques (IUserPiecesGraphiques * const aUser);
    void DesabonnerUtilisateurPiecesGraphiques (IUserPiecesGraphiques *const aUser);
    void RechargerPiecesGraphiquesDesAffichages() const;

    void AbonnerUtilisateurTraductions (IUserTraductions * const aUser);
    void DesabonnerUtilisateurTraductions(IUserTraductions * const aUser);
    void RechargerTraductions();

private:
    InfosGraphiques();
    static InfosGraphiques* _instance;
    typedef boost::bimap < eTypePiece, QString > bimap_TypePieceString;
    bimap_TypePieceString _mapTypePieceTraduction, _mapTypePieceNoTraduction;
    typedef boost::bimap < eTypePiece, char > bimap_TypePieceChar;
    bimap_TypePieceChar _mapTypePieceNotation;
    bool get_GraphicPieceFromSpriteType1 (eColorPiece aColorPiece, eTypePiece aGenrePiece, QPixmap& aGraphicPiece) const;
    bool get_GraphicPieceFromKidsPieces(eColorPiece aColorPiece, eTypePiece aGenrePiece, QPixmap& aGraphicPiece) const;
    bool get_GraphicPieceFromGabrielPieces (eColorPiece aColorPiece, eTypePiece aGenrePiece, QPixmap& aGraphicPiece) const;
    bool get_GraphicPieceFromSpriteEnsifer (eColorPiece aColorPiece, eTypePiece aGenrePiece, QPixmap& aGraphicPiece) const;
    bool get_GraphicPieceFromSpriteType2 (eColorPiece aColorPiece, eTypePiece aGenrePiece, QPixmap& aGraphicPiece) const;
    tMapPrefPieceName      _MapPrefPiecesName;
    tMapPrefStyleAppName   _MapPrefStyleAppName;
    QList <IUserPiecesGraphiques *> _ListUsersPiecesGraphiques;
    QList <IUserTraductions *> _ListUsersTraductions;
    void translate();
};

#endif // INFOSGRAPHIQUES_H
