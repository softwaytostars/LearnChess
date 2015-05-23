#include "painterpathmoves.h"

#include <QPainter>
#include <QPointF>

#include <QtCore/qmath.h>

const int CArrowSize = 20;
const qreal PI = qAcos (-1);

PainterPathMoves::PainterPathMoves(QPainter& aPainter,
                                   const QPointF& aPos,
                                   eTypePiece aTypePiece,
                                   eColorPiece aColorPiece,
                                   float aCellSize):
    _Painter (aPainter),
    _Pos (aPos),
    _TypePiece (aTypePiece),
    _ColorPiece (aColorPiece),
    _CellSize (aCellSize)

{
}

void PainterPathMoves::DrawPaths () {

    QPen lPenOld = _Painter.pen();
    QPen lPen = lPenOld;
    lPen.setWidth(3);
    lPen.setColor(Qt::black);
    _Painter.setPen(lPen);

    QVector<eDirection> lDirectionsForPiece;
    int lNbreMaxPasAllowed = 0;

    switch (_TypePiece) {

        case Rook : {
                      lDirectionsForPiece.push_back(eVertic);
                      lDirectionsForPiece.push_back(eHoriz);
                      lNbreMaxPasAllowed = 8;
                      break;
                    }

        case Bishop : {
                  lDirectionsForPiece.push_back(eDiag);
                  lNbreMaxPasAllowed = 8;
                  break;
                }

        case Queen : {
              lDirectionsForPiece.push_back(eDiag);
              lDirectionsForPiece.push_back(eHoriz);
              lDirectionsForPiece.push_back(eVertic);
              lNbreMaxPasAllowed = 8;
              break;
            }

        case King : {
              lDirectionsForPiece.push_back(eDiag);
              lDirectionsForPiece.push_back(eHoriz);
              lDirectionsForPiece.push_back(eVertic);
              lNbreMaxPasAllowed = 1;
              break;
            }

        //cas particulier : une seule direction (pas en arriere)
        case Pawn : {

            QPointF lCenterPawn (_Pos.x() + _CellSize/2,
                                 _Pos.y() + _CellSize/2);
            DessineFlechePawn (lCenterPawn, 1);
            lPen.setColor(Qt::red);
            _Painter.setPen(lPen);
            //dessine une autre fleche à coté
            if (_Pos.y() == (6*_CellSize))
                DessineFlechePawn (lCenterPawn+QPointF(2, 0), 2);
            break;
        }

        //autre cas particuliers
        case Knight : {

            //dessine un demi-T
            QPointF lCenter (_Pos.x() + _CellSize/2,
                             _Pos.y() + _CellSize/2);
            QPointF lPoint = lCenter;

            QPainterPath path, path2, path3, path4;
            path.moveTo (lPoint);
            lPoint = lPoint - QPointF(0, 2*_CellSize);
            path.lineTo (lPoint);

            lPoint = lPoint + QPointF(_CellSize, 0);
            path.lineTo (lPoint);
            _Painter.drawPath(path);
            addArrow(eArrowRight, lPoint);
            path.lineTo (lPoint - 2*QPointF(_CellSize, 0));
            _Painter.drawPath(path);
            addArrow(eArrowLeft, lPoint - 2*QPointF(_CellSize, 0));

            ///
            path2.moveTo (lCenter);
            lPoint = lCenter - 2*QPointF(_CellSize, 0);
            path2.lineTo (lPoint);

            lPoint = lPoint + QPointF(0, _CellSize);
            path2.lineTo (lPoint);
            _Painter.drawPath(path2);
            addArrow(eArrowBottom, lPoint);
            lPoint = lPoint - 2*QPointF(0, _CellSize);
            path2.lineTo (lPoint);
            _Painter.drawPath(path2);
            addArrow(eArrowTop, lPoint);

            ///
            path3.moveTo (lCenter);
            lPoint = lCenter + 2*QPointF(0, _CellSize);
            path3.lineTo (lPoint);

            lPoint = lPoint + QPointF(_CellSize, 0);
            path3.lineTo (lPoint);
            _Painter.drawPath(path3);
            addArrow(eArrowRight, lPoint);
            lPoint = lPoint - 2*QPointF(_CellSize, 0);
            path3.lineTo (lPoint);
            _Painter.drawPath(path3);
            addArrow(eArrowLeft, lPoint);

            ///
            path4.moveTo (lCenter);
            lPoint = lCenter + 2*QPointF(_CellSize, 0);
            path4.lineTo (lPoint);

            lPoint = lPoint + QPointF(0, _CellSize);
            path4.lineTo (lPoint);
            _Painter.drawPath(path4);
            addArrow(eArrowBottom, lPoint);
            lPoint = lPoint - 2*QPointF(0, _CellSize);
            path4.lineTo (lPoint);
            _Painter.drawPath(path4);
            addArrow(eArrowTop, lPoint);

            break;
        }
        default : return;
    }//switch

    for (QVector<eDirection>::const_iterator it = lDirectionsForPiece.begin();
         it != lDirectionsForPiece.end();
         ++it) {

        if (*it == eHoriz) {

            QPointF lPointX1, lPointX2;
            lPointX1 = QPointF(qMax<qreal>((_CellSize/2), _Pos.x()+_CellSize/2-lNbreMaxPasAllowed*_CellSize),
                               _Pos.y()+(_CellSize/2));
            lPointX2 = QPointF(qMin<qreal>(8*_CellSize - (_CellSize/2), _Pos.x()+_CellSize/2+lNbreMaxPasAllowed*_CellSize),
                               _Pos.y()+(_CellSize/2));

            createLineDoubleArrow (eHoriz, lPointX1, lPointX2);

        }//if (*it == eHoriz)

        if (*it == eVertic) {

            QPointF lPointY1, lPointY2;
            lPointY1 = QPointF(_Pos.x()+(_CellSize/2),
                                qMax<qreal>((_CellSize/2), _Pos.y()+(_CellSize/2)-lNbreMaxPasAllowed*_CellSize));
            lPointY2 = QPointF(_Pos.x()+(_CellSize)/2,
                               qMin<qreal>(8*_CellSize - (_CellSize/2), _Pos.y()+(_CellSize/2)+lNbreMaxPasAllowed*_CellSize));

            createLineDoubleArrow (eVertic, lPointY1, lPointY2);

        }//if (*it == eVertic)

        if (*it == eDiag) {

            //cherche coin top-left
            int lNbreMaxPas = 0;
            while ( (lNbreMaxPas < lNbreMaxPasAllowed) && ((_Pos.x() - (lNbreMaxPas)*_CellSize) >=0)
                    && ((_Pos.y() - (lNbreMaxPas)*_CellSize) >=0))
               lNbreMaxPas++;

            QPointF lPointTopLeft = QPointF(qMax<qreal> (_Pos.x() +(_CellSize/2) - lNbreMaxPas*_CellSize, (_CellSize/2)),
                                            qMax<qreal> (_Pos.y() +(_CellSize/2) - (qMax<int>(1, (lNbreMaxPas-1)))*_CellSize, (_CellSize/2)));
            //cherche coin right-bottom
            lNbreMaxPas = 0;
            while ( (lNbreMaxPas < lNbreMaxPasAllowed) && ((_Pos.x() + (lNbreMaxPas+1)*_CellSize) <=8*_CellSize)
                    && ((_Pos.y() + (lNbreMaxPas+1)*_CellSize) <= 8*_CellSize))
               lNbreMaxPas++;

            QPointF lPointBottomRight = QPointF(qMin<qreal>(_Pos.x() +(_CellSize/2)+ (qMax<int>(1, (lNbreMaxPas-1)))*_CellSize, 8*_CellSize-(_CellSize/2)),
                                                qMin<qreal>(_Pos.y() +(_CellSize/2)+ lNbreMaxPas*_CellSize, 8*_CellSize-(_CellSize/2)));

            createLineDoubleArrow (eDiag1, lPointTopLeft, lPointBottomRight);

            //cherche coin bottom-left
            lNbreMaxPas = 0;
            while ( (lNbreMaxPas < lNbreMaxPasAllowed) && ((_Pos.x() - (lNbreMaxPas+1)*_CellSize) >=0)
                    && ((_Pos.y() + (lNbreMaxPas+2)*_CellSize) >=0))
               lNbreMaxPas++;

            QPointF lPointBottomLeft = QPointF(qMax<qreal> (_Pos.x() +(_CellSize/2) - lNbreMaxPas*_CellSize, (_CellSize/2)),
                                               qMin<qreal> (_Pos.y() +(_CellSize/2) + lNbreMaxPas*_CellSize, 8*_CellSize-(_CellSize/2)));
            //cherche coin right-top
            lNbreMaxPas = 0;
            while ( (lNbreMaxPas < lNbreMaxPasAllowed) && ((_Pos.x() + (lNbreMaxPas+2)*_CellSize) <=8*_CellSize)
                    && ((_Pos.y() - (lNbreMaxPas+1)*_CellSize) <= 8*_CellSize))
               lNbreMaxPas++;

            QPointF lPointTopRight = QPointF(qMin<qreal>(_Pos.x() +(_CellSize/2) + lNbreMaxPas*_CellSize, 8*_CellSize-(_CellSize/2)),
                                             qMax<qreal>(_Pos.y() +(_CellSize/2) - lNbreMaxPas*_CellSize, (_CellSize/2)));

            createLineDoubleArrow (eDiag2, lPointBottomLeft, lPointTopRight);

        }//if (*it == eDiag)

    }//for (QVector<eDirection>...


    _Painter.setPen(lPenOld);
}

