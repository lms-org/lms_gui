#ifndef QT_VISULATION_H
#define QT_VISULATION_H

#include "main_window.h"
#include <QtCharts/QChartView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QSplineSeries>
#include <data_collector.h>


/**
 * @brief LMS module qt_visulation
 **/
class QtVisulation{
    int my_cycleCounter;
    //Gui
    MainWindow* mainWindow;
    DataCollector* dataCollector;
public:
    bool initialize();
    bool deinitialize();
    bool cycle();

    int cycleCounter(){
        return my_cycleCounter;
    }
};

#endif // QT_VISULATION_H
