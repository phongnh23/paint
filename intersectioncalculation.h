#ifndef INTERSECTIONCALCULATION_H
#define INTERSECTIONCALCULATION_H

#include <QObject>
#include <QtMath>
#include <QPainter>
#include <QDebug>

class IntersectionCalculation;

class Point : public QPointF
{
    friend class IntersectionCalculation;
public:
    Point(QPointF point);

    Point(QPointF point, QPointF point1, QPointF point2, QPointF point3, QPointF point4);

    bool inline isIntersection() {return intersectFlag;}

    void inline passed() {passedFlag = true;}

    bool inline isPassed() {return passedFlag;}

    bool isEqual(QPointF point1, QPointF point2) const;

    bool isEqual(Point point) const;

public:
    QPointF line1_point1, line1_point2, line2_point1, line2_point2;

private:
    bool intersectFlag, passedFlag;
};

class IntersectionCalculation : public QObject
{
    Q_OBJECT
public:
    explicit IntersectionCalculation(QObject *parent = 0);

    bool isEqual(QPointF point1, QPointF point2) const;

    qreal calculateAngle(const QPointF &basePoint, const QPointF &beginPoint, const QPointF &endPoint);

    QList<QPolygonF> defaultIntersected(const QPolygonF &polygon1, const QPolygonF &polygon2);

    bool checkContainPoint(const QPointF &point, const QPolygonF &polygon);

    QPointF lineIntersected(const QPointF &point1, const QPointF &point2, const QPointF &point3, const QPointF &point4);

    bool lineIntersects(const QPointF &point1, const QPointF &point2, const QPointF &point3, const QPointF &point4);

    qreal distance(const QPointF &point1, const QPointF &point2);

    bool sortFuntion(Point point1, Point point2);

    QList<Point> toPolygonWithIntersection(const QPolygonF &inSubject, const QPolygonF &inClipped);

    QList<Point> getPointsInside(const QList<Point> &pointList, const Point &inPoint, const QPolygonF &polygon);

    QList<QPolygonF> customedIntersected(QPolygonF &polygon1, QPolygonF &polygon2);

signals:

public slots:

private:

};

#endif // INTERSECTIONCALCULATION_H
