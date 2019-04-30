#ifndef SERVER_MAIN_H
#define SERVER_MAIN_H

#include <QObject>

class server_main : public QObject
{
    Q_OBJECT


public:
    server_main();

    int exist_db(char*);
    int read_configuration();
    void begin();

};

#endif // SERVER_MAIN_H
