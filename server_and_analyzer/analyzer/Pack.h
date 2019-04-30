#ifndef SERVER2_PACK_H
#define SERVER2_PACK_H

#include <string>
#include <vector>
#include <iostream>

using  namespace std;

class Pack {

private:
    int sq_ctrl;
    string hash, mac;
    string ssid , date, timestamp;
    long systime;
    vector<int> rssi; //Lista di rssi uno per ogni scheda
    vector<double> distances;

public:

    Pack(const string &hash, const string &mac, const int &rssi, const int &sq_ctrl, const string &date, const string &timestamp, const string &ssid){
        this -> hash = hash;
        this -> mac = mac;
        this -> rssi.push_back(rssi);
        this -> sq_ctrl = sq_ctrl;
        this -> date = date;
        this -> timestamp = timestamp;
        this -> ssid = ssid;
    }

    Pack(const string &hash, const string &mac, const long &systime, const int &rssi, const int &sq_ctrl, const string &date, const string &timestamp, const string &ssid){
        this -> hash = hash;
        this -> mac = mac;
        this -> systime = systime;
        this -> rssi.push_back(rssi);
        this -> sq_ctrl = sq_ctrl;
        this -> date = date;
        this -> timestamp = timestamp;
        this -> ssid = ssid;
    }

    void addRSSI(int);
    void printvector();
    int getSq_ctrl() const;
    bool isCompatible(Pack*);
    const vector<double> &getDistances() const;
    const string &getDate() const;
    const string &getTimestamp() const;
    const string &getHash() const;
    const string &getMac() const;
    const string &getSsid() const;
    const vector<int> &getRssi() const;
    void setDistances(const vector<double> &distances);
    void setMac(const string &value);
    void setRssi(const vector<int> &value);
    void setSq_ctrl(int value);
};


#endif //SERVER2_PACK_H
