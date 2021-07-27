#include "pathNet.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <queue>
#include<QTextCodec>
#define INF 0x7f7f7f7f


PathGraph::PathGraph(){}

void PathGraph::clearData()
{
    points.clear();
    roads.clear();
    pointsStorage.clear();
    roadsStorage.clear();
    edges.clear();
    graph.clear();
}

//插入边
bool PathGraph::insertEdge(int n1, int n2)
{
    if (edges.contains(Edge(n1, n2)) || edges.contains(Edge(n2, n1)))
    {
        return false;
    }
    edges.insert(Edge(n1, n2));
    return true;
}

//生成图结构
void PathGraph::buildNet()
{
    graph.clear();
    graph=QVector<QVector<Node>>(points.size(), QVector<Node>());
    for (auto &a : edges)
    {
        double dist=points[a.first].distance(points[a.second]);
        graph[a.first].push_back(Node(a.second, dist));
        graph[a.second].push_back(Node(a.first, dist));
    }
}


//获取线路颜色
QColor PathGraph::getRoadColor(int l)
{
    return roads[l].color;
}

//获取线路名
QString PathGraph::getRoadName(int l)
{
    return roads[l].name;
}

//获取哈希后的存储值
int PathGraph::getRoadStorage(QString roadName)
{
    if(roadsStorage.contains(roadName))
    {
        return roadsStorage[roadName];
    }
    return -1;
}

QList<int> PathGraph::getRoadsStorage(QList<QString> roadsList)
{
    QList<int> StorageList;
    for (auto &a:roadsList)
    {
        StorageList.push_back(getRoadStorage(a));
    }
    return StorageList;
}

//获取线路名集合
QList<QString> PathGraph::getRoadsNameList()
{
    QList<QString> roadsNameList;
    for (auto a:roads)
    {
        roadsNameList.push_back(a.name);
    }
    return roadsNameList;
}

//获取线路的所有包含站点
QList<QString> PathGraph::getRoadPointsList(int l)
{
    QList<QString> pointsList;
    for (auto &a:roads[l].pointsSet)
    {
        pointsList.push_back(points[a].name);
    }
    return pointsList;
}



//更新边界经纬度
void PathGraph::updateEdge()
{
    double minLongitude=200, minLatitude=200;
    double maxLongitude=0, maxLatitude=0;

    for (auto &s : points)
    {
        minLongitude = qMin(minLongitude, s.longitude);
        minLatitude = qMin(minLatitude, s.latitude);
        maxLongitude = qMax(maxLongitude, s.longitude);
        maxLatitude = qMax(maxLatitude, s.latitude);
    }
    Point::minLongitude = minLongitude;
    Point::minLatitude = minLatitude;
    Point::maxLongitude = maxLongitude;
    Point::maxLatitude = maxLatitude;

}

 //获取站点最小坐标
QPointF PathGraph::getMinXY()
{
    return QPointF(Point::minLongitude, Point::minLatitude);
}

//获取站点最大坐标
QPointF PathGraph::getMaxXY()
{
    return QPointF(Point::maxLongitude, Point::maxLatitude);
}

//获取两个站点的公共所属线路
QList<int> PathGraph::getCommonRoads(int s1, int s2)
{
    QList<int> roadsList;
    for (auto &s : points[s1].roadsInfo)
    {
        if(points[s2].roadsInfo.contains(s))
            roadsList.push_back(s);
    }
    return roadsList;
}


QString PathGraph::getPointName(int s)
{
    return points[s].name;
}

//获取站点地理坐标
QPointF PathGraph::getPointXY(int s)
{
    return QPointF(points[s].longitude, points[s].latitude);
}

//获取站点所属线路
QList<int> PathGraph::getPointRoads(int s)
{
    return points[s].roadsInfo.toList();
}

//获取站点Storage值
int PathGraph::getPointStorage(QString pointName)
{
    if(pointsStorage.contains(pointName))
    {
        return pointsStorage[pointName];
    }
    return -1;
}
QList<QString> PathGraph::getPointsNameList()
{
    QList<QString> list;
    for (auto &a: points)
    {
        list.push_back(a.name);
    }
    return list;
}






