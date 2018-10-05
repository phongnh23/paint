#include "intersectioncalculation.h"

IntersectionCalculation::IntersectionCalculation(QObject *parent) : QObject(parent)
{
}

bool IntersectionCalculation::isEqual(QPointF point1, QPointF point2) const
{
    qreal eps = 10e-10;
    if(qAbs(point1.x() - point2.x()) <= qAbs(point1.x()) * eps &&
       qAbs(point1.y() - point2.y()) <= qAbs(point1.y()) * eps)
        return true;
    return false;
}

qreal IntersectionCalculation::calculateAngle(const QPointF &basePoint, const QPointF &beginPoint, const QPointF &endPoint)
{
    /************ calculate angle of 2 points with a base point (|angle| <= PI)*/

    qreal angle1 = qAtan2((beginPoint - basePoint).y(), (beginPoint - basePoint).x());
    qreal angle2 = qAtan2((endPoint - basePoint).y(), (endPoint - basePoint).x());
    if(angle1 * angle2 >= 0)
        return angle2 - angle1;
    if(angle2 > angle1)
        return angle2 - angle1 > M_PI ? angle2 - angle1 - 2 * M_PI : angle2 - angle1;
    return angle2 - angle1 < -M_PI ? angle2 - angle1 + 2 * M_PI : angle2 - angle1;
}

QList<QPolygonF> IntersectionCalculation::defaultIntersected(const QPolygonF &polygon1, const QPolygonF &polygon2)
{
    /************ using default intersected function in Qt */

    QPainterPath subject; subject.addPolygon(polygon1);
    QPainterPath clip; clip.addPolygon(polygon2);

    return subject.intersected(clip).toFillPolygons();
//    QList<QPolygonF> list;
//    list << subject.intersected(clip).toFillPolygon().toPolygon();
//    return list;
}

bool IntersectionCalculation::checkContainPoint(const QPointF &point, const QPolygonF &polygon)
{
    /************ check if a point is inside a polygon */

    qreal angle = 0;
    int i;
    for(i = 0; i < polygon.size() - 1; ++i)
        angle += calculateAngle(point, polygon[i], polygon[i + 1]);
    angle += calculateAngle(point, polygon[i], polygon[0]);
//    qDebug() << "angle:" << angle;
    if(angle < 10e-3 && angle > -10e-3) // angle == 0 : outside, == 2PI : inside
        return false;
    return true;
}

QPointF IntersectionCalculation::lineIntersected(const QPointF &point1, const QPointF &point2,
                                                 const QPointF &point3, const QPointF &point4)
{
    /************ return (QPointF) intersection of 2 lines */
    /************ reference: http://www.cs.swan.ac.uk/~cssimon/line_intersection.html */

    qreal ta = (point3.y() - point4.y())*(point1.x() - point3.x()) + (point4.x() - point3.x())*(point1.y() - point3.y());
    qreal tb = (point1.y() - point2.y())*(point1.x() - point3.x()) + (point2.x() - point1.x())*(point1.y() - point3.y());
    qreal den = (point4.x() - point3.x())*(point1.y() - point2.y()) - (point1.x() - point2.x())*(point4.y() - point3.y());
    ta = ta/den;
    tb = tb/den;
    if(0 <= ta && ta <= 1 && 0 <= tb && tb <= 1)
        return(point1 + ta*(point2 - point1));
    return QPointF(0,0);    // NULL QPointF
}

bool IntersectionCalculation::lineIntersects(const QPointF &point1, const QPointF &point2,
                                             const QPointF &point3, const QPointF &point4)
{
    qreal ta = (point3.y() - point4.y())*(point1.x() - point3.x()) + (point4.x() - point3.x())*(point1.y() - point3.y());
    qreal tb = (point1.y() - point2.y())*(point1.x() - point3.x()) + (point2.x() - point1.x())*(point1.y() - point3.y());
    qreal den = (point4.x() - point3.x())*(point1.y() - point2.y()) - (point1.x() - point2.x())*(point4.y() - point3.y());
    ta = ta/den;
    tb = tb/den;
    if(0 <= ta && ta <= 1 && 0 <= tb && tb <= 1)
        return true;
    return false;
}

