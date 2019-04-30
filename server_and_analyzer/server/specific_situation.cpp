#include "specific_situation.h"
#include "ui_specific_situation.h"
#include <map>
#include "Position.h"
#include <QString>
#include <algorithm>
#include "error_specific_situation.h"

specific_situation::specific_situation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::specific_situation)
{
    ui->setupUi(this);

    ui->plot->setLocale(QLocale(QLocale::English, QLocale::Italy));

    ui->plot->plotLayout()->insertRow(0);
    title = new QCPTextElement(ui->plot, " ", 20);
    ui->plot->plotLayout()->addElement(0, 0, title);
    ui->plot->addGraph();
    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
    ui->plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->plot->xAxis->setLabel("X-coordinate");
    ui->plot->yAxis->setLabel("Y-coordinate");

    ui->dateTimeEdit_start->setFocus();

    this->setWindowTitle("Show specific situation in the past");

    db = new Database(true);
}

void specific_situation::on_pushButton_clicked()
{
    start = ui->dateTimeEdit_start->dateTime();
    end = ui->dateTimeEdit_end->dateTime();

    if (start > end) {
        error_specific_situation *e = new error_specific_situation();
        e->show();
    } else {

        date_start = start.date();
        x = date_start.toString("dd/MM/yyyy");
        x1 = x.toStdString();

        time_start = start.time();
        y = time_start.toString("hh:mm:ss");
        y1 = y.toStdString();

        date_end = end.date();
        x = date_end.toString("dd/MM/yyyy");
        x2 = x.toStdString();

        time_end = end.time();
        y = time_end.toString("hh:mm:ss");
        y2 = y.toStdString();

        db->get_specific_situation(x1, y1, x2, y2, mappa);

        num = (int)mappa.size();

        ui->horizontalSlider->setMinimum(0);
        ui->horizontalSlider->setMaximum(num - 1);
        ui->horizontalSlider->setTickPosition(ui->horizontalSlider->TicksBothSides);
        ui->horizontalSlider->setTickInterval(num - 1);

        for (auto &t : mappa) {
            long timestamp = t.first; //prendo il timestamp
            QDateTime date_and_time;
            date_and_time.setTime_t(timestamp);

            QDate date = date_and_time.date();
            QTime time = date_and_time.time();

            QString tmp = date.toString("dd/MM/yyyy");
            date_title = tmp.toStdString();
            tmp = time.toString("hh:mm");
            time_title = tmp.toStdString();

            map<string, Position*> mac_pos_tmp = t.second;
            clearData();
            for(auto &m_p : mac_pos_tmp)
                specific_situation::addPoint(m_p.second->getX(), m_p.second->getY());
            break;
        }

        std::string str = "Situation at " + date_title + " " + time_title + " - " + std::to_string(qv_x.length()) + " devices";
        title->setText(QString::fromStdString(str));
        ui->plot->replot();

    }
}

void specific_situation::on_horizontalSlider_valueChanged(int value)
{
    clearData();

    int k = 0;
    for (auto &t : mappa) {
        if (k == value) {
            long timestamp = t.first; //prendo il timestamp
            QDateTime date_and_time;
            date_and_time.setTime_t(timestamp);

            QDate date = date_and_time.date();
            QTime time = date_and_time.time();

            QString tmp = date.toString("dd/MM/yyyy");
            date_title = tmp.toStdString();
            tmp = time.toString("hh:mm");
            time_title = tmp.toStdString();

            map<string, Position*> mac_pos_tmp = t.second;
            for(auto &m_p : mac_pos_tmp)
                specific_situation::addPoint(m_p.second->getX(), m_p.second->getY());
            break;
        }
        k++;
    }

    std::string str = "Situation at " + date_title + " " + time_title + " - " + std::to_string(qv_x.length()) + " devices";
    title->setText(QString::fromStdString(str));
    ui->plot->replot();
}

void specific_situation::addPoint(double x, double y)
{
    qv_x.append(x);
    qv_y.append(y);

    min_x = *std::min_element(qv_x.begin(), qv_x.end());
    max_x = *std::max_element(qv_x.begin(), qv_x.end());
    ui->plot->xAxis->setRange(0, max_x + 5);

    min_y = *std::min_element(qv_y.begin(), qv_y.end());
    max_y = *std::max_element(qv_y.begin(), qv_y.end());
    ui->plot->yAxis->setRange(0, max_y + 5);

    plot();
}

void specific_situation::clearData()
{
    qv_x.clear();
    qv_y.clear();
}

void specific_situation::plot()
{
    ui->plot->graph(0)->setData(qv_x, qv_y);
    ui->plot->replot();
    ui->plot->update();
}

specific_situation::~specific_situation()
{
    delete db; //a logica questo ci dovrebbe essere
    delete ui;
}
