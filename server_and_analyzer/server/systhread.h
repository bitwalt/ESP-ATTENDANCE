#pragma once
#include <QThread>
#include "system.h"

class sysThread: public QThread
{
    Q_OBJECT
    void run() override {

        sys = new System();
        sys->start();
        sys->sever_start();
    }

public:
    sysThread();
    void stop();

private:
    volatile bool stopped;
    System *sys;

};
