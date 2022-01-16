#include "mainwindow.h"
#include "ui_mainwindow.h"


#define RangeX(axes) axes[0]-deltaX,axes[axes.size()-1]+deltaX
#define RangeY(axes) axes[0]-deltaY,axes[axes.size()-1]+deltaY


float GetAngle (double a1,double a2) {
    double k=abs((a2-a1)/(1+a1*a2));
    return abs(atan(k)*180/3.14159);
}

float GetDisplacement (double x0,double y0, double k, double b) {
    return abs((-1)*k*x0+y0-b)/sqrt(k*k+1);
}

double GetSizeOfLine (double x1,double y1,double x2,double y2) {
    double dx=(x2-x1);
    double dy=(y2-y1);
    return sqrt(dx*dx+dy*dy);
}

double getCoef (const QVector <double> &X,const QVector <double> &Y, const size_t &c=0, const size_t &step=20) {
    //vector <double> result;
    double k=0;//,b=0;
    double sxy=0, sx=0,sy=0,sxx=0;
    for (size_t i=c;i<c+step;i++) {
        sxy+=X[i]*Y[i];
        sx+=X[i];
        sy+=Y[i];
        sxx+=X[i]*X[i];

    }

    k=(sxy*step-sx*sy)/(sxx*step-sx*sx);

    return k;
}

QVector<size_t> MainWindow::GetIntersectionPoint(const QVector <double> &X,const QVector <double> &Y){
    /*реализация скрыта*/
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    myplot = ui->customPlot;
    myplot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                          QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iSelectItems);
    fileName = "37.txt";
    loadData(fileName);
    setInterface1();
    threadProfile = nullptr;

//    plotThreadPoints();
    ui->customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
    connect(ui->action, &QAction::triggered, this, &MainWindow::actionClicked);
    connect(ui->selectKeyPoints, &QAction::triggered, this, &MainWindow::selectKeyPointsClicked);
    connect(ui->buildApproxLines, &QAction::triggered, this, &MainWindow::buildApproxClicked);
    connect(ui->removePointsLines, &QAction::triggered, this, &MainWindow::removeAllClicked);
    connect(ui->removeLines, &QAction::triggered, this, &MainWindow::removeLinesClicked);
    connect(ui->removeKeyPoints, &QAction::triggered, this, &MainWindow::removeKeyPointsClicked);
    connect(ui->calculateParams, &QAction::triggered, this, &MainWindow::calculateParamsClicked);
    //    calculateParams
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setInterface1() {
    ui->grooveAngle->setText("no data");
    ui->edgeDisplacement->setText("no data");
    ui->gapValue->setText("no data");
    myplot->axisRect()->setupFullAxesBox(); //делает график ограниченным прямоугольником, а не двумя осями
    /*Задаем название виджету*/
    myplot->plotLayout()->insertRow(0);
    QCPTextElement *title = new QCPTextElement(ui->customPlot, "Профиль разделки кромок",
                                               QFont("sans", 17, QFont::Bold));
    myplot->plotLayout()->addElement(0, 0, title);
    /*Задаем название виджету*/

    /*Задаем названия осей*/
    myplot->xAxis->setLabel("x Axis");
    myplot->yAxis->setLabel("y Axis");
    /*Задаем названия осей*/

    /*Задание легенды*/
    myplot->legend->setVisible(true);//делает видимым маленькое окошко в правом верхнем углу виджета, где будут отображаться графики
    QFont legendFont = font();
    legendFont.setPointSize(10);
    myplot->legend->setFont(legendFont);
    myplot->legend->setSelectedFont(legendFont);
    myplot->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

}

