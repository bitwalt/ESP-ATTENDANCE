#ifndef DATASTREAM_H
#define DATASTREAM_H

#include <QDialog>

namespace Ui {
class datastream;
}

class datastream : public QDialog
{
    Q_OBJECT

public:
    explicit datastream(QWidget *parent = 0);
    ~datastream();

    void setData(const QString &value);
    void cleanData();

private:
    Ui::datastream *ui;
    QString data;
};

#endif // DATASTREAM_H
