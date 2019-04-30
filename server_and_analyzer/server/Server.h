#pragma once
#define COLUMN 128
#define SIZE_ID 2
#define MAC_SIZE 18
#define TIMEOUT_SIZE 65
#define TIMEOUT_PACK 3

#include "Analyzer.h"

#include <thread>
#include <future>
#include <unordered_set>
#include <sys/time.h>


class Server : public QObject
{
    Q_OBJECT

private:
    QString device_conn, error, log;

protected:
    int port, backlog;
    int socketfd , sockconn;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    char recvbuf[COLUMN+1];
    int N , count=0;
    unordered_set<int> connected;
    vector<string> vector_mac;
    map<string,int> hash_map; //(hash, +1)
    map<string, Pack*> pack_map; //(hash, *Pack)
    Analyzer* analyzer; //puntatore all'analyzer

signals:
    void launch_analyzer(map<string, Pack*>, int);

public:

    Server(int port, int N, Analyzer* analyzer) {
        this->port = port;
        this->backlog = N;
        this->analyzer = analyzer;
        this->N = N;
    }

    int init();
    int start();
    void stop();
    int accept_s(int, struct sockaddr_in*);
    void close_s();
    void service(int, int, promise<int>&&);
    void send_ack(int, bool);
    bool read_ack(int);
    int send_time(int);
    int send_id(int, uint16_t);
    int find_mac(string);
    int readfile_tomaps();
    int start_select(int, fd_set, struct timeval, bool);
    string read_mac(int);
    void read_macs();
    void show_error(int);
    bool errors_present(vector<future<int>>&);
    void filter();
    void setError(const QString &value);
    void setLog(const QString &value);
    ~Server();

    static void do_join(std::thread& t) {
        t.join();
    }

};