qreal IntersectionCalculation::distance(const QPointF &point1, const QPointF &point2)
{
    return qSqrt((point1.x() - point2.x()) * (point1.x() - point2.x()) +
                 (point1.y() - point2.y()) * (point1.y() - point2.y()));
}

bool IntersectionCalculation::sortFuntion(Point point1, Point point2)
{
    /************ check which point is nearer to the begining point of line */

    return distance(point1, point1.line1_point1) < distance(point2, point2.line1_point1);
}

QList<Point> IntersectionCalculation::toPolygonWithIntersection(const QPolygonF &inSubject, const QPolygonF &inClipped)
{
    /************ add intersection points to the polygon */

    QPolygonF subject = inSubject, clip = inClipped;
    subject.append(inSubject.first());  // append begining point to last
    clip.append(clip.first());
    QList<Point> polygonWithIntersection;
    for(int i = 0; i < subject.size() - 1; ++i)
    {
        polygonWithIntersection.append(Point(subject[i]));  // add first point
        QList<Point> localList;     // contain intersections of Line[i,i+1] with the clipped shape
        for(int j = 0; j < clip.size() - 1; ++j)
        {
            if(lineIntersects(subject[i], subject[i + 1], clip[j], clip[j+1]))
            {
                Point tempPoint(lineIntersected(subject[i], subject[i + 1], clip[j], clip[j+1]), subject[i], subject[i + 1], clip[j], clip[j+1]);
                if(localList.isEmpty())
                    localList.append(tempPoint);    // add intersection point
                else
                {
                    bool insertedFlag = false;
                    for(int k = 0; k < localList.size(); ++k)
                        if(sortFuntion(tempPoint, localList[k]))    // add intersection point to the right position
                        {
                            localList.insert(k, tempPoint);
                            insertedFlag = true;
                            break;
                        }
                    if(!insertedFlag)
                        localList.append(tempPoint);    // if not find a position, append to last
                }
            }
        }
        foreach(Point point, localList)
        {
            polygonWithIntersection.append(point);  // add to the result
        }
    }
    return polygonWithIntersection;
}

QList<Point> IntersectionCalculation::getPointsInside(const QList<Point> &pointList, const Point &inPoint, const QPolygonF &polygon)
{
    /************ find points of pointList beginning from inPoint,
                  which are inside polygon until the pointList gets out of the polygon*/

    int index, maxIndex = pointList.size() - 1;
    bool isNext = false;
    QList<Point> insidePoints; // result
    for(index = 0; index < pointList.size(); ++index)
    {
        if(inPoint.isEqual(pointList.at(index)))    // find the index of inPoint in pointList
        {
//            qDebug() << "*1";
            break;
        }
    }
    Point checkPoint = pointList.at(index == maxIndex ? 0 : index + 1); // checkPoint assumably is the next point in pointList and inside polygon
//    qDebug() << "*2";
    QPointF midPoint((inPoint.x()+checkPoint.x()) / 2, (inPoint.y()+checkPoint.y()) / 2);
    if(checkContainPoint(midPoint, polygon))    // checkPoint is the right point and we will go forward,
    {                                           // to get outside points, add ! before if-condiditon.
        isNext = true;
        index = index == maxIndex ? 0 : index + 1;
    }
    else
    {
        index = index == 0 ? maxIndex : index - 1;
        checkPoint = pointList.at(index); // else, we will go backward and choose another checkPoint
//        qDebug() << "*3";
    }
    insidePoints.append(inPoint);   // append to result
    insidePoints.append(checkPoint);
    int numLoop = 0;    // variable to limit number of loop (< 1000)
    while(!checkPoint.isIntersection() && numLoop < 1000)
    {
        if(isNext)
        {
            index = index == maxIndex ? 0 : index + 1;
            checkPoint = pointList.at(index);
//            qDebug() << "*4";
        }
        else
        {
            index = index == 0 ? maxIndex : index - 1;
            checkPoint = pointList.at(index);
//            qDebug() << "*5";
        }
        insidePoints.append(checkPoint);    // append checkPoint to result if it is still inside polygon
        ++numLoop;
    }
    if(numLoop == 1000) qDebug() << "break loop!"; // err
    return insidePoints;
}

