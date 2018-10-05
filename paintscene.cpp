#include "paintscene.h"

paintScene::paintScene(QObject *parent) : QGraphicsScene(parent)
{
    firstClick = true;
    firstPolygon = true;
}

paintScene::~paintScene()
{

}

void paintScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(firstClick)
        {
            previousPoint = event->scenePos();
            firstClick = false;
            if(firstPolygon)
            {
                if(!polygon1.isEmpty())
                    polygon1.clear();
                polygon1.append(previousPoint);
            }
            else
            {
                if(!polygon2.isEmpty())
                    polygon2.clear();
                polygon2.append(previousPoint);
            }
            qDebug() << previousPoint;
            addEllipse(previousPoint.x() - 2,
                       previousPoint.y() - 2,
                       4,
                       4,
                       QPen(Qt::NoPen),
                       firstPolygon ? QBrush(Qt::red) : QBrush(Qt::green));
        }
        else
        {
            addLine(previousPoint.x(),
                    previousPoint.y(),
                    event->scenePos().x(),
                    event->scenePos().y(),
                    QPen(firstPolygon ? Qt::red : Qt::green,2,Qt::SolidLine,Qt::RoundCap));
            previousPoint = event->scenePos();
            if(firstPolygon)
                polygon1.append(previousPoint);
            else
                polygon2.append(previousPoint);
            qDebug() << previousPoint;
        }
    }
    else if(event->button() == Qt::RightButton && !firstClick)
    {
        addLine(previousPoint.x(),
                previousPoint.y(),
                firstPolygon ? polygon1.first().x() : polygon2.first().x(),
                firstPolygon ? polygon1.first().y() : polygon2.first().y(),
                QPen(firstPolygon ? Qt::red : Qt::green,2,Qt::SolidLine,Qt::RoundCap));
        if(!firstPolygon)
        {
//            polygons = intersection.defaultIntersected(polygon1, polygon2);
            // using customed intersected function
            polygons = intersection.customedIntersected(polygon1, polygon2);
            foreach(QPolygonF poly, polygons){
                qDebug() << poly;
            }
            QPolygonF polygon;
            foreach(polygon, polygons){
                addPolygon(polygon, QPen(Qt::blue,3,Qt::DashLine,Qt::RoundCap)/*, QBrush(Qt::blue)*/);
            }
        }
        firstClick = true;
        firstPolygon = !firstPolygon;
    }
    else
    {
        clear();
        firstClick = true;
        firstPolygon = true;
    }
}

//void paintScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
//{
//}
