#include "profiling.h"
#include <iostream>

Profiling::Profiling(){
    QVBoxLayout *layout = new QVBoxLayout;
    //show list with all modules name,mean,stdev
    moduleTable = new QTableWidget;
    moduleTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    moduleTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    moduleTable->setSelectionMode(QAbstractItemView::SingleSelection);
    moduleTable->verticalHeader()->setVisible(false);
    moduleTable->setShowGrid(false);
    moduleTable->setRowCount(1);
    moduleTable->setColumnCount(4);
    QStringList qlist;
    qlist<<"Module"<<"type"<<"mean in us"<<"std in us";
    moduleTable->setHorizontalHeaderLabels(qlist);
    layout->addWidget(moduleTable);


    //show plot over time

    timeChart = new QtCharts::QChart();
    QtCharts::QSplineSeries *tmp = new QtCharts::QSplineSeries();
    tmp->append(0,10);
    tmp->append(1,12);
    tmp->append(2,9);
    //timeChart->legend()->hide();
    timeChart->addSeries(tmp);
    tmp->setName("tmp");
    timeChart->setTitle("");
    timeChart->createDefaultAxes();
    timeChart->axisY()->setTitleText("Time");
    //timeChart->axisX()->setVisible(false);
    timeChart->axisX()->setTitleText("Iteration");
    QtCharts::QChartView *chartView = new QtCharts::QChartView(timeChart);
    chartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(chartView);
    //TODO set offset timeChart->axisY()->setMax(13);

    setLayout(layout);

    //just for debugging
    setModuleTable(0,"runtime","main",0,0);
}

void Profiling::setModuleTable(int row,QString name,QString type, int mean, int stdev){

    moduleTable->setItem(row, 0, new QTableWidgetItem(name));
    moduleTable->setItem(row, 1, new QTableWidgetItem(type));
    moduleTable->setItem(row, 2, new QTableWidgetItem(mean));
    moduleTable->setItem(row, 3, new QTableWidgetItem(stdev));
}
