#ifndef PAINTERPATHMOVES_H
#define PAINTERPATHMOVES_H

#include <QPointF>
#include "TypePiece.hpp"

class QPainter;

class PainterPathMoves
{
public:

/**
 * @brief
 *
 * @param aPainter
 * @param aPos est la positon sur la scene de la piece
 * @param aTypePiece
 * @param aColorPiece
 * @param aCellSize
 */
    PainterPathMoves(QPainter& aPainter,
                     const QPointF& aPos,
                     eTypePiece aTypePiece,
                     eColorPiece aColorPiece,
                     float aCellSize);
    void DrawPaths ();
private:
    enum eDirectionArrow {
        eArrowLeft,
        eArrowRight,
        eArrowTop,
        eArrowBottom,
        eArrowTopLeft,
        eArrowTopRight,
        eArrowBottomLeft,
        eArrowBottomRight
    };

    enum eDirection {
        eDiag,
        eHoriz,
        eVertic,
        eDiag1,
        eDiag2
    };

    QPainter& _Painter;
    QPointF _Pos; /**< Position sur la scene de la piece */
    eTypePiece _TypePiece;
    eColorPiece _ColorPiece;
    float _CellSize;
    void addArrow (eDirectionArrow aDirArrow, QPointF aPos);
    void createLineDoubleArrow(eDirection aDir, QPointF p1, QPointF p2);
    void DessineFlechePawn (QPointF aCenterPawn, int aNbreCases);
};

#endif // PAINTERPATHMOVES_H
