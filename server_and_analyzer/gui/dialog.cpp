#include <QApplication>
#include "dialog.h"
#include "ui_dialog.h"
#include "ui_runtimewindow.h"
#include "runtimewindow.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "thread.h"

#define PORT_NUMBER 5000

extern RuntimeWindow* rwp;

int coord_x = -1;
int coord_y = -1;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{

    ui->setupUi(this);

    ui->input_dimensions_x->setValidator( new QIntValidator(0, 60, this) );
    ui->input_dimensions_y->setValidator( new QIntValidator(0, 60, this) );

    ui->next_esp_pushButton->hide();
    ui->ok_pushButton->hide();
    ui->input_xc->hide();
    ui->input_yc->hide();
    ui->input_dimensions_x->hide();
    ui->input_dimensions_y->hide();
    ui->label_2->hide();
    ui->label_3->hide();
    ui->label_4->hide();
    ui->label_5->hide();
    ui->label_6->hide();

}

Dialog::~Dialog()
{

    delete ui;

}

void Dialog::on_numESP_comboBox_currentTextChanged(const QString &arg1)
{

    boards_no = arg1.toInt();

    ui->numESP_comboBox->setEnabled(false);

    f = fopen("config.txt", "w");
    fprintf(f, "%d\n", boards_no);
    fprintf(f, "%d\n", PORT_NUMBER);

    ui->label_2->show();
    ui->label_5->show();
    ui->label_6->show();
    ui->input_dimensions_x->show();
    ui->input_dimensions_y->show();
    ui->ok_pushButton->show();

}

void Dialog::on_ok_pushButton_clicked()
{

    if (ui->input_dimensions_x->isModified())
        coord_x = ui->input_dimensions_x->text().toInt();
    if (ui->input_dimensions_y->isModified())
        coord_y = ui->input_dimensions_y->text().toInt();

    if (coord_x != -1 && coord_y != -1) {

        fprintf(f, "%d %d\n", coord_x, coord_y);
        ui->label_7->clear();
        ui->ok_pushButton->setEnabled(false);
        ui->input_dimensions_x->setReadOnly(true);
        ui->input_dimensions_y->setReadOnly(true);

    }

    else {

        ui->label_7->setText("ERROR! INSERT ROOM DIMENSIONS");
        return;

    }

    ui->next_esp_pushButton->show();
    ui->input_xc->show();
    ui->input_yc->show();
    ui->label_3->show();
    ui->label_4->show();
    ui->input_xc->setFocus();

    for (int i = 0; i <= coord_x; i++)
        ui->input_xc->addItem(QString::fromStdString(std::to_string(i)));
    for (int i = 0; i <= coord_y; i++)
        ui->input_yc->addItem(QString::fromStdString(std::to_string(i)));

    std::string cnt_str = "Board #";
    cnt_str.append(std::to_string(++cnt_int));
    QString cnt_qstr = QString::fromStdString(cnt_str);
    ui->board->setText(cnt_qstr);

}

void Dialog::on_next_esp_pushButton_clicked()
{

    if (cnt_int <= boards_no) {

        get_coordinates();
        std::string cnt_str = "Board #";
        cnt_str.append(std::to_string(++cnt_int));
        QString cnt_qstr = QString::fromStdString(cnt_str);
        ui->board->setText(cnt_qstr);
        ui->input_xc->setFocus();

    }

    if (cnt_int == boards_no + 1) {

        fclose(f);
        this->close();
        rwp->uirw->plot->xAxis->setRange(0, coord_x);
        rwp->uirw->plot->yAxis->setRange(0, coord_y);
        rwp->show();
        Thread* t = new Thread();
        t->start();

    }

}

void Dialog::get_coordinates()
{

    QString x_str = ui->input_xc->currentText();
    int x = x_str.toInt();
    QString y_str = ui->input_yc->currentText();
    int y = y_str.toInt();

    fprintf(f, "%d %d\n", x, y);

    ui->input_xc->setCurrentIndex(ui->input_xc->findText("0"));
    ui->input_yc->setCurrentIndex(ui->input_yc->findText("0"));

}