QList<QPolygonF> IntersectionCalculation::customedIntersected(QPolygonF &polygon1, QPolygonF &polygon2)
{
    /************ customed intersected function */

    QList<QPolygonF> intersectedPolygons;   // initialize result
    // preprocess input: remove coincident points
    for(int i = 0; i < polygon1.size(); ++i){
        while(i + 1 < polygon1.size() && isEqual(polygon1[i], polygon1[i+1])){
            polygon1.removeAt(i);
        }
    }
    for(int i = 0; i < polygon2.size(); ++i){
        while(i + 1 < polygon2.size() && isEqual(polygon2[i], polygon2[i+1])){
            polygon2.removeAt(i);
        }
    }
    // not a polygon, return empty list
    if(polygon1.size() < 3 || polygon2.size() < 3) return intersectedPolygons;

    QList<Point> polygon1WithInterSect = toPolygonWithIntersection(polygon1,polygon2);
    // not intersect, return the inside polygon
    if(polygon1WithInterSect.size() == polygon1.size()){
        if(checkContainPoint(polygon1.first(), polygon2)) intersectedPolygons << polygon1;
        else if(checkContainPoint(polygon2.first(), polygon1)) intersectedPolygons << polygon2;
        return intersectedPolygons;
    }
    QList<Point> polygon2WithInterSect = toPolygonWithIntersection(polygon2,polygon1);
    QList<Point> tempPoints;    // points of 1 intersection polygon
    QPolygonF tempPolygon;      // 1 intersection polygon
    bool inTurn = true;         // to change turn
//    foreach(Point point, polygon1WithInterSect){  // containter thay doi => k dung foreach, thay bang for
    for(int i = 0; i < polygon1WithInterSect.size(); ++i){
        Point point = polygon1WithInterSect.at(i);
//        qDebug() << "*6";
        if(point.isIntersection() && !point.isPassed()){
            Point inPoint = point;  // the beginning point to go in a polygon
            int numLoop = 0;
            do{
                QList<Point> insidePoints = getPointsInside(inTurn ? polygon1WithInterSect : polygon2WithInterSect, inPoint, inTurn ? polygon2 : polygon1);
                foreach(Point point2, insidePoints){
                    tempPoints.append(point2);
                }
                inPoint = tempPoints.last();    // next turn, change the inPoint
                tempPoints.pop_back();          // remove the overlap
                inTurn = !inTurn;

                // mark the points which were passed in 2 polygonWithInterSect lists
                for(int i = 0; i < polygon1WithInterSect.size(); ++i){
                    if(polygon1WithInterSect.at(i).isEqual(insidePoints.first()) || polygon1WithInterSect.at(i).isEqual(insidePoints.last())){
//                        qDebug() << "*7";
                        Point temp = polygon1WithInterSect.at(i); temp.passed();
//                        qDebug() << "*9";
                        polygon1WithInterSect.replace(i,temp);
                    }
                }
                for(int i = 0; i < polygon2WithInterSect.size(); ++i){
                    if(polygon2WithInterSect.at(i).isEqual(insidePoints.first()) || polygon2WithInterSect.at(i).isEqual(insidePoints.last())){
//                        qDebug() << "*10";
                        Point temp = polygon2WithInterSect.at(i); temp.passed();
//                        qDebug() << "*12";
                        polygon2WithInterSect.replace(i,temp);
                    }
                }

                ++numLoop;  // limit number of loop (< 1000)
            } while(!inPoint.isEqual(point) && numLoop < 1000);
            if(numLoop == 1000) qDebug() << "break loop!";   // err
            foreach(Point point2, tempPoints){
                tempPolygon.append(QPointF(point2.x(), point2.y()));    // convert points list to polygon
            }

            intersectedPolygons.append(tempPolygon);    // append to result
            tempPoints.clear(); // clean for new loop
            tempPolygon.clear();
        }
    }
    return intersectedPolygons;
}

