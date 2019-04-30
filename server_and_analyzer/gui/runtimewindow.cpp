#include "runtimewindow.h"

RuntimeWindow::RuntimeWindow(QWidget *parent) :
    QMainWindow(parent),
    uirw(new Ui::RuntimeWindow)
{
    uirw->setupUi(this);
    this->setWindowTitle("SITUATION AT RUN-TIME");
    uirw->plot->setLocale(QLocale(QLocale::English, QLocale::Italy));
    uirw->plot->plotLayout()->insertRow(0);

    title = new QCPTextElement(uirw->plot, "Current situation", 20);
    uirw->plot->plotLayout()->addElement(0, 0, title);
    uirw->plot->addGraph();
    uirw->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
    uirw->plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    uirw->plot->xAxis->setLabel("X-coordinate");
    uirw->plot->yAxis->setLabel("Y-coordinate");

    uirw->plot_2->plotLayout()->insertRow(0);
    uirw->plot_2->plotLayout()->addElement(0, 0, new QCPTextElement(uirw->plot_2, "Number of devices continuously present for 5 min", 20));
    uirw->plot_2->addGraph();
    uirw->plot_2->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
    uirw->plot_2->graph(0)->setLineStyle(QCPGraph::lsLine);
    gettimeofday(&tv, NULL);
    uirw->plot_2->xAxis->setRange(tv.tv_sec - 20, tv.tv_sec + 380);
    uirw->plot_2->yAxis->setRange(0, 80);
    uirw->plot_2->xAxis->setLabel("Time");
    uirw->plot_2->yAxis->setLabel("Number of devices");

    QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
    timeTicker->setDateTimeFormat("dd/MM/yy\nhh:mm:ss");
    uirw->plot_2->xAxis->setTicker(timeTicker);

    uirw->error->setStyleSheet(QStringLiteral("QLabel{color: rgb(255,0,0);}"));

}

RuntimeWindow::~RuntimeWindow()
{
    delete uirw;
}

void RuntimeWindow::addPoint(double x, double y)
{
    qv_x.append(x);
    qv_y.append(y);
    plot_current_situation();

}

void RuntimeWindow::plot_esp_position(){

    if (pos.size() == 3) { //3 esp

        QCPItemLine* line1 = new QCPItemLine(uirw->plot);
        QCPItemLine* line2 = new QCPItemLine(uirw->plot);
        QCPItemLine* line3 = new QCPItemLine(uirw->plot);



        line1->start->setCoords(0,0);
        line1->end->setCoords(pos[1]->getX(), pos[1]->getY());
        line2->start->setCoords(pos[1]->getX(), pos[1]->getY());
        line2->end->setCoords(pos[2]->getX(), pos[2]->getY());
        line3->start->setCoords(pos[2]->getX(), pos[2]->getY());
        line3->end->setCoords(0,0);
        uirw->plot->replot();

     }else if (pos.size() == 4) { //4 esp
        uirw->plot->addGraph();
        uirw->plot->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);
        uirw->plot->graph(1)->setLineStyle(QCPGraph::lsNone);
        uirw->plot->graph(1)->setPen(QPen(Qt::red));
        QCPItemRect *rect = new QCPItemRect(uirw->plot);
        rect->setVisible(true);
        rect->topLeft->setCoords(pos[2]->getX(),pos[2]->getY());
        rect->bottomRight->setCoords(pos[1]->getX(),pos[1]->getY());
        uirw->plot->replot();
        uirw->plot->update();
    } else { //2 esp
        uirw->plot->addGraph();
        uirw->plot->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);
        uirw->plot->graph(1)->setLineStyle(QCPGraph::lsLine);
        uirw->plot->graph(1)->setPen(QPen(Qt::red));
        uirw->plot->graph(1)->setData(esp_x, esp_y);
        uirw->plot->replot();
        uirw->plot->update();
    }
}

void RuntimeWindow::addEspPoint(double x, double y)
{
    esp_x.append(x);
    esp_y.append(y);
    plot_esp_position();
}

void RuntimeWindow::clearData()
{
    qv_x.clear();
    qv_y.clear();
}

void RuntimeWindow::plot_current_situation()
{       
    uirw->plot->graph(0)->setData(qv_x, qv_y);
    uirw->plot->replot();
    uirw->plot->update();
}

void RuntimeWindow::addPoint2(long x, int y)
{

    qv_x2.push_front(x);
    qv_y2.push_front(y);

    if (qv_x2.length() > 10) {
        qv_x2.removeLast();
        qv_y2.removeLast();
    }

    //Auto-update of max and min
    min_x = *std::min_element(qv_x2.begin(), qv_x2.end());
    max_x = *std::max_element(qv_x2.begin(), qv_x2.end());
    uirw->plot_2->xAxis->setRange(min_x - 1, max_x + 20);

    min_y = *std::min_element(qv_y2.begin(), qv_y2.end());
    max_y = *std::max_element(qv_y2.begin(), qv_y2.end());
    uirw->plot_2->yAxis->setRange(0, max_y + 5);

    plot_attendance();
}

void RuntimeWindow::plot_attendance()
{
    uirw->plot_2->graph(0)->setData(qv_x2, qv_y2);
    uirw->plot_2->replot();
    uirw->plot_2->update();
}

void RuntimeWindow::on_frequent_devices_pushButton_clicked(){
    frequent_devices *f = new frequent_devices();
    f->show();
}

void RuntimeWindow::on_specific_situation_pushButton_2_clicked(){
    specific_situation *s = new specific_situation();
    s->show();
}

void RuntimeWindow::SLOT_plot1(){
    clearData();
    for (auto &mac_p : mac_position) {
        Position *pos = mac_p.second;
        addPoint(pos->getX(), pos->getY());
    }

    for(auto& it : pos){
        addEspPoint(it->getX(), it->getY());
    }

    mac_position.clear();
}

void RuntimeWindow::SLOT_plot2(){
    addPoint2(timestamp, storico_n_disp[timestamp]);
}

void RuntimeWindow::setNumber_devices(int value){
    number_devices = value;
    std::string str = "Current situation: " + std::to_string(number_devices) + " devices";
    title->setText(QString::fromStdString(str));
    uirw->plot->replot();
}

void RuntimeWindow::setDevice_conn(const QString &value){
    device_conn.clear();
    device_conn = value;
    uirw->device_conn->setText(device_conn);
}

void RuntimeWindow::setTimestamp(int value){
    timestamp = value;
}

void RuntimeWindow::setStorico_n_disp(const map<long, int> &value){
    storico_n_disp = value;
}

void RuntimeWindow::setMac_position(const map<string, Position *> &value){
    mac_position = value;
}

void RuntimeWindow::setError(const QString &value){
    error.clear();
    error = value;
    uirw->error->setText(error);
}

void RuntimeWindow::setPos(const vector<Position *> &value){
    pos = value;
}
