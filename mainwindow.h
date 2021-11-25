#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma once
#include <qcustomplot.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <fpoints.h>
#include <utility>
#include <math.h>
#include <algorithm>

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_addRandomGraph_clicked();
    void addRandomGraph();
    void on_selectRegion_clicked();
    void slotSelRec(const QRect &rect, QMouseEvent *event);
    void slotSelRec2(QMouseEvent *event);
    void slotSelRec3(QMouseEvent *event);
    void contextMenuRequest(QPoint pos);
    void removeSelectedGraph();
    void on_removeSelectionRegion_clicked();
    void removeAllGraphs();
    void on_zoomProfile_clicked();

    void on_addThreadProfile_clicked();
    void actionClicked();
    void selectKeyPointsClicked();
    void buildApproxClicked();
    void removeKeyPointsClicked();
    void removeAllClicked();
    void removeLinesClicked();
    void calculateParamsClicked();
    void on_spinBox_valueChanged(int arg1);

    void on_doubleSpinBox_valueChanged(double arg1);

    void on_checkBox_stateChanged(int arg1);

    void on_minNumOfPointsInGroup_valueChanged(int arg1);

    void on_comboBox_2_currentIndexChanged(const QString &arg1);

private:
    friend class fPoints;
    Ui::MainWindow *ui;
    QCustomPlot* myplot;
    bool LinesWereBuild;
    ///////////////////////////////////////////
    void loadData(std::string name);
    void setPainter();
    void plotThreadPoints();
    void addItem(QCPItemRect &rec);
    void setSelectedItems();
    void setInterface1();
    QVector<QVector<QPointF>> filterPoints(QVector<double> X, QVector<double> Y);
    std::string fileName;
    QCPSelectionRect* selRec;
    QVector<QCPItemRect*> areas;
    QVector<double> X,Y, filteredX, filteredY;
    QCPScatterStyle* myScatter;
    QCPScatterStyle* selectedScatter;
    QCPItemRect* rect = nullptr;
    QCPGraph* threadProfile;
    QCPSelectionDecorator* selDec;
    fPoints bevelGrooveProfile;
    QVector<size_t> intersectionPointIndexes;
    QVector<fPoints> groovePoints;
    QVector<QCPItemEllipse *> keyPointsEllipses;
    QVector<QCPItemLine*> approxLines;
    QVector<size_t> GetIntersectionPoint(const QVector <double> &X,const QVector <double> &Y);
    ////////////////////////////////////////////

};
#endif // MAINWINDOW_H