void MainWindow::addRandomGraph()
{
    int n = 50; // number of points in graph
    double xScale = (std::rand()/(double)RAND_MAX + 0.5)*2;
    double yScale = (std::rand()/(double)RAND_MAX + 0.5)*2;
    double xOffset = (std::rand()/(double)RAND_MAX - 0.5)*4;
    double yOffset = (std::rand()/(double)RAND_MAX - 0.5)*10;
    double r1 = (std::rand()/(double)RAND_MAX - 0.5)*2;
    double r2 = (std::rand()/(double)RAND_MAX - 0.5)*2;
    double r3 = (std::rand()/(double)RAND_MAX - 0.5)*2;
    double r4 = (std::rand()/(double)RAND_MAX - 0.5)*2;
    QVector<double> x(n), y(n);
    for (int i=0; i<n; i++)
    {
        x[i] = (i/(double)n-0.5)*10.0*xScale + xOffset;
        y[i] = (qSin(x[i]*r1*5)*qSin(qCos(x[i]*r2)*r4*3)+r3*qCos(qSin(x[i])*r4*2))*yScale + yOffset;
    }

    myplot->addGraph();
    myplot->graph()->setName(QString("New graph %1").arg(myplot->graphCount()-1));
    myplot->graph()->setData(x, y);
    myplot->graph()->setLineStyle((QCPGraph::LineStyle)(std::rand()%5+1));
    if (std::rand()%100 > 50)
        myplot->graph()->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)(std::rand()%14+1)));
    QPen graphPen;
    graphPen.setColor(QColor(std::rand()%245+10, std::rand()%245+10, std::rand()%245+10));
    graphPen.setWidthF(std::rand()/(double)RAND_MAX*2+1);
    myplot->graph()->setPen(graphPen);
    myplot->replot();
}

void MainWindow::loadData(std::string name)
{
    if (!X.empty() && !Y.empty()) {
        X.clear();
        Y.clear();
    }
    double x=0, y=0;
    std::string line;
    std::stringstream ss;
    std::ifstream input(name);

    if (input.is_open()) {
        while (getline(input,line)) {
            ss << line;
            ss >> x >> y;
            X.push_back(x);
            Y.push_back(y);
            ss.clear();
        }
    }
    else qDebug() << "file is not opened";
    input.close();

}



void MainWindow::setPainter()
{
    myScatter = new QCPScatterStyle;
    myScatter->setShape(QCPScatterStyle::ssCircle);
    myScatter->setPen(QPen(Qt::blue));
    myScatter->setBrush(Qt::blue);
    myScatter->setSize(2);
}

void MainWindow::plotThreadPoints()
{

    setPainter();
    setSelectedItems();

    /*Добавляем график на виджет - начало*/
    threadProfile = myplot->addGraph();
    threadProfile->setData(X,Y);
    threadProfile->setScatterStyle(*myScatter);//устанавливаем тип точек
    threadProfile->setLineStyle(QCPGraph::lsNone);//устанавливаем как будут соединяться точки
    //    on_zoomProfile_clicked();
    threadProfile->setSelectable(QCP::stDataRange);
    threadProfile->setSelectionDecorator(selDec);
    threadProfile->setName("Welding groove");
    on_zoomProfile_clicked();
    myplot->replot();
}

void MainWindow::addItem(QCPItemRect &rec)
{
    areas.push_back(&rec);
}

void MainWindow::setSelectedItems()
{
    selDec = new QCPSelectionDecorator;
    selectedScatter = new QCPScatterStyle;
    selectedScatter->setShape(QCPScatterStyle::ssCircle);
    selectedScatter->setPen(QPen(Qt::red));
    selectedScatter->setBrush(Qt::white);
    selectedScatter->setSize(3);
    selDec->setScatterStyle(*selectedScatter, QCPScatterStyle::spAll);

}

void MainWindow::on_addRandomGraph_clicked()
{
    addRandomGraph();
}

void MainWindow::on_selectRegion_clicked()
{
    ui->selectRegion->setEnabled(0);
    if (rect!=nullptr) {
        myplot->removeItem(rect);
        rect = nullptr;
        myplot->replot();
    }
    myplot->setSelectionRectMode(QCP::srmSelect);
    myplot->setCursor(Qt::CursorShape::CrossCursor);
    connect(myplot->selectionRect(), &QCPSelectionRect::accepted, this, &MainWindow::slotSelRec);
}

