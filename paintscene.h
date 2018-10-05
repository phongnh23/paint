#ifndef PAINTSCENE_H
#define PAINTSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <QDebug>
#include "intersectioncalculation.h"

class paintScene : public QGraphicsScene
{

    Q_OBJECT

public:
    explicit paintScene(QObject *parent = 0);

    ~paintScene();

private:
    QPointF previousPoint;

    bool firstClick, firstPolygon;

    QPolygonF polygon1, polygon2;

    QList<QPolygonF> polygons;

    IntersectionCalculation intersection;

private:

    void mousePressEvent(QGraphicsSceneMouseEvent * event);

//    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

};

#endif // PAINTSCENE_H
