#include "RuleMoveBishop.h"

RuleMoveBishop::RuleMoveBishop() : IRuleMovePiece()
{
}

bool RuleMoveBishop::_DoMovement (const IChessManager &aIChessManager,
                                  const ArticlePiece& aArticleFrom,
                                  int aNewPosX,
                                  int aNewPosY,
                                  CommandPlay* aCommandPlay,
                                  const CommandPlay* const,
                                  bool,
                                  bool aDoNotCheckFriendFinalSquare,
                                  bool ARayonX) const{

    return _ActionSurDeplacementDiag (aIChessManager,
                                      aArticleFrom,
                                      aNewPosX,
                                      aNewPosY,
                                      aCommandPlay,
                                      aDoNotCheckFriendFinalSquare,
                                      ARayonX);
}