void MainWindow::slotSelRec(const QRect &rectangle, QMouseEvent *event) {
    //    connect(ui->selectRegion, ui->selectRegion->clicked(), this, &MainWindow::on_selectRegion_clicked);
    /*    Q_UNUSED(rect);*/
    Q_UNUSED(event);
    myplot->setCursor(Qt::CursorShape::ArrowCursor);
    myplot->setSelectionRectMode(QCP::srmNone);
    myplot->selectionRect()->disconnect();

    rect = new QCPItemRect(ui->customPlot);
    rect->setSelectable(true);
    rect->setSelectedPen(QPen(Qt::red));
    double xleft = myplot->xAxis->pixelToCoord(rectangle.topLeft().x());
    double yleft = myplot->yAxis->pixelToCoord(rectangle.topLeft().y());
    double xright = myplot->xAxis->pixelToCoord(rectangle.bottomRight().x());
    double yright = myplot->yAxis->pixelToCoord(rectangle.bottomRight().y());
    rect->topLeft->setCoords(xleft,yleft);
    rect->bottomRight->setCoords(xright,yright);
    myplot->replot();
    ui->selectRegion->setEnabled(1);


    if (myplot->selectedGraphs().size()) {
        QCPGraph* selGraph = myplot->selectedGraphs().at(0);
        QCPDataSelection selection = selGraph->selection();
        double sum = 0;
        QList<QCPDataRange> dRanges = selection.dataRanges();//.at(0);
        qDebug()<< "QCPDataRange container size is equal to " << dRanges.size();

        foreach (QCPDataRange dataRange, selection.dataRanges())
        {
            QCPGraphDataContainer::const_iterator begin = selGraph->data()->at(dataRange.begin()); // get range begin iterator from index
            QCPGraphDataContainer::const_iterator end = selGraph->data()->at(dataRange.end()); // get range end iterator from index
            for (QCPGraphDataContainer::const_iterator it=begin; it!=end; ++it)
            {
                // iterator "it" will go through all selected data points, as an example, we calculate the value average
                sum += it->value;
            }
        }
        double average = sum/selection.dataPointCount();
        qDebug()<< average;

    }

}

void MainWindow::on_removeSelectionRegion_clicked()
{
    if (rect!=nullptr) {
        myplot->removeItem(rect);
        rect = nullptr;
        myplot->replot();
    }
    ui->selectRegion->setEnabled(1);
}

void MainWindow::removeSelectedGraph()
{
    if (ui->customPlot->selectedGraphs().size() > 0)
    {
        if (myplot->selectedGraphs().first() == threadProfile) threadProfile = nullptr;
        ui->customPlot->removeGraph(ui->customPlot->selectedGraphs().first());
        ui->customPlot->replot();
    }
}

void MainWindow::removeAllGraphs()
{
    ui->customPlot->clearGraphs();
    threadProfile = nullptr;
    ui->customPlot->replot();
}

void MainWindow::contextMenuRequest(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addAction("Add random graph", this, SLOT(addRandomGraph()));
    if (ui->customPlot->selectedGraphs().size() > 0)
        menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
    if (ui->customPlot->graphCount() > 0)
        menu->addAction("Remove all graphs", this, SLOT(removeAllGraphs()));
    menu->popup(ui->customPlot->mapToGlobal(pos));
}

void MainWindow::on_zoomProfile_clicked()
{
    double minYpos = *std::min_element(Y.begin(), Y.end());
    double maxYpos = *std::max_element(Y.begin(), Y.end());
    double deltaX = X[X.size()-1] - X[0];
    double midY =  (maxYpos + minYpos)/2;
    double koef = double(myplot->axisRect()->width())/myplot->axisRect()->height();
    QCPRange yRange(midY - deltaX/(koef*2), midY + deltaX/(koef*2));
    QCPRange xRange(X[0], X[X.size()-1]);
    myplot->xAxis->setRange(xRange);
    myplot->yAxis->setRange(yRange);
    myplot->replot();
}

void MainWindow::slotSelRec2(QMouseEvent *event) {
    Q_UNUSED(event);
    //    qDebug()<< "Im here slotSelRec2 " << i++;
    myplot->setSelectionRectMode(QCP::srmNone);
    //    if (selRec->isActive()) {
    //        qDebug()<< "Selecting Rectangle is Active";
    //        qDebug()<< "Position end: " << event->pos().x();
    //    }
    //    else if (QApplication::mouseButtons()) qDebug()<< "Selecting Rectangle is not Active " << i++;
    //    //    delete selRec;
}

void MainWindow::slotSelRec3(QMouseEvent *event) {
    Q_UNUSED(event);
    //    qDebug()<< "Im here slotSelRec3";
    myplot->setSelectionRectMode(QCP::srmNone);

}

void MainWindow::on_addThreadProfile_clicked()
{
    if (!threadProfile) plotThreadPoints();
    else QMessageBox::critical(this,"Профиль построен", "Смотри внимательнее");//qDebug()<<"Profile of thread is already built";

}

void MainWindow::actionClicked()
{
    QCPItemLine* line = new QCPItemLine(myplot);
    qDebug()<<"action was clicked";
    QPen p(Qt::red);
    line->setPen(p);
    line->start->setCoords(24,24);
    line->end->setCoords(120,120);
    myplot->replot();
}

