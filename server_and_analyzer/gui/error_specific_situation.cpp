#include "error_specific_situation.h"
#include "ui_error_specific_situation.h"

error_specific_situation::error_specific_situation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::error_specific_situation)
{
    ui->setupUi(this);

    this->setWindowTitle("ERROR!");
}

error_specific_situation::~error_specific_situation(){
    delete ui;
}
