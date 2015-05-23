#include "qgraphicspieceitem.h"
#include <QPainter>
#include <QGraphicsScene>
#include "TypeInfoChessBoard.hpp"
#include <QStyleOptionGraphicsItem>

QGraphicsPieceItem::QGraphicsPieceItem(const QPixmap& aImagePiece, eTypePiece aTypePiece, eColorPiece aColor): QObject(), QGraphicsPixmapItem(0),
    _TypePiece (aTypePiece),
    _ColorPiece (aColor)
{
   this->setTransformationMode(Qt::SmoothTransformation);
   this->setPixmap(aImagePiece);
   this->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
}

void QGraphicsPieceItem::ChangePieceGraphique (const QPixmap& aImagePiece) {

    this->setPixmap(aImagePiece);
}

void QGraphicsPieceItem::paint (QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget )
{

//    qreal lFactorX = getFactorX();
//    qreal lFactorY = getFactorY();
//    painter->scale(lFactorX, lFactorY);
//    painter->drawPixmap (QPoint(0,0),  _ImagePiece);
    QStyleOptionGraphicsItem *lnewoption = const_cast<QStyleOptionGraphicsItem*>(option);
    lnewoption->state &= ~QStyle::State_Selected;
    QGraphicsPixmapItem::paint(painter, lnewoption, widget);
}

//QRectF QGraphicsPieceItem::boundingRect() const
//{
//    return QRectF(0,
//                  0,
//                  (this->scene()->width()-constants::GLOBAL_CONST_BORDER)/constants::GLOBAL_CONST_DIMENSION,
//                  (this->scene()->height()-constants::GLOBAL_CONST_BORDER)/constants::GLOBAL_CONST_DIMENSION);
//    //return QRectF(0,0, getFactorX()*_ImagePiece.width(), getFactorY()*_ImagePiece.height());
//}
