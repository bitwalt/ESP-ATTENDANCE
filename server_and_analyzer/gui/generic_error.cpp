#include "generic_error.h"
#include "ui_generic_error.h"

generic_error::generic_error(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::generic_error)
{
    ui->setupUi(this);
}

generic_error::~generic_error()
{
    delete ui;
}

void generic_error::setError(const QString &value)
{
    this->show();
    error = value;
    ui->label->setText(error);
}

void generic_error::on_pushButton_clicked()
{
    this->close();
}

void generic_error::update_error(const QString &value)
{
    this->show();
    error = value;
    ui->label->setText(error);
}
