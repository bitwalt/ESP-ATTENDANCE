#ifndef SPECIFIC_SITUATION_H
#define SPECIFIC_SITUATION_H

#include <QDialog>
#include "Database.h"
#include <qcustomplot.h>
#include <QDateTime>

namespace Ui {
class specific_situation;
}

class specific_situation : public QDialog
{
    Q_OBJECT

private:
    Ui::specific_situation *ui;
    QVector<double> qv_x, qv_y;
    int min_x = 0, max_x = 0, min_y = 0, max_y = 0;
    std::string x1, x2, y1, y2;
    Database *db;
    map<long, map<string, Position*>> mappa;
    QCPTextElement *title;

    QDateTime start, end;
    QDate date_start, date_end;
    QString x, y;
    QTime time_start, time_end;

    int num;
    int num_device = 0;
    std::string num_device_str;
    std::string date_title, time_title;

public:
    explicit specific_situation(QWidget *parent = 0);
    ~specific_situation();
    void addPoint(double, double);
    void clearData();
    void plot();

private slots:
    void on_pushButton_clicked();
    void on_horizontalSlider_valueChanged(int);


};

#endif // SPECIFIC_SITUATION_H
