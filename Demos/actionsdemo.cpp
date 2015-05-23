#include "actionsdemo.h"
#include <QtGlobal>

ActionDemoAutoMove::ActionDemoAutoMove(eTypePiece aGenrePiece,
                               int aOldPosX, int aOldPosY,
                               int anewPosX, int anewPosY):
    IActionDemo(),
    _TypePiece (aGenrePiece)
{
    sMoveDemo lMoveDemo;
    //enregistre le depart
    lMoveDemo._tempsRelatif = 0.0;
    lMoveDemo._posX         = aOldPosX;
    lMoveDemo._posY         = aOldPosY;
    _ListeMovesDemo.push_back(lMoveDemo);

    //seul le cavalier aura son mouvement décomposé
    if (aGenrePiece == Knight) {

        lMoveDemo._tempsRelatif = 0.5;
        int ldiffX = qAbs<int> (aOldPosX-anewPosX);
        int ldiffY = qAbs<int> (aOldPosY-anewPosY);
        if (ldiffX > ldiffY) {
            lMoveDemo._posX         = anewPosX;
            lMoveDemo._posY         = aOldPosY;
        }else {
            lMoveDemo._posX         = aOldPosX;
            lMoveDemo._posY         = anewPosY;
        }
        _ListeMovesDemo.push_back(lMoveDemo);
    }

    lMoveDemo._tempsRelatif = 1.0;
    lMoveDemo._posX         = anewPosX;
    lMoveDemo._posY         = anewPosY;
    _ListeMovesDemo.push_back(lMoveDemo);
}

/*******ActionDemoKill*******/

ActionDemoKill::ActionDemoKill(eTypePiece aTypePiece,
                               int aPosX, int aPosY) :
    IActionDemo(),
    _TypePiece (aTypePiece),
    _posX (aPosX), _posY (aPosY)
{
}

/*******ActionDemoAjout*******/

ActionDemoAjout::ActionDemoAjout(eTypePiece aTypePiece,
                                 eColorPiece aColorPiece,
                                 int aPosX, int aPosY):
    IActionDemo(),
    _TypePiece (aTypePiece),
    _ColorPiece (aColorPiece),
    _posX (aPosX), _posY (aPosY)
{

}

/*******ActionDemoPlayer*******/

ActionDemoPlayer::ActionDemoPlayer(eTypePiece aGenrePiece,
               int aOldPosX, int aOldPosY,
               int anewPosX, int anewPosY,
               eTypePiece aGenrePieceFinale) :
    IActionDemo(),
    _TypePiece (aGenrePiece),
    _TypePieceFinale(aGenrePieceFinale),
    _OldPosX (aOldPosX), _OldPosY (aOldPosY),
    _newPosX (anewPosX), _newPosY (anewPosY)
{

}
