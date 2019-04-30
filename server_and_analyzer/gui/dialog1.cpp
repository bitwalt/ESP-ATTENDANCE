#include "dialog1.h"
#include "ui_dialog1.h"
#include "runtimewindow.h"
#include "ui_runtimewindow.h"
#include "systhread.h"
#include <QString>
#include "datastream.h"

#define PORT_NUMBER 5000

extern RuntimeWindow* rwp;
extern datastream *d;

Dialog1::Dialog1(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog1)
{
    ui->setupUi(this);

    ui->title_label->setAlignment(Qt::AlignCenter);

    ui->label->hide();
    ui->distance->hide();
    ui->ok_2->hide();
    ui->label_2->hide();
    ui->label_3->hide();
    ui->label_4->hide();
    ui->label_5->hide();
    ui->label_6->hide();
    ui->label_7->hide();
    ui->x2->hide();
    ui->y2->hide();
    ui->x3->hide();
    ui->y3->hide();
    ui->label_8->hide();
    ui->label_9->hide();
    ui->label_10->hide();
    ui->length->hide();
    ui->width->hide();
    ui->ok_3->hide();
    ui->ok_4->hide();
    ui->pushButton->hide();
}

Dialog1::~Dialog1()
{
    delete ui;
}

void Dialog1::on_radioButton_clicked() //2 Esp
{
    ui->label->show();
    ui->distance->show();
    ui->ok_2->show();

    ui->label_2->hide();
    ui->label_3->hide();
    ui->label_4->hide();
    ui->label_5->hide();
    ui->label_6->hide();
    ui->label_7->hide();
    ui->x2->hide();
    ui->y2->hide();
    ui->x3->hide();
    ui->y3->hide();
    ui->label_8->hide();
    ui->label_9->hide();
    ui->label_10->hide();
    ui->length->hide();
    ui->width->hide();
    ui->ok_3->hide();
    ui->ok_4->hide();

    for (int i = 1; i <= 30; i++)
        ui->distance->addItem(QString::fromStdString(std::to_string(i)));
}

void Dialog1::on_ok_2_clicked()
{
    ui->distance->setEnabled(false);
    QString x_str = ui->distance->currentText();
    int x = x_str.toInt();
    max_x = x;
    max_y = 2;

    f = fopen("config.txt", "w");
    fprintf(f, "Number of ESP modules: %d\n", 2);
    fprintf(f, "PORT NUMBER: %d\n", PORT_NUMBER);

    fprintf(f, "%d %d\n%d %d\n", 0, 0, x, 0);
    fclose(f);

    ui->config->clear();

    std::string str = "Number of ESPs: 2\nCoordinates:\n - (0, 0)\n - (";
    str.append(std::to_string(x)).append(", 0)");
    QString qstr = QString::fromStdString(str);

    ui->config->setText(qstr);
    ui->pushButton->show();
}

void Dialog1::on_radioButton_2_clicked() //3 Esp
{
    ui->label_2->show();
    ui->label_3->show();
    ui->label_4->show();
    ui->label_5->show();
    ui->label_6->show();
    ui->label_7->show();
    ui->x2->show();
    ui->y2->show();
    ui->x3->show();
    ui->y3->show();
    ui->ok_3->show();

    ui->label->hide();
    ui->distance->hide();
    ui->label_8->hide();
    ui->label_9->hide();
    ui->label_10->hide();
    ui->length->hide();
    ui->width->hide();
    ui->ok_2->hide();
    ui->ok_4->hide();

    for (int i = 0; i <= 30; i++) {
        ui->x2->addItem(QString::fromStdString(std::to_string(i)));
        ui->y2->addItem(QString::fromStdString(std::to_string(i)));
        ui->x3->addItem(QString::fromStdString(std::to_string(i)));
        ui->y3->addItem(QString::fromStdString(std::to_string(i)));
    }

}

void Dialog1::on_x2_currentIndexChanged(int index)
{
    if (index != 0)
        ui->y2->setCurrentIndex(0);
}

