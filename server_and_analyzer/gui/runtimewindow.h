#pragma once
#include <QMainWindow>
#include <map>
#include <qcustomplot.h>
#include <sys/time.h>
#include <algorithm>
#include <QEvent>
#include <QMouseEvent>
#include "ui_runtimewindow.h"
#include "dialog.h"
#include "specific_situation.h"
#include "frequent_devices.h"
#include <Position.h>
#include "Server.h"

namespace Ui {
class RuntimeWindow;
}

class RuntimeWindow : public QMainWindow
{
    Q_OBJECT

private:
    QCPTextElement *title;
    QVector<double> qv_x, qv_y, esp_x, esp_y, qv_x2, qv_y2;
    vector<Position*> pos;

    int min_x = 0, max_x = 0, min_y = 0, max_y = 0;
    struct timeval tv;
    map<string , Position*> mac_position;
    map<long, int> storico_n_disp ;
    int timestamp, number_devices;
    QString device_conn, error;

public:

    Ui::RuntimeWindow *uirw;
    explicit RuntimeWindow(QWidget *parent = 0);
    ~RuntimeWindow();

    void addPoint(double x, double y);
    void addPoint2(long x, int y);
    void addEspPoint(double,double);
    void clearData();
    void plot_current_situation();
    void plot_esp_position();
    void plot_attendance();
    void setMac_position(const map<string, Position *> &value);
    void setStorico_n_disp(const map<long, int> &value);
    void setTimestamp(int value);
    void setDevice_conn(const QString &value);
    void setNumber_devices(int value);
    void setPos(const vector<Position *> &value);
    void setError(const QString &value);

public slots:
    void on_frequent_devices_pushButton_clicked();
    void on_specific_situation_pushButton_2_clicked();
    void SLOT_plot1();
    void SLOT_plot2();
};

