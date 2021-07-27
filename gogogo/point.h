#ifndef POINT_H
#define POINT_H

#include <QString>
#include <QPointF>
#include <QSet>

class PathGraph;
class QTextStream;

class Point
{
protected:
    int id;
    QString name;
    double longitude, latitude; //节点经纬度
    QSet<int> roadsInfo;        //节点所属主干道

    //所有站点的边界位置
    static double minLongitude, minLatitude, maxLongitude, maxLatitude;

public:
    //构造函数
    Point();
    Point(QString nameStr, double longi, double lati, QList<int> roadsList);

protected:
    //求取站点间实地直线距离
    int distance(Point other);

    //声明友元
    friend class PathGraph;
    friend class QTextStream;
};

#endif
