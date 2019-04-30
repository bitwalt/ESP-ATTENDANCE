#ifndef GENERIC_ERROR_H
#define GENERIC_ERROR_H

#include <QDialog>

namespace Ui {
class generic_error;
}

class generic_error : public QDialog
{
    Q_OBJECT

public:
    explicit generic_error(QWidget *parent = 0);
    ~generic_error();
    void setError(const QString &value);

private slots:
    void on_pushButton_clicked();

public slots:
    void update_error(const QString &value);

private:
    Ui::generic_error *ui;
    QString error;
};

#endif // GENERIC_ERROR_H
