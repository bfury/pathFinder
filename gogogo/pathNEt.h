#ifndef pathGRAPH_H
#define pathGRAPH_H

#include "point.h"
#include "road.h"
#include <QString>
#include <QPoint>
#include <QVector>
#include <QHash>

//图  邻接点
class Node{
public:
    int pointID;      //nodeID
    double distance;    //node与node之间的距离
    //构造函数
    Node(){}
    Node(int s, double dist) :pointID(s), distance(dist)
    {}


    bool operator > (const Node& n) const    //运算重载用于小顶堆的比较
    {
        return this->distance>n.distance;
    }
};

//操作高速网类
class PathGraph
{
protected:
    QVector<Point> points;          //存储【所有】结点
    QVector<Road> roads;                //存储【所有】主干道
    QHash<QString, int> pointsStorage;   //站点名到存储位置的哈希值
    QHash<QString, int> roadsStorage;         //线路名到存储位置的哈希值
    QSet<Edge> edges;                   //所有边的集合
    QVector<QVector<Node>> graph;       //网络图

public:

    PathGraph();

    QString getRoadName(int l);
    QColor getRoadColor(int l);
    int getRoadStorage(QString roadName);
    QList<int> getRoadsStorage(QList<QString> roadsList);
    QList<QString> getRoadsNameList();
    QList<QString> getRoadPointsList(int l);
    QString getPointName(int s);
    QPointF getPointXY(int s);
    QPointF getMinXY();
    QPointF getMaxXY();

    //获取结点在哪条主干道
    QList<int> getPointRoads(int s);
    //
    //获取两个站点的公共主干道
    //
    QList<int> getCommonRoads(int s1, int s2);
    //
    //


    //获取结点实际存储的值
    //和结点的集合实际存储的值
    int getPointStorage(QString pointName);
    QList<QString> getPointsNameList();


    void getGraph(QList<int>&pointsList, QList<Edge>&edgesList);


    //
    //
    //获取最短线路
    //
    //
    //
    bool Dijkstra(int s1, int s2,
                              QList<int>&pointsList,
                              QList<Edge>&edgesList);
    //
    //
    //
    //

    //读文件
    //
    bool NetFromFile(QString fileName);

private:
    void clearData();
    //插入一条边
    bool insertEdge(int s1, int s2);
    //更新边界 依靠坐标和经纬度
    void updateEdge();
    //生成
    void buildNet();
};

#endif // pathGRAPH_H
