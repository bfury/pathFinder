#include "ui_GUI.h"
#include "GUI.h"
#include "pathNet.h"

#include <QGraphicsItem>
#include <QMessageBox>
#include <QColorDialog>
#include <QTimer>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "iostream"
#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    myView = new Graphics_view_zoom(ui->graphicsView);
    myView->set_modifiers(Qt::NoModifier);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    scene=new QGraphicsScene;
    scene->setSceneRect(-LINE_INFO_WIDTH,0,SCENE_WIDTH,SCENE_HEIGHT);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    pathGraph = new PathGraph;

    bool flag = pathGraph->NetFromFile(":/datas.txt");
    if (!flag)
    {
        QMessageBox box;
        box.setWindowTitle(tr("error information"));
        box.setIcon(QMessageBox::Warning);
        box.setText("读取交通图数据错误!\n！");
        box.addButton(tr("确定"), QMessageBox::AcceptRole);
        if (box.exec() == QMessageBox::Accepted)
        {
            box.close();
        }
    }

    myConnect();

    updateChooseInfo();

    showall();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete myView;
    delete scene;
    delete pathGraph;
}


void MainWindow::myConnect()  //信号&槽
{
    connect(ui->StartRoad, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(newStarter(QString)));
    connect(ui->DstRoad, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(newDestinationer(QString)));
    connect(ui->pushButtonTransfer, SIGNAL(clicked()), this, SLOT(signalToDijkstra()));
}


//由线路表计算投射到graphicsview界面的颜色
//该计算方法来自百度地图
QColor MainWindow::getRoadsColor(const QList<int>& roadsList)
{
    QColor color1=QColor(255,255,255);
    QColor color2;
    for (int i=0; i<roadsList.size(); ++i)
    {
        color2=pathGraph->getRoadColor(roadsList[i]);
        color1.setRed(color1.red()*color2.red()/255);
        color1.setGreen(color1.green()*color2.green()/255);
        color1.setBlue(color1.blue()*color2.blue()/255);
    }
    return color1;
}

//取线路的名字集
QString MainWindow::getRoadsName(const QList<int>& roadsList)
{
    QString str;
    str+="\t";
    for (int i=0; i<roadsList.size(); ++i)
    {
        str+=" ";
        str+=pathGraph->getRoadName(roadsList[i]);
    }

    return str;
}

//经纬度转视图坐标
QPointF MainWindow::transferXY(QPointF XY)
{
    QPointF minXY=pathGraph->getMinXY();
    QPointF maxXY=pathGraph->getMaxXY();
    double x = (XY.x()-minXY.x())/(maxXY.x()-minXY.x())*NET_WIDTH+MARGIN;
    double y = (maxXY.y()-XY.y())/(maxXY.y()-minXY.y())*NET_HEIGHT+MARGIN;
    return QPointF(x,y);
}

//绘制网络图的边
void MainWindow::drawEdges(const QList<Edge>& edgesList)
{
    for(int i=0; i<edgesList.size(); ++i)
    {
        int s1=edgesList[i].first;
        int s2=edgesList[i].second;

        QList<int> roadsList=pathGraph->getCommonRoads(s1, s2);
        QColor color=getRoadsColor(roadsList);
        QString tip="途经： "+pathGraph->getPointName(s1)+"--"+pathGraph->getPointName(s2)+"\n线路：";
        tip+=getRoadsName(roadsList);
        QPointF s1Pos=transferXY(pathGraph->getPointXY(s1));
        QPointF s2Pos=transferXY(pathGraph->getPointXY(s2));

        QGraphicsLineItem* edgeItem=new QGraphicsLineItem;
        edgeItem->setPen(QPen(color, EDGE_PEN_WIDTH));
        edgeItem->setCursor(Qt::PointingHandCursor);
        edgeItem->setToolTip(tip);
        edgeItem->setPos(s1Pos);
        edgeItem->setLine(0, 0, s2Pos.x()-s1Pos.x(), s2Pos.y()-s1Pos.y());
        scene->addItem(edgeItem);
    }
}

//绘制网络图节点
void MainWindow::drawPoints (const QList<int>& pointsList)
{
    for (int i=0; i<pointsList.size(); ++i)
    {
        int s=pointsList[i];
        QString name=pathGraph->getPointName(s);
        QList<int> roadsList=pathGraph->getPointRoads(s);
        QColor color=getRoadsColor(roadsList);
        QPointF longiLati=pathGraph->getPointXY(s);
        QPointF XY=transferXY(longiLati);
        QString tip="站名：  "+name+"\n"+
                "经度：  "+QString::number(longiLati.x(),'f',7)+"\n"+
                "纬度：  "+QString::number(longiLati.y(),'f',7)+"\n"+
                "线路："+getRoadsName(roadsList);

        QGraphicsEllipseItem* pointItem=new QGraphicsEllipseItem;
        pointItem->setRect(-NODE_HALF_WIDTH, -NODE_HALF_WIDTH, NODE_HALF_WIDTH<<1, NODE_HALF_WIDTH<<1);
        pointItem->setPos(XY);
        pointItem->setPen(color);
        pointItem->setCursor(Qt::PointingHandCursor);
        pointItem->setToolTip(tip);

        if(roadsList.size()<=1)
        {
            pointItem->setBrush(QColor(QRgb(0xffffff)));
        }

        scene->addItem(pointItem);

        QGraphicsTextItem* textItem=new QGraphicsTextItem;
        textItem->setPlainText(name);
        textItem->setFont(QFont("consolas",4,1));
        textItem->setPos(XY.x(),XY.y()-NODE_HALF_WIDTH*2);
        scene->addItem(textItem);
    }
}