void MainWindow::selectKeyPointsClicked()
{
    intersectionPointIndexes = GetIntersectionPoint(X,Y);

    intersectionPointIndexes.push_front(0);
    intersectionPointIndexes.push_back(X.size()-1);

    QPen pen(Qt::red);
    pen.setWidth(2);

    for (auto x:intersectionPointIndexes) {
        keyPointsEllipses.push_back(new QCPItemEllipse(myplot));
        keyPointsEllipses.back()->setPen(pen);
        keyPointsEllipses.back()->topLeft->setCoords(X[x]-1,Y[x]+1);
        keyPointsEllipses.back()->bottomRight->setCoords(X[x]+1,Y[x]-1);
    }
    myplot->replot();

}

void MainWindow::buildApproxClicked()
{
//    if (LinesWereBuild) return;
    if (!intersectionPointIndexes.size()) {

        selectKeyPointsClicked();
    }
    switch (ui->comboBox->currentIndex()) {
    case 0:
        if (intersectionPointIndexes.size()<5) {
            qDebug() << "недостаточно точек для построения всех прямых";
            ui->grooveAngle->setText("no data");
            ui->edgeDisplacement->setText("no data");
            ui->gapValue->setText("no data");
        }
        else if (intersectionPointIndexes.size()>5) {
            qDebug() << "присутствуют лишние точки";
            ui->grooveAngle->setText("no data");
            ui->edgeDisplacement->setText("no data");
            ui->gapValue->setText("no data");
        }
        else {
            for (size_t i = 0; i<4; i++) {
                fPoints points(X,Y,intersectionPointIndexes[i]+5,intersectionPointIndexes[i+1]-5);
                groovePoints.push_back(points);
            }

            for (size_t i = 0; i<4; i++) {
                int r = 255 - i*50;
                int g = i*70;
                int b = 255/(i+1);
                QPen pen(QColor(r,g,b));
                pen.setWidth(3);
                approxLines.push_back(new QCPItemLine(myplot));
                approxLines.back()->start->setCoords(groovePoints[i].linearRegression().start);
                approxLines.back()->end->setCoords(groovePoints[i].linearRegression().end);
                approxLines.back()->setPen(pen);
            }
            LinesWereBuild = true;
        }

        break;
    case 1:
        qDebug()<<"alghorithm is not written";
        break;
    case 2:
        qDebug()<<"alghorithm is not written";
        break;
    case 3:
        qDebug()<<"alghorithm is not written";
        break;
    default:
        qDebug()<<"error";
    }
    calculateParamsClicked();
    myplot->replot();
}

void MainWindow::removeKeyPointsClicked()
{
    removeAllClicked();
}

void MainWindow::removeAllClicked()
{
//    for (auto x:myplot->grap)
    for (auto x:approxLines) {
        myplot->removeItem(x);
    }
    for (auto y:keyPointsEllipses) {
        myplot->removeItem(y);
    }
    intersectionPointIndexes.clear();
    groovePoints.clear();
    approxLines.clear();
    keyPointsEllipses.clear();
    LinesWereBuild = false;
    myplot->replot();
}

void MainWindow::removeLinesClicked()
{
    ui->grooveAngle->setText("no data");
    ui->edgeDisplacement->setText("no data");
    ui->gapValue->setText("no data");
    if (groovePoints.size()) {
        for (auto x:approxLines) {
            myplot->removeItem(x);
        }
        approxLines.clear();
        groovePoints.clear();
    }
    LinesWereBuild = false;
    myplot->replot();
}

void MainWindow::calculateParamsClicked()
{
    switch (ui->comboBox->currentIndex()) {
    case 0:
        if (groovePoints.size()==4) {
            ui->grooveAngle->setText(QString::number(GetAngle(groovePoints[1].getLine().K, groovePoints[2].getLine().K)));
            ui->edgeDisplacement->setText(
                        QString::number(
                            std::max(GetDisplacement(groovePoints[3].getX().first(),groovePoints[3].getY().first(),groovePoints[0].getLine().K, groovePoints[0].getLine().B),
                    GetDisplacement(groovePoints[0].getX().last(),groovePoints[0].getY().last(),groovePoints[3].getLine().K, groovePoints[3].getLine().B)
                    ) ));
        }
        else {
            ui->grooveAngle->setText("no data");
            ui->edgeDisplacement->setText("no data");
            ui->gapValue->setText("no data");
        }

        break;
    case 1:

        break;
    case 2:

        break;
    case 3:

        break;
    default:
        qDebug()<<"error";
    }
}



