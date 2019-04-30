#include "mainwindow.h"
#include "runtimewindow.h"
#include "ui_mainwindow.h"
#include "unistd.h"
#include "Database.h"

#define u "pds"
#define p "7a9fc31646f8c8642f73ef17b6f073be"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{   
    ui->setupUi(this);

    QWidget::setWindowTitle("PROGETTO PDS 2018");

    ui->input_password->setEchoMode(QLineEdit::Password);
    ui->input_username->setFocus();
    ui->login_button->setDefault(true);

    char database_name[20];
    sprintf(database_name, "sniff.db");

    if(!exist_db(database_name)){
        db = new Database(false); //false create a new database
    }
    else {
        db = new Database(true); //true get the existing one
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

int MainWindow::exist_db(char* name) {
    struct stat buf;
    return (stat(name, &buf) == 0);
}

void MainWindow::on_login_button_clicked() {

    QString username_inserted = ui->input_username->text();
    QString password_inserted = ui->input_password->text();
//    bool authorizated = db->log_in(username_inserted.toStdString() , password_inserted.toStdString());

    string hash = md5(string(password_inserted.toStdString()));

    bool authorizated;
    if (username_inserted == u && hash == p)
        authorizated = true;
    else
        authorizated = false;

    if (authorizated) {

        if (access("config.txt", F_OK) != -1) {// File exists
            Dialog2* dialog2 = new Dialog2();
            dialog2->show();
            MainWindow::close();
        } else { // File doesn't exist
            this->close();
            Dialog1* dialog1 = new Dialog1();
            dialog1->show();
        }
    } else {
        ui->label_error_login->setText("ERROR! WRONG CREDENTIALS");
        ui->input_username->selectAll();
    }
}