//获取网络结构，用于前端显示
void PathGraph::getGraph(QList<int>&pointsList, QList<Edge>&edgesList)
{
    pointsList.clear();
    for (int i=0; i<points.size(); ++i)
    {
        pointsList.push_back(i);
    }
    edgesList=edges.toList();
    return ;
}

//读取数据
bool PathGraph::NetFromFile(QString fileName)
{
    clearData();
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return false;
    QTextStream in(&file);

     in.setCodec("UTF-8");

    while(!in.atEnd())
    {
        Road road;
        QString id, name, colour, fromTo, totalPoints;
        QString color, froms, tos;
        bool ok;
        int total;
        Point point;
        int lvIndex, svIndex1, svIndex2;

        in>>id>>road.id;
        in>>name>>road.name;
        in>>colour>>color;
        road.color.setRgba(color.remove(0,1).toUInt(&ok, 16));
        in>>fromTo>>froms>>tos;
        in>>totalPoints>>total;

        road.fromTo.push_back(froms);
        road.fromTo.push_back(tos);
        if (roadsStorage.count(road.name))
        {
            lvIndex = roadsStorage[road.name];
            roads[lvIndex].fromTo.push_back(froms);
            roads[lvIndex].fromTo.push_back(tos);
        }
        else
        {
            lvIndex = roadsStorage[road.name] = roads.size();
            roads.push_back(road);
        }

        QString longlat;
        QStringList strList;
        for (int i=0; !in.atEnd()&&i<total; ++i)
        {
            in>>point.id>>point.name>>longlat;
            strList=longlat.split(QChar(','));
            point.longitude=strList.first().toDouble();
            point.latitude=strList.last().toDouble();


            if (pointsStorage.count(point.name))
            {
                svIndex2 = pointsStorage[point.name];
            }
            else
            {
                svIndex2 = pointsStorage[point.name] = points.size();
                points.push_back(point);
            }

            points[svIndex2].roadsInfo.insert(lvIndex);
            roads[lvIndex].pointsSet.insert(svIndex2);

            if (i)
            {
                roads[lvIndex].edges.insert(Edge(svIndex1, svIndex2));
                roads[lvIndex].edges.insert(Edge(svIndex2, svIndex1));
                insertEdge(svIndex1, svIndex2);
            }
            svIndex1 = svIndex2;
        }

        bool flag = id=="id:" && name=="name:" && colour=="colour:" && fromTo=="fromTo:"
                && totalPoints=="totalPoints:" && ok && !in.atEnd();

        if(flag==false)
        {
            file.close();
            clearData();
            return false ;
        }
        in.readLine();
    }
    file.close();

    updateEdge();

    return true;
}



//获取最少时间的线路
bool PathGraph::Dijkstra(int s1, int s2, QList<int>&pointsList, QList<Edge>&edgesList)
{
    pointsList.clear();
    edgesList.clear();

    if(s1==s2)
    {
        pointsList.push_back(s2);
        pointsList.push_back(s1);
        return true;
    }
    buildNet();

    std::vector<int> path(points.size(), -1);
    std::vector<double> dist(points.size(), INF);
    dist[s1]=0;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> priQ;
    priQ.push(Node(s1, 0));
    while(!priQ.empty())
    {
        Node top=priQ.top();
        priQ.pop();
        if(top.pointID==s2)
        {
            break;
        }

        for (int i=0; i<graph[top.pointID].size(); ++i)
        {
            Node &adjNode=graph[top.pointID][i];
            if(top.distance+adjNode.distance<dist[adjNode.pointID])
            {
                path[adjNode.pointID]=top.pointID;
                dist[adjNode.pointID]=top.distance+adjNode.distance;
                priQ.push(Node(adjNode.pointID, dist[adjNode.pointID]));
            }
        }
    }

    if(path[s2]==-1)
    {
        return false;
    }
    int p=s2;
    while(path[p]!=-1)
    {
        pointsList.push_front(p);
        edgesList.push_front(Edge(path[p],p));
        p=path[p];
    }
    pointsList.push_front(s1);
    return true;
}

