#ifndef FACTORYRULEMOVEPIECE_H
#define FACTORYRULEMOVEPIECE_H

#include "TypePiece.hpp"

class IRuleMovePiece;
class RuleMoveRook;
class RuleMoveKnight;
class RuleMoveBishop;
class RuleMoveQueen;
class RuleMoveKing;
class RuleMovePawn;

/**
 * @brief Classe permettant de gerer creation/acces/supression des objets contenant regles de mvts des pieces
 *
 */
//comptage par reference car plusieurs echiquiers pourront posseder les meme objets RuleMovePiece
class FactoryRuleMovePiece
{
public:
    static void Attach ();
    static void Detach ();
    static IRuleMovePiece* GetRulePiece (eTypePiece aGenrePiece);
private:
    static int _NbReference;
    static RuleMoveRook   * _RuleMoveRook;
    static RuleMoveKnight *_RuleMoveKnight;
    static RuleMoveBishop *_RuleMoveBishop;
    static RuleMoveQueen  *_RuleMoveQueen;
    static RuleMoveKing   *_RuleMoveKing;
    static RuleMovePawn   *_RuleMovePawn;
    FactoryRuleMovePiece();
};

#endif // FACTORYRULEMOVEPIECE_H
