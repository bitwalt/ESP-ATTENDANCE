#ifndef FREQUENT_DEVICES_H
#define FREQUENT_DEVICES_H

#include <QDialog>
#include "Database.h"
#include <QDateTime>

namespace Ui {
class frequent_devices;
}

class frequent_devices : public QDialog
{
    Q_OBJECT

public:
    explicit frequent_devices(QWidget *parent = 0);
    ~frequent_devices();

private slots:
    void on_pushButton_clicked();

private:
    Ui::frequent_devices *ui;
    Database *db;

    std::string x1, x2, y1, y2;
    QDateTime start, end;
    QDate date_start, date_end;
    QString x, y;
    QTime time_start, time_end;
    int n_limit;
    double n_limit_double;
};

#endif // FREQUENT_DEVICES_H