void MainWindow::on_spinBox_valueChanged(int arg1)
{
    if (keyPointsEllipses.size()) {
        for (auto x:keyPointsEllipses)
            myplot->removeItem(x);
        keyPointsEllipses.clear();
        intersectionPointIndexes.clear();
        selectKeyPointsClicked();
    }

    if (LinesWereBuild) {
        if (approxLines.size()) {
            for (auto x:approxLines) {
                myplot->removeItem(x);
            }
            approxLines.clear();
            groovePoints.clear();
        }
        buildApproxClicked();
    }

}

void MainWindow::on_doubleSpinBox_valueChanged(double arg1)
{

    if (keyPointsEllipses.size()) {
        for (auto x:keyPointsEllipses)
            myplot->removeItem(x);
        keyPointsEllipses.clear();
        intersectionPointIndexes.clear();
        selectKeyPointsClicked();
    }

    if (LinesWereBuild) {
        if (approxLines.size()) {
            for (auto x:approxLines) {
                myplot->removeItem(x);
            }
            approxLines.clear();
            groovePoints.clear();
        }
        buildApproxClicked();
    }

}

QVector<QVector<QPointF>> MainWindow::filterPoints(QVector<double> X, QVector<double> Y) {
    QVector<QVector<QPointF>> result;

    for (int i=0; i<X.size()-1;i++) {
        if (abs(Y[i+1]-Y[i])<0.3) {
            QVector<QPointF> curGroupXY;
            while (i<X.size()-2 && abs(Y[i+1]-Y[i])<0.3) {
                curGroupXY.push_back({X[i],Y[i]});
                i++;
            }
            if (curGroupXY.size()>ui->minNumOfPointsInGroup->value()) result.push_back(curGroupXY);
        }
        else {
            QVector<QPointF> curGroupXY;
            while (i<X.size()-2 && abs(Y[i+1]-Y[i])<0.3) {
                curGroupXY.push_back({X[i],Y[i]});
                i++;
            }
            if (curGroupXY.size()>ui->minNumOfPointsInGroup->value()) result.push_back(curGroupXY);
        }
    }
    return result;
}

double averageValue(QVector<QPointF> Y)
{
    double sum = 0;
    for (auto x:Y) {
        sum+=x.y();
    }
    return sum/Y.size();
}


void MainWindow::on_checkBox_stateChanged(int arg1)
{
    removeAllGraphs();
    if (ui->checkBox->isChecked()) {
        QVector<QVector<QPointF>> groupsOfPoints = filterPoints(X,Y);
        QVector<QVector<QPointF>> filteredGroups;
        for (int i=1; i<groupsOfPoints.size()-1;i++) {
            double avLeft = averageValue(groupsOfPoints[i-1]);
            double avMid = averageValue(groupsOfPoints[i]);
            double avRight = averageValue(groupsOfPoints[i+1]);
            if (((avMid>avLeft && avMid>avRight) || (avMid<avLeft && avMid<avRight))
                    && abs(groupsOfPoints[i-1].last().x()-groupsOfPoints[i+1].first().x())>0.3) {
                filteredGroups.push_back(groupsOfPoints[i-1]);
                filteredGroups.push_back(groupsOfPoints[i+1]);
            }
        }
        X.clear();
        Y.clear();
        for (auto x:groupsOfPoints/*filteredGroups*/) {
            for (auto y:x) {
                X.push_back(y.x());
                Y.push_back(y.y());
            }

        }
    }
    else (loadData(fileName));
    plotThreadPoints();
}

void MainWindow::on_minNumOfPointsInGroup_valueChanged(int arg1)
{
    if (ui->checkBox->isChecked()) {
        loadData(fileName);
        on_checkBox_stateChanged(arg1);
        on_spinBox_valueChanged(0);
        myplot->replot();

    }
}

void MainWindow::on_comboBox_2_currentIndexChanged(const QString &arg1)
{
    removeAllClicked();
    myplot->clearGraphs();
    loadData(arg1.toStdString());
    plotThreadPoints();
    if (ui->checkBox->isChecked()) {
        on_checkBox_stateChanged(0);
    }
}
