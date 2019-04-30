#include "dialog2.h"
#include "ui_dialog2.h"
#include "ui_runtimewindow.h"
#include "runtimewindow.h"
#include <QApplication>
#include "Server.h"
#include "system.h"
#include "systhread.h"
#include "datastream.h"

extern RuntimeWindow* rwp;
extern datastream* d;

Dialog2::Dialog2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog2)
{

    ui->setupUi(this);
    ui->title_label->setAlignment(Qt::AlignCenter);
    read_file();

}

Dialog2::~Dialog2()
{
    delete ui;
}

void Dialog2::read_file() {

    FILE* f = fopen("config.txt", "r");
    int i, x, y, num_esp, port_number;
    fscanf(f, "Number of ESP modules: %d\nPORT NUMBER: %d\n", &num_esp, &port_number);

    std::string coordinates_str;

    for (i = 0; i < num_esp; i++) {
        fscanf(f, "%d%d", &x, &y);
        if ( x > max_x)
            max_x = x;
        if ( y > max_y)
            max_y = y;
        coordinates_str.append(std::to_string(x)).append("\t").append(std::to_string(y)).append("\n");
    }

    fclose(f);

    ui->esp_number_label->setText(QString::fromStdString(std::to_string(num_esp)));
    ui->esp_coordinates_label->setText(QString::fromStdString(coordinates_str));
}

void Dialog2::on_yes_pushButton_clicked() {

    this->close();
    rwp->show();
    read_file();
    rwp->uirw->plot->xAxis->setRange(-0.5, max_x +0.5);
    rwp->uirw->plot->yAxis->setRange(-0.5, max_y +0.5);
    d->show();

    sysThread* st = new sysThread();
    st->start();
}

void Dialog2::on_no_pushButton_clicked() {

    this->close();
    Dialog1* dialog1 = new Dialog1();
    dialog1->show();
}
