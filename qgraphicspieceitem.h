#ifndef QGRAPHICSPIECEITEM_H
#define QGRAPHICSPIECEITEM_H

#include <QGraphicsObject>
#include <QGraphicsPixmapItem>
#include "TypePiece.hpp"

class QGraphicsPieceItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    QGraphicsPieceItem(const QPixmap& aImagePiece, eTypePiece aTypePiece, eColorPiece aColor);
    inline eTypePiece getTypePiece () const {return _TypePiece;}
    inline eColorPiece getColorPiece () const  {return _ColorPiece;}
 //   virtual QRectF boundingRect() const;
    void ChangePieceGraphique (const QPixmap& aImagePiece);
protected:
    virtual void paint ( QPainter * painter,
                         const QStyleOptionGraphicsItem * option,
                         QWidget * widget = 0 );
private:
    eTypePiece _TypePiece;
    eColorPiece _ColorPiece;
//    qreal getFactorX () const;
//    qreal getFactorY () const;
};

#endif // QGRAPHICSPIECEITEM_H
