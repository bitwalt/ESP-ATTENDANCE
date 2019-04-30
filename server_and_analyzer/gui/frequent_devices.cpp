#include "frequent_devices.h"
#include "ui_frequent_devices.h"
#include "error_specific_situation.h"

frequent_devices::frequent_devices(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frequent_devices)
{
    ui->setupUi(this);
    db = new Database(true);
    ui->dateTimeEdit_start->setFocus();

    this->setWindowTitle("Frequent devices in the past");
}

frequent_devices::~frequent_devices(){
    delete ui;
}

void frequent_devices::on_pushButton_clicked(){
    start = ui->dateTimeEdit_start->dateTime();
    end = ui->dateTimeEdit_end->dateTime();

    if (start > end) {
        error_specific_situation *e = new error_specific_situation();
        e->show();
    } else {

        date_start = start.date();
        x = date_start.toString("dd/MM/yyyy");
        x1 = x.toStdString();

        time_start = start.time();
        y = time_start.toString("hh:mm:ss");
        y1 = y.toStdString();


        date_end = end.date();
        x = date_end.toString("dd/MM/yyyy");
        x2 = x.toStdString();

        time_end = end.time();
        y = time_end.toString("hh:mm:ss");
        y2 = y.toStdString();

        n_limit_double = ui->n_limit->value();
        n_limit = (int)n_limit_double;

        vector<pair<string, int>> topN = db->topN(x1, y1, x2, y2, n_limit);

        int i = 1;
        std::string res;
        QString resul;
        if (topN.size() == 0)
            resul = "Nothing found";
        else {
            for(auto iter = topN.begin(); iter != topN.end(); iter++) {
                res = "#" + std::to_string(i) + "\t MAC: " + iter->first + "\tPRESENT: " + std::to_string(iter->second)
                          + "\tTIMES, FOR A TOTAL OF:\t" + std::to_string(iter->second*5) + " MIN\n";
                resul.append(QString::fromStdString(res));
                i++;
                res.clear();
            }
        }

        ui->result->setText(resul);

    }
}
