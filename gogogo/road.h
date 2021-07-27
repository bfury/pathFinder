#ifndef ROAD_H
#define ROAD_H
class PathGraph;
class QTextStream;
#include <QString>
#include <QColor>
#include <QPair>
#include <QSet>
#include <QVector>





//
typedef QPair<int,int> Edge;
// 定义高速路的边



class Road
{
protected:
    int id;
    //高速路id
    QString name;
    //高速路名称
    QColor color;
    //高速路颜色
    QVector <QString> fromTo;
    //高速路起始站点
    QSet<int> pointsSet;
    //高速路站点集合
    QSet<Edge> edges;
    //高速路站点连接

public:
    Road(){};
    Road(QString roadName, QColor roadColor):name(roadName), color(roadColor)
    {};

    friend class PathGraph;
    friend class QTextStream; //操作必须
};

#endif // LINE_H