void Dialog1::on_y2_currentIndexChanged(int index)
{
    if (index != 0)
        ui->x2->setCurrentIndex(0);
}

void Dialog1::on_ok_3_clicked()
{
    QString x2_str = ui->x2->currentText();
    int x2 = x2_str.toInt();

    QString y2_str = ui->y2->currentText();
    int y2 = y2_str.toInt();

    QString x3_str = ui->x3->currentText();
    int x3 = x3_str.toInt();

    QString y3_str = ui->y3->currentText();
    int y3 = y3_str.toInt();

    if (x2 > x3)
        max_x = x2;
    else
        max_x = x3;

    if (y2 > y3)
        max_y = y2;
    else
        max_y = y3;

    f = fopen("config.txt", "w");
    fprintf(f, "Number of ESP modules: %d\n", 3);
    fprintf(f, "PORT NUMBER: %d\n", PORT_NUMBER);
    fprintf(f, "%d %d\n%d %d\n%d %d\n", 0, 0, x2, y2, x3, y3);
    fclose(f);

    std::string str = "Number of ESPs: 3\nCoordinates:\n - (0, 0)\n - (";
    str.append(std::to_string(x2) + ", " + std::to_string(y2));
    str.append(")\n - (" + std::to_string(x3));
    str.append(", " + std::to_string(y3));
    str.append(")");
    QString qstr = QString::fromStdString(str);
    ui->config->setText(qstr);

    ui->x2->setEnabled(false);
    ui->y2->setEnabled(false);
    ui->x3->setEnabled(false);
    ui->y3->setEnabled(false);

    ui->pushButton->show();
}

void Dialog1::on_radioButton_3_clicked() //4 Esp
{
    ui->label_8->show();
    ui->label_9->show();
    ui->label_10->show();
    ui->length->show();
    ui->width->show();
    ui->ok_4->show();

    ui->label->hide();
    ui->distance->hide();
    ui->ok_2->hide();
    ui->label_2->hide();
    ui->label_3->hide();
    ui->label_4->hide();
    ui->label_5->hide();
    ui->label_6->hide();
    ui->label_7->hide();
    ui->x2->hide();
    ui->y2->hide();
    ui->x3->hide();
    ui->y3->hide();
    ui->ok_3->hide();

    for (int i = 1; i <= 30; i++) {
        ui->length->addItem(QString::fromStdString(std::to_string(i)));
        ui->width->addItem(QString::fromStdString(std::to_string(i)));
    }
}

void Dialog1::on_ok_4_clicked()
{
    ui->length->setEnabled(false);
    QString x_str = ui->length->currentText();
    int x = x_str.toInt();

    max_x = x;

    ui->width->setEnabled(false);
    QString y_str = ui->width->currentText();
    int y = y_str.toInt();

    max_y = y;

    f = fopen("config.txt", "w");
    fprintf(f, "Number of ESP modules: %d\n", 4);
    fprintf(f, "PORT NUMBER: %d\n", PORT_NUMBER);
    fprintf(f, "%d %d\n%d %d\n%d %d\n%d %d\n", 0, 0, x, 0, 0, y, x, y);
    fclose(f);

    std::string str = "Number of ESPs: 4\nCoordinates:\n - (0, 0)\n - (";
    str.append(std::to_string(x) + ", 0)\n");
    str.append(" - (0, " + std::to_string(y));
    str.append(")\n - (" + std::to_string(x));
    str.append(", " + std::to_string(y));
    str.append(")");
    QString qstr = QString::fromStdString(str);
    ui->config->setText(qstr);

    ui->pushButton->show();
}


void Dialog1::on_pushButton_clicked()
{
    this->close();
    rwp->uirw->plot->xAxis->setRange(-0.5, max_x + 0.5);
    rwp->uirw->plot->yAxis->setRange(-0.5, max_y + 0.5);
    rwp->show();
    d->show();

    sysThread* st = new sysThread();
    st->start();
}
