#ifndef ERROR_SPECIFIC_SITUATION_H
#define ERROR_SPECIFIC_SITUATION_H

#include <QDialog>

namespace Ui {
class error_specific_situation;
}

class error_specific_situation : public QDialog
{
    Q_OBJECT

public:
    explicit error_specific_situation(QWidget *parent = 0);
    ~error_specific_situation();

private:
    Ui::error_specific_situation *ui;
};

#endif // ERROR_SPECIFIC_SITUATION_H