//更新信息
void MainWindow::updateChooseInfo()
{

    QComboBox* comboL1=ui->StartRoad;
    QComboBox* comboL2=ui->DstRoad;

    comboL1->clear();
    comboL2->clear();
    QList<QString> roadsList=pathGraph->getRoadsNameList();
    for(auto &a:roadsList)
    {
        comboL1->addItem(a);
        comboL2->addItem(a);
    }
    newStarter(comboL1->itemText(0));
    newDestinationer(comboL2->itemText(0));
}

//@出发线路改变槽函数
void MainWindow::newStarter(QString roadName)
{
    QComboBox* comboS1=ui->StartPoint;
    comboS1->clear();
    QString helper_early = "前期救援站";
    QString helper = "中期救援站";
    QString helper_late = "后期救援站";
    int roadStorage=pathGraph->getRoadStorage(roadName);
    if(roadStorage==-1)
    {
        return ;
    }

    QList<QString> pointsList=pathGraph->getRoadPointsList(roadStorage);
    for(auto &a:pointsList)
    {
        if(QString::compare(a,helper_early)==0 || QString::compare(a,helper)==0 || QString::compare(a,helper_late)==0)
        comboS1->addItem(a);
    }
}

//@目的线路改变槽函数
void MainWindow::newDestinationer(QString roadName)
{
    QString helper_early = "前期救援站";
    QString helper = "中期救援站";
    QString helper_late = "后期救援站";
    QComboBox* comboS2=ui->DstPoint;
    comboS2->clear();

    int roadStorage=pathGraph->getRoadStorage(roadName);
    if(roadStorage==-1)
    {
        return ;
    }

    QList<QString> pointsList=pathGraph->getRoadPointsList(roadStorage);
    for(auto &a:pointsList)
    {
        if(QString::compare(a,helper_early)!=0 && QString::compare(a,helper)!=0 && QString::compare(a,helper_late)!=0)
        comboS2->addItem(a);
    }
}

//@查询槽函数
void MainWindow::signalToDijkstra()
{
    int s1=pathGraph->getPointStorage(ui->StartPoint->currentText());
    int s2=pathGraph->getPointStorage(ui->DstPoint->currentText());

    if(s1==-1||s2==-1)
    {
        QMessageBox box;
        box.setWindowTitle(tr("path"));
        box.setIcon(QMessageBox::Warning);
        box.setText(tr("请选择有效救援站!"));
        box.addButton(tr("彳亍口巴"),QMessageBox::AcceptRole);
        if(box.exec()==QMessageBox::Accepted)
        {
            box.close();
        }
    }
    else
    {
        QList<int> pointsList;
        QList<Edge> edgesList;
        bool flag=true;
        flag=pathGraph->Dijkstra(s1, s2, pointsList, edgesList);
        if(flag)
        {
            scene->clear();
            drawEdges(edgesList);
            drawPoints(pointsList);
            QString text="from:";
            for(int i=0; i<pointsList.size(); ++i)
            {
                if(i)
                {
                    text+="   🚓   ";
                }
                text+=pathGraph->getPointName(pointsList[i]);
                qDebug()<<"\n"<<text;
            }
            QTextBrowser* browser=ui->textBrowserRoute;
            browser->clear();
            browser->setText(text);
            QMessageBox box;
            box.setWindowTitle(tr("path"));
            box.setIcon(QMessageBox::Information);
            box.setText(tr("成功查找到相应路径"));
            box.addButton(tr("彳亍口巴"),QMessageBox::AcceptRole);
            if(box.exec()==QMessageBox::Accepted)
            {
                box.close();
            }
        }
        else
        {
            QMessageBox box;
            box.setWindowTitle(tr("path"));
            box.setIcon(QMessageBox::Warning);
            box.setText(tr("暂时无法到达！"));
            box.addButton(tr("彳亍口巴"),QMessageBox::AcceptRole);
            if(box.exec()==QMessageBox::Accepted)
            {
                box.close();
            }
        }
    }
}

void MainWindow::setIndex(int index)
{
    int tempindex = index;
    tempindex++;
    return;
}






void MainWindow::on_pushshowall_clicked()
{
showall();
}

