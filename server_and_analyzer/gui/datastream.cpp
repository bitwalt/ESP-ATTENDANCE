#include "datastream.h"
#include "ui_datastream.h"

datastream::datastream(QWidget *parent) : QDialog(parent), ui(new Ui::datastream) {
    ui->setupUi(this);
    this->setWindowTitle("LOG");
}

datastream::~datastream() {
    delete ui;
}

void datastream::setData(const QString &value) {
    data.append(value);
    ui->label->setText(data);
}

void datastream::cleanData() {
    data.clear();
}
