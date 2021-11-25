#ifndef FPOINTS_H
#define FPOINTS_H
#pragma once
//#include <mainwindow.h>
#include <QVector>
#include <QPointF>
#include <math.h>
#include <QDebug>

struct lineData {
    double K = 0;
    double B = 0;
    QPointF start = QPointF(0,0);
    QPointF end = QPointF(0,0);
};

class fPoints
{
private:

    lineData Line;
    QVector<double> xCoord, yCoord;

    size_t contSize;
public:
    fPoints();
    QVector<double> getX() const {
        return xCoord;
    };
    QVector<double> getY() const {
        return yCoord;
    };
//    fPoints(const QVector<double> &X, const QVector<double> &Y, const double leftEdge, const double rightEdge);
    fPoints(const QVector<double> &X, const QVector<double> &Y, const int leftEdge, const int rightEdge);
    void copyData();
    size_t size() const;
    lineData linearRegression();
    lineData getLine() const;
};

#endif // FPOINTS_H
