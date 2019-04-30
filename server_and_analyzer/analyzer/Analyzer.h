#pragma once
#include "Database.h"
#include "Pack.h"
#include "Position.h"
#include "sys/time.h"
#include <list>
#include <QObject>
#include <QThread>
#include <QDebug>
#include <QApplication>
#include "datastream.h"

#define TURN 5
#define MAX 100

class Analyzer : public QObject
{
    Q_OBJECT

private:

    QString log;
    double max;
    Database* db;
    int N;
    int turn = 1;
    int now_present; //stima del #device presenti ogni 5 minuti nella stanza ( = hash_pack.size() )
    vector<Position*> posizioni_esp; //posizioni delle schede
    vector<double> distances;
    vector<string> mac_vector;
    map<string, Pack*> hiders_map;
    map<string, int> mac_count ; // contiene mac e numero di volte presente ogni 5 min
    map<long, int> storico_n_disp ;
    map<string, Pack*> hash_pack; // ( int hash, Pack p ) MAPPA RICEVUTA OGNI MINUTO DAL SERVER
    map<string, vector<double>> hash_distances;
    map<string, vector<double>> filtered_hash_distances;
    map<string , Position*> mac_position;/* Contiene la correlazione MAC - POSIZIONE (X,Y) [aggiornata ogni minuto]
                                          * (aa:bb:cc:dd:00:11:22 ; (x= 10 , y=5)) ultima posizione del mac in questo minuto */

public:

    explicit Analyzer(QObject *parent = nullptr);

    Analyzer(int N, Database* database, vector<Position*> position){
        this->db = database;
        this->N = N;
        this->posizioni_esp = position;
    }

    void init();
    void transform_dB_signal_to_m();
    void filter_distances();
    void print_distances();
    void print_specific_situation();
    void calculate_positions_xy();
    void count_show_attendance();
    void print_topN(const vector<pair<string, int>> &);
    void found_hided_mac();
    void estimate_continuously_present();
    void setLog(const QString &value);
    int  getIndexMac(Pack* p);
    double pitagora(const vector<Position*> &);
    double calculateDistance(int signal, double freqMhz);
    vector<double> transformDbTom(const vector<int> &);
    vector<Position *> getPosizioni_esp() const;
    map<string, Pack *> populate_mac_map(const map<string, Pack *> &);
    ~Analyzer();

public slots:
    void compute_analysis(const map<string, Pack*> &hash_pack, int five_min);

signals:
    void update_view();
    void end_analysis5min();
};

