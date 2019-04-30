#ifndef DIALOG1_H
#define DIALOG1_H

#include <QDialog>

namespace Ui {
class Dialog1;
}

class Dialog1 : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog1(QWidget *parent = 0);
    ~Dialog1();

private slots:

    void on_radioButton_clicked();
    void on_ok_2_clicked();
    void on_radioButton_2_clicked();
    void on_x2_currentIndexChanged(int index);
    void on_y2_currentIndexChanged(int index);
    void on_ok_3_clicked();
    void on_radioButton_3_clicked();
    void on_pushButton_clicked();
    void on_ok_4_clicked();

private:
    Ui::Dialog1 *ui;
    FILE *f;
    int max_x, max_y;

};

#endif // DIALOG1_H
