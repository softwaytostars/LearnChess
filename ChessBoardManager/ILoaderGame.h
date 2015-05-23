#ifndef ILOADERGAME_H
#define ILOADERGAME_H

class CommandPlay;
class IChessManager;

class ILoaderGame {

public:
    //ajoute ici car creation depends de IChessManager& et non const IChessManager&, or on veut personne d'autre que le loader ne puisse modifier IChessManager
    virtual void InitializeGame(bool aCreationPiece) = 0;
    virtual void AjouterDemiCoup (CommandPlay* aCommandPlay) = 0;
    virtual IChessManager& getChessManager() = 0;

};

#endif // ILOADERGAME_H
