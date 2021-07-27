#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "zoomNet.h"
#include "pathNet.h"
//#include "pathOperate.h"
//#include "expand.h"
#include <QMainWindow>
#include <QLabel>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QFile>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

    void showall();
    void setIndex(int index);
    void updateChooseInfo();
    //出发点改变
    void newStarter(QString roadName);
    //目的点改变
    void newDestinationer(QString roadNames);
    //发送计算信号
    void signalToDijkstra();
private slots:
    void on_pushbig_clicked();
    void on_pushlittle_clicked();
    void on_pushshowall_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    void myConnect();

protected:
    Ui::MainWindow *ui;             //主窗口UI
    Graphics_view_zoom *myView;     //鼠标缩放
    QGraphicsScene *scene;
    PathGraph* pathGraph;


    //混合颜色，达到随机颜色效果 （伪
    QColor getRoadsColor(const QList<int>& roadsList);
    //获得线路表的名字集
    QString getRoadsName(const QList<int>& roadsList);
    //将站点的经纬度地理坐标转为视图坐标
    QPointF transferXY(QPointF XY);
    //绘制网络图的边
    void drawEdges (const QList<Edge>& edgesList);
    //绘制网络图的站点节点
    void drawPoints (const QList<int>& pointsList);


};




















































#define LINE_INFO_WIDTH 0
#define MARGIN 30
#define NET_WIDTH 2000      //最大宽度
#define NET_HEIGHT 2000     //最大高度
#define SCENE_WIDTH (LINE_INFO_WIDTH+MARGIN*2+NET_WIDTH)    //视图宽
#define SCENE_HEIGHT (MARGIN*2+NET_HEIGHT)                  //视图高

#define EDGE_PEN_WIDTH 2
#define NODE_HALF_WIDTH 3

#endif // MAINWINDOW_H
