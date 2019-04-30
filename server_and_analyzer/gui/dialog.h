#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "dialog2.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:

    void on_next_esp_pushButton_clicked();

    void get_coordinates();

    void on_numESP_comboBox_currentTextChanged(const QString &arg1);

    void on_ok_pushButton_clicked();

private:
    Ui::Dialog *ui;
    FILE *f;
    int boards_no;
    int cnt_int = 0;

};

#endif // DIALOG_H
