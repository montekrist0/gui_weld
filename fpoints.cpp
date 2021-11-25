#include "fpoints.h"

fPoints::fPoints(){
    contSize = 0;
    xCoord = QVector<double>(0);
    yCoord = QVector<double>(0);
}

//fPoints::fPoints(const QVector<double> &X, const QVector<double> &Y, const double leftEdge = -200, const double rightEdge = 200)
//{
//    if (!X.size()) {
//        qDebug() << "insertion failed. initial container is empty";
//        return;
//    }
//    for (int i=0; i<X.size();i++) {
//        if (X[i]>=leftEdge && X[i]<=rightEdge) {
//            xCoord.push_back(X[i]);
//            yCoord.push_back(Y[i]);
//        }
//    }
//    contSize = xCoord.size();
//}

fPoints::fPoints(const QVector<double> &X, const QVector<double> &Y, const int leftEdge, const int rightEdge)
{
    if (!X.size()) {
        qDebug() << "insertion failed. initial container is empty";
        return;
    }
    for (int i=leftEdge; i<rightEdge;i++) {

        xCoord.push_back(X[i]);
        yCoord.push_back(Y[i]);

    }
    contSize = xCoord.size();
}

size_t fPoints::size() const
{
    return xCoord.size();
}

lineData fPoints::linearRegression()
{

    double k=0,b=0;
    double sxy=0, sx=0,sy=0,sxx=0;
    for (size_t i=0;i<contSize;i++) {
        sxy+=xCoord[i]*yCoord[i];
        sx+=xCoord[i];
        sy+=yCoord[i];
        sxx+=xCoord[i]*xCoord[i];
    }
    k=(sxy*contSize-sx*sy)/(sxx*contSize-sx*sx);

    b=(sy-k*sx)/contSize;

//    double sxsr=sx/contSize;
//    double sysr=sy/contSize;
//    double chisl=0;
//    double znam=0;
//    for (size_t i=0;i<contSize;i++) {
//        chisl+=(xCoord[i]-sxsr)*(yCoord[i]-sysr);
//        znam=pow((xCoord[i]-sxsr),2);
//    }
//    k=chisl/znam;
//    b=sysr-k*sxsr;  //конец блока);
    Line.B = b;
    Line.K = k;
    Line.start = QPointF(xCoord[0], k*xCoord[0]+b);
    Line.end = QPointF(xCoord[contSize-1],k*xCoord[contSize-1]+b);
    return Line;
}

lineData fPoints::getLine() const
{
    return Line;
}