void PainterPathMoves::addArrow (eDirectionArrow aDirArrow, QPointF aPos) {

    QPainterPath path;
    path.moveTo(aPos);

    switch (aDirArrow) {

        case eArrowLeft : {

            path.lineTo (aPos.x() + CArrowSize*qCos (PI/4),
                          aPos.y() - CArrowSize*qSin (PI/4)) ;
            path.lineTo (aPos.x() + CArrowSize*qCos (PI/4),
                          aPos.y() + CArrowSize*qSin (PI/4));
        break;
        }
        case eArrowRight : {

            path.lineTo (aPos.x() - CArrowSize*qCos (PI/4),
                          aPos.y() - CArrowSize*qSin (PI/4));
            path.lineTo (aPos.x() - CArrowSize*qCos (PI/4),
                          aPos.y() + CArrowSize*qSin (PI/4));
            break;
        }

        case eArrowTop : {

            path.lineTo (aPos.x() + CArrowSize*qSin (PI/4),
                          aPos.y() + CArrowSize*qCos (PI/4)) ;
            path.lineTo (aPos.x() - CArrowSize*qSin (PI/4),
                          aPos.y() + CArrowSize*qCos (PI/4));
            break;
        }

        case eArrowBottom : {

            path.lineTo (aPos.x() + CArrowSize*qSin (PI/4),
                          aPos.y() - CArrowSize*qCos (PI/4));
            path.lineTo (aPos.x() - CArrowSize*qSin (PI/4),
                          aPos.y() - CArrowSize*qCos (PI/4));
            break;
        }

        case eArrowTopLeft : {

            path.lineTo (aPos.x() + CArrowSize,
                          aPos.y());
            path.lineTo (aPos.x(),
                          aPos.y() + CArrowSize);
            break;
        }

        case eArrowTopRight : {

            path.lineTo (aPos.x() - CArrowSize,
                          aPos.y());
            path.lineTo (aPos.x(),
                          aPos.y() + CArrowSize);
            break;
        }

        case eArrowBottomLeft : {

            path.lineTo (aPos.x() + CArrowSize,
                          aPos.y());
            path.lineTo (aPos.x(),
                          aPos.y() - CArrowSize);
            break;
        }

        case eArrowBottomRight : {

            path.lineTo (aPos.x(),
                          aPos.y() - CArrowSize);
            path.lineTo (aPos.x() - CArrowSize,
                          aPos.y());
            break;
        }

    }

    path.closeSubpath();
    QBrush oldBrush = _Painter.brush();
    QBrush newbrush = QBrush (_Painter.pen().color());
    _Painter.setBrush(newbrush);
    _Painter.drawPath(path);
    _Painter.setBrush(oldBrush);
}

void PainterPathMoves::createLineDoubleArrow(eDirection aDir, QPointF p1, QPointF p2)
{

    QPainterPath path;
    path.moveTo (p1);
    path.lineTo (p2);
    _Painter.drawPath(path);

    if (aDir == eDiag1) {

        addArrow (eArrowTopLeft, p1);
        addArrow (eArrowBottomRight, p2);

    }
    else if (aDir == eDiag2) {

        addArrow (eArrowBottomLeft, p1);
        addArrow (eArrowTopRight, p2);

    }
    else if (aDir == eHoriz) {

        addArrow (eArrowLeft, p1);
        addArrow (eArrowRight, p2);

    } else if (aDir == eVertic) {

        addArrow (eArrowTop, p1);
        addArrow (eArrowBottom, p2);

    }

}

void PainterPathMoves::DessineFlechePawn (QPointF aCenterPawn, int aNbreCases) {

    QPainterPath path;
    path.moveTo (aCenterPawn);
    path.lineTo (aCenterPawn - QPointF(0, aNbreCases*_CellSize));
    _Painter.drawPath(path);

    //dessine fleche
    aCenterPawn = aCenterPawn - QPointF(0, aNbreCases*_CellSize);
    addArrow(eArrowTop, aCenterPawn);
}
