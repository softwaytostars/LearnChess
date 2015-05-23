#include "FactoryRuleMovePiece.h"
#include "ChessBoardManager/RuleMoveRook.h"
#include "ChessBoardManager/RuleMoveKnight.h"
#include "ChessBoardManager/RuleMoveBishop.h"
#include "ChessBoardManager/RuleMoveQueen.h"
#include "ChessBoardManager/RuleMoveKing.h"
#include "ChessBoardManager/RuleMovePawn.h"

int FactoryRuleMovePiece::_NbReference   = 0;
RuleMoveRook *FactoryRuleMovePiece::_RuleMoveRook = 0;
RuleMoveKnight  *FactoryRuleMovePiece::_RuleMoveKnight = 0;
RuleMoveBishop  *FactoryRuleMovePiece::_RuleMoveBishop = 0;
RuleMoveQueen   *FactoryRuleMovePiece::_RuleMoveQueen  = 0;
RuleMoveKing    *FactoryRuleMovePiece::_RuleMoveKing   = 0;
RuleMovePawn    *FactoryRuleMovePiece::_RuleMovePawn   = 0;

void FactoryRuleMovePiece::Attach ()
{
    if (_NbReference==0){
        _RuleMoveRook   = new RuleMoveRook;
        _RuleMoveKnight = new RuleMoveKnight;
        _RuleMoveBishop = new RuleMoveBishop;
        _RuleMoveQueen  = new RuleMoveQueen;
        _RuleMoveKing   = new RuleMoveKing;
        _RuleMovePawn   = new RuleMovePawn;
    }
    _NbReference += 1;
}

void FactoryRuleMovePiece::Detach ()
{
    _NbReference -= 1;
    if (_NbReference == 0){
        delete _RuleMoveRook;
        _RuleMoveRook   = 0;
        delete _RuleMoveKnight;
        _RuleMoveKnight = 0;
        delete _RuleMoveBishop;
        _RuleMoveBishop = 0;
        delete _RuleMoveQueen;
        _RuleMoveQueen  = 0;
        delete _RuleMoveKing;
        _RuleMoveKing   = 0;
        delete _RuleMovePawn;
        _RuleMovePawn = 0;
    }//if (_NbReference = 0)
}

IRuleMovePiece* FactoryRuleMovePiece::GetRulePiece (eTypePiece aGenrePiece)
{
     switch (aGenrePiece){
         case Rook     : return _RuleMoveRook;
         case Knight   : return _RuleMoveKnight;
         case Bishop   : return _RuleMoveBishop;
         case Queen    : return _RuleMoveQueen;
         case King     : return _RuleMoveKing;
         case Pawn     : return _RuleMovePawn;
         default : return 0;
     }
}

