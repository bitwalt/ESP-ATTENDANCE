#pragma once
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <strings.h>
#include <stdlib.h>
#include <string>
#include "Position.h"
#include "Server.h"
#include "runtimewindow.h"

class System
{

private:
    Server *server;
    Analyzer *analyzer;
    Database *db;
    int N, PORT;
    vector<Position*> pos;
    QString log;

public:
    System();
    ~System();
    int exist_db(char*);
    int read_configuration();
    void start();
    void sever_start();

    void setLog(const QString &value);
};