void MainWindow::on_pushButton_clicked()
{
    QTime time;
    time= QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    int n = qrand() % 6;    //产生6以内的随机数

    QComboBox* comboL1=ui->StartRoad;
    QComboBox* comboL2=ui->DstRoad;
//    QComboBox* comboR1=ui->StartPoint;
//    QComboBox* comboR2=ui->DstPoint;
    comboL1->clear();
    comboL2->clear();
    QList<QString> roadsList=pathGraph->getRoadsNameList();
    for(auto &a:roadsList)
    {
        comboL1->addItem(a);
        comboL2->addItem(a);
    }
    newStarter(comboL1->itemText(0));
    newDestinationer(comboL2->itemText(n));

    int s1=pathGraph->getPointStorage(ui->StartPoint->currentText());
    int s2=pathGraph->getPointStorage(ui->DstPoint->currentText());

    if(s1==-1||s2==-1)
    {
        QMessageBox box;
        box.setWindowTitle(tr("path"));
        box.setIcon(QMessageBox::Warning);
        box.setText(tr("请选择有效救援站!"));
        box.addButton(tr("彳亍口巴"),QMessageBox::AcceptRole);
        if(box.exec()==QMessageBox::Accepted)
        {
            box.close();
        }
    }
    else
    {
        QList<int> pointsList;
        QList<Edge> edgesList;
        bool flag=true;
        flag=pathGraph->Dijkstra(s1, s2, pointsList, edgesList);
        if(flag)
        {
            scene->clear();
            drawEdges(edgesList);
            drawPoints(pointsList);
            QString text="from:";
            for(int i=0; i<pointsList.size(); ++i)
            {
                if(i)
                {
                    text+="   🚑   ";
                }
                text+=pathGraph->getPointName(pointsList[i]);
                qDebug()<<"\n"<<text;
            }
            QTextBrowser* browser=ui->textBrowserRoute;
            browser->clear();
            browser->setText(text);
            QMessageBox box;
            box.setWindowTitle(tr("path"));
            box.setIcon(QMessageBox::Information);
            box.setText(tr("成功查找到相应路径"));
            box.addButton(tr("彳亍口巴"),QMessageBox::AcceptRole);
            if(box.exec()==QMessageBox::Accepted)
            {
                box.close();
            }
        }
        else
        {
            QMessageBox box;
            box.setWindowTitle(tr("path"));
            box.setIcon(QMessageBox::Warning);
            box.setText(tr("暂时无法到达！"));
            box.addButton(tr("彳亍口巴"),QMessageBox::AcceptRole);
            if(box.exec()==QMessageBox::Accepted)
            {
                box.close();
            }
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{
//    MainWindow m;
//    m.show();
//    this->close();
    scene->clear();
    bool flag = pathGraph->NetFromFile(":/datas-rgb.txt");
    if (!flag)
    {
        QMessageBox box;
        box.setWindowTitle(tr("error information"));
        box.setIcon(QMessageBox::Warning);
        box.setText("读取交通图数据错误!\n！");
        box.addButton(tr("彳亍口巴"), QMessageBox::AcceptRole);
        if (box.exec() == QMessageBox::Accepted)
        {
            box.close();
        }
    }
    updateChooseInfo();
    showall();
    if(flag){
    QMessageBox box;
    box.setWindowTitle(tr("path"));
    box.setIcon(QMessageBox::Information);
    box.setText(tr("我觉得ok!\n"));
    box.addButton(tr("彳亍口巴"),QMessageBox::AcceptRole);
    if(box.exec()==QMessageBox::Accepted)
    {
        box.close();
    }
    }
}

void MainWindow::showall()
{

    scene->clear();
    QList<int> pointsList;
    QList<Edge> edgesList;
    pathGraph->getGraph(pointsList,edgesList);
    drawEdges(edgesList);
    drawPoints(pointsList);
}

void MainWindow::on_pushbig_clicked()
{
    ui->graphicsView->scale(1.5,1.5);
}

void MainWindow::on_pushlittle_clicked()
{
    ui->graphicsView->scale(2.0/3,2.0/3);
}


void MainWindow::on_pushButton_3_clicked()
{
    scene->clear();
    bool flag = pathGraph->NetFromFile(":/datas.txt");
    if (!flag)
    {
        QMessageBox box;
        box.setWindowTitle(tr("error information"));
        box.setIcon(QMessageBox::Warning);
        box.setText("读取交通图数据错误!\n！");
        box.addButton(tr("确定"), QMessageBox::AcceptRole);
        if (box.exec() == QMessageBox::Accepted)
        {
            box.close();
        }
    }
    updateChooseInfo();
    showall();
    if(flag){
    QMessageBox box;
    box.setWindowTitle(tr("path"));
    box.setIcon(QMessageBox::Information);
    box.setText(tr("我觉得ok!\n"));
    box.addButton(tr("彳亍口巴"),QMessageBox::AcceptRole);
    if(box.exec()==QMessageBox::Accepted)
    {
        box.close();
    }
    }
}
