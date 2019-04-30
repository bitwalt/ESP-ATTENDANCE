#ifndef SERVER2_DATABASE_H
#define SERVER2_DATABASE_H

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cerrno>
#include <string>
#include <iostream>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <map>
#include <iterator>
#include <functional>
#include <algorithm>
#include <set>
#include <utility>
#include "md5.h"
#include "Position.h"
#include "Pack.h"

class Database {

protected:
    sqlite3* conn;

public:

    Database(bool exist) {
        if(exist) {
            open_database("sniff.db");
        }
        else {
            std::cout << "CREATING DATABASE..." << std::endl;
            create_database();
        }
    }

    void populate_db(map<string, Pack *>, map<string, Position *>&);
    void get_specific_situation(const string&, const string&, const string&, const string&, map<long, map<string, Position *>>&);
    vector<pair<string, int>> topN(const string&, const string&, const string&, const string&, int);
    void save_presents(long,int);
    void create_database();
    void create_table_present();
    void sql_create_table_sniff(char *zErrMsg);
    int open_database(const char *filename);
    bool log_in(const string&, const string&);
    ~Database();

};

#endif //SERVER2_DATABASE_H
