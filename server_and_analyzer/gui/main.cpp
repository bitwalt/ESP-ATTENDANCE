#include "mainwindow.h"
#include "runtimewindow.h"
#include <QApplication>
#include "datastream.h"

RuntimeWindow* rwp;
datastream* d;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RuntimeWindow rw;
    rwp = &rw;
    datastream ds;
    d = &ds;

    MainWindow w;
    w.show();

    return a.exec();
}
