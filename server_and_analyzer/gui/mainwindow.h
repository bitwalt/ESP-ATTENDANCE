#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dialog1.h"
#include "dialog2.h"
#include "form.h"
#include "Database.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    int exist_db(char*);
    ~MainWindow();

private slots:

    void on_login_button_clicked();

private:
    Ui::MainWindow *ui;
    Dialog1* dialog1;
    Database *db;
};

#endif // MAINWINDOW_H