/********************************************* Point Class **************************************************/

Point::Point(QPointF point) : QPointF(point)
{
    line1_point1 = QPointF(0,0);
    line1_point2 = QPointF(0,0);
    line2_point1 = QPointF(0,0);
    line2_point2 = QPointF(0,0);
    intersectFlag = false;
    passedFlag = false;
}

Point::Point(QPointF point, QPointF point1, QPointF point2, QPointF point3, QPointF point4) : QPointF(point)
{
    line1_point1 = point1;
    line1_point2 = point2;
    line2_point1 = point3;
    line2_point2 = point4;
    intersectFlag = true;
    passedFlag = false;
}

bool Point::isEqual(QPointF point1, QPointF point2) const
{
    /************ check if 2 points are the same */

    qreal eps = 10e-10;
    if(qAbs(point1.x() - point2.x()) <= qAbs(point1.x()) * eps &&
       qAbs(point1.y() - point2.y()) <= qAbs(point1.y()) * eps)
        return true;
    return false;
}

bool Point::isEqual(Point point) const
{
    /************ check if 2 intersection points are the same,
                  2 cross lines must be also the same */

    if(isEqual(QPointF(point.x(), point.y()), QPointF(x(),y()))){
        if(     isEqual(point.line1_point1, line1_point1) &&    //2 line giong nhau
                isEqual(point.line1_point2, line1_point2) &&
                isEqual(point.line2_point1, line2_point1) &&
                isEqual(point.line2_point2, line2_point2))
            return true;
        if(     isEqual(point.line1_point1, line1_point2) &&    //giong nhau nhung line 1 nguoc dau
                isEqual(point.line1_point2, line1_point1) &&
                isEqual(point.line2_point1, line2_point1) &&
                isEqual(point.line2_point2, line2_point2))
            return true;
        if(     isEqual(point.line1_point1, line1_point1) &&    //giong nhau nhung line 2 nguoc dau
                isEqual(point.line1_point2, line1_point2) &&
                isEqual(point.line2_point1, line2_point2) &&
                isEqual(point.line2_point2, line2_point1))
            return true;
        if(     isEqual(point.line1_point1, line1_point2) &&    //giong nhau nhung ca line1 va line2 nguoc dau
                isEqual(point.line1_point2, line1_point1) &&
                isEqual(point.line2_point1, line2_point2) &&
                isEqual(point.line2_point2, line2_point1))
            return true;
        if(     isEqual(point.line1_point1, line2_point1) &&    //2 line khau nhau
                isEqual(point.line1_point2, line2_point2) &&
                isEqual(point.line2_point1, line1_point1) &&
                isEqual(point.line2_point2, line1_point2))
            return true;
        if(     isEqual(point.line1_point1, line2_point2) &&    //khau nhau va line 1 nguoc dau
                isEqual(point.line1_point2, line2_point1) &&
                isEqual(point.line2_point1, line1_point1) &&
                isEqual(point.line2_point2, line1_point2))
            return true;
        if(     isEqual(point.line1_point1, line2_point1) &&    //khau nhau va line 2 nguoc dau
                isEqual(point.line1_point2, line2_point2) &&
                isEqual(point.line2_point1, line1_point2) &&
                isEqual(point.line2_point2, line1_point1))
            return true;
        if(     isEqual(point.line1_point1, line2_point2) &&    //khau nhau va ca line1 va line2 nguoc dau
                isEqual(point.line1_point2, line2_point1) &&
                isEqual(point.line2_point1, line1_point2) &&
                isEqual(point.line2_point2, line1_point1))
            return true;
    }
    return false;
}
