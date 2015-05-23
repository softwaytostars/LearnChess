#ifndef ACTIONSDEMO_H
#define ACTIONSDEMO_H

#include "TypePiece.hpp"
#include "TypeDemo.hpp"

class IActionDemo {
public:
    virtual bool isActionDemoAutoMove (eTypePiece&, tListMovesdemo& ) const {return false;}
    virtual bool isActionDemoKill (eTypePiece&, int&, int&) const {return false;}
    virtual bool isActionDemoAjout (eTypePiece&, eColorPiece&, int&, int&) const {return false;}
    virtual bool isActionDemoPlayer(eTypePiece&, int&, int&, int&, int&, eTypePiece&) const {return false;}
};

class ActionDemoAutoMove : public IActionDemo
{
public:
    ActionDemoAutoMove(eTypePiece aGenrePiece,
                   int aOldPosX, int aOldPosY,
                   int anewPosX, int anewPosY);
    virtual bool isActionDemoAutoMove (eTypePiece& aTypePiece, tListMovesdemo& alisteMovesDemo) const {

        alisteMovesDemo.clear();
        aTypePiece      = _TypePiece;
        alisteMovesDemo = _ListeMovesDemo;
        return true;
    }
private:
    eTypePiece _TypePiece;
    tListMovesdemo _ListeMovesDemo;
};

class ActionDemoKill : public IActionDemo
{
public:
    ActionDemoKill(eTypePiece aTypePiece,
                   int aPosX, int aPosY);
    virtual bool isActionDemoKill (eTypePiece& aTypePiece, int& aPosX, int& aPosY ) const {
        aTypePiece = _TypePiece;
        aPosX      = _posX;
        aPosY      = _posY;
        return true;
    }
private:
    eTypePiece _TypePiece;
    int _posX, _posY;
};

class ActionDemoAjout : public IActionDemo
{
public:
    ActionDemoAjout(eTypePiece aTypePiece,
                    eColorPiece aColorPiece,
                    int aPosX, int aPosY);
    virtual bool isActionDemoAjout (eTypePiece& aTypePiece, eColorPiece& aColorPiece, int&aPosX, int&aPosY) const {
        aTypePiece  = _TypePiece;
        aColorPiece = _ColorPiece;
        aPosX       = _posX;
        aPosY       = _posY;
        return true;
    }
private:
    eTypePiece _TypePiece;
    eColorPiece _ColorPiece;
    int _posX, _posY;
};

class ActionDemoPlayer : public IActionDemo
{
public:
    ActionDemoPlayer(eTypePiece aGenrePiece,
                   int aOldPosX, int aOldPosY,
                   int anewPosX, int anewPosY,
                     eTypePiece aGenrePieceFinale);
    virtual bool isActionDemoPlayer (eTypePiece& aTypePiece,
                                     int &aOldPosX, int &aOldPosY,
                                     int &anewPosX, int &anewPosY,
                                     eTypePiece& aTypePieceFinale) const {

        aTypePiece   = _TypePiece;
        aTypePieceFinale = _TypePieceFinale;
        aOldPosX     = _OldPosX;
        aOldPosY     = _OldPosY;
        anewPosX     = _newPosX;
        anewPosY     = _newPosY;
        return true;
    }
private:
    eTypePiece _TypePiece, _TypePieceFinale;
    int _OldPosX, _OldPosY, _newPosX, _newPosY;
};

#endif // ACTIONSDEMO_H
