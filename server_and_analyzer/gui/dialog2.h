#ifndef DIALOG2_H
#define DIALOG2_H

#include <QDialog>
#include "dialog1.h"
#include "server_main.h"

namespace Ui {
class Dialog2;
}

class Dialog2 : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog2(QWidget *parent = 0);
    ~Dialog2();

private slots:

    void read_file();
    void on_yes_pushButton_clicked();
    void on_no_pushButton_clicked();

private:
    Ui::Dialog2 *ui;
    int max_x = 0, max_y = 0;
};

#endif // DIALOG2_H
