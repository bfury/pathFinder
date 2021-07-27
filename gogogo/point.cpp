#include "point.h"
#include <math.h>

// angles->rad
double rad(double d)
{
    const double PI = 3.1415926535898;
    return d * PI / 180.0;
}
// distanct
/*
 * 球是一个近乎标准的椭球体，它的赤道半径为6378.37千米，极半径为 6356.755千米，平均半径6371.004千米。
 * 如果我们假设地球是一个完美的球体，那么它的半径就是地球的平均半径，记为R。如果以0度经线为基准，那么根据地球表面任意两点的经纬度就可以计算出这两点间的地表距离
 * （这里忽略地球表面地形对计算带来的误差，仅仅是理论上的估算值）。
 * 设第一点A的经 纬度为(LonA, LatA)，第二点B的经纬度为(LonB, LatB)，
 * 按照0度经线的基准，东经取经度的正值(Longitude)，西经取经度负值(-Longitude)，北纬取90-纬度值(90- Latitude)，南纬取90+纬度值(90+Latitude)，
 * 则经过上述处理过后的两点被计为(MLonA, MLatA)和(MLonB, MLatB)。那么根据三角推导，可以得到计算两点距离的如下公式：
 * C=sin(MLatA)*sin(MLatB)*cos(MLonA-MLonB)+cos(MLatA)*cos(MLatB)
 * Distance=R*Arccos(C)*Pi/180
 * 这里，R和Distance单位是相同，如果是采用6371.004千米作为半径，那么Distance就是千米为单位，如果要使用其他单位，比如mile，还需要做单位换算，1千米=0.621371192mile，如果仅对经度作正负的处理，而不对纬度作90-Latitude(假设都是北半球，南半球只有澳洲具有应用意义)的处理，那么公式将是：
 * C=sin(LatA)*sin(LatB)+cos(LatA)*cos(LatB)*cos(MLonA-MLonB)
 * Distance=R*Arccos(C)*Pi/180
 * 以上通过简单的三角变换就可以推出。
 * 如果三角函数的输入和输出都采用弧度值，那么公式还可以写作：
 * C=sin(LatA*Pi/180)*sin(LatB*Pi/180)+cos(LatA*Pi/180)*cos(LatB*Pi/180)*cos((MLonA-MLonB)*Pi/180)
 * Distance=R*Arccos(C)*Pi/180
 * 也就是：
 * C=sin(LatA/57.2958)*sin(LatB/57.2958)+cos(LatA/57.2958)*cos(LatB/57.2958)*cos((MLonA-MLonB)/57.2958)
 * Distance=R*Arccos(C)=6371.004*Arccos(C) kilometer=0.621371192*6371.004*Arccos(C)mile=3958.758349716768*Arccos(C) mile
 */


int calcuDistance(double fLati1, double fLong1, double fLati2, double fLong2)
{
    const double EARTH_RADIUS = 6378.137;

    double radLat1 = rad(fLati1);
    double radLat2 = rad(fLati2);
    double a = radLat1 - radLat2;
    double b = rad(fLong1) - rad(fLong2);
    double s = 2 * asin(sqrt(pow(sin(a/2),2) + cos(radLat1)*cos(radLat2)*pow(sin(b/2),2)));
    s = s * EARTH_RADIUS;
    s = (int)(s * 10000000) / 10000;
    return s;
}

double Point::minLongitude = 200;
double Point::minLatitude = 200;
double Point::maxLongitude = 0;
double Point::maxLatitude = 0;


Point::Point(){}


Point::Point(QString nameStr, double longi, double lati, QList<int> roadsList):
    name(nameStr), longitude(longi), latitude(lati)
{
    roadsInfo=roadsList.toSet();
}

//get distanct (a to other)
int Point::distance(Point other)
{
    return calcuDistance(latitude, longitude, other.latitude, other.longitude);
}
