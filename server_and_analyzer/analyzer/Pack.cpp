#include "Pack.h"

void Pack::addRSSI(int signal) {
    rssi.push_back(signal);
}

void Pack::printvector(){
    cout << "[  " ;
    for(auto i = rssi.begin(); i!= rssi.end() ; i++){
        cout << *i << "  " ;
    }
    cout << "]" ;
}

const string &Pack::getHash() const {
    return hash;
}

const string &Pack::getMac() const {
    return mac;
}

const string &Pack::getSsid() const {
    return ssid;
}

const vector<int> &Pack::getRssi() const {
    return rssi;
}

void Pack::setDistances(const vector<double> &distances) {
    Pack::distances = distances;
}

const vector<double> &Pack::getDistances() const {
    return distances;
}

const string &Pack::getDate() const {
    return date;
}

const string &Pack::getTimestamp() const {
    return timestamp;
}

int Pack::getSq_ctrl() const {
    return sq_ctrl;
}

void Pack::setSq_ctrl(int value){
    sq_ctrl = value;
}

void Pack::setRssi(const vector<int> &value){
    rssi = value;
}

void Pack::setMac(const string &value){
    mac = value;
}

bool Pack::isCompatible(Pack* p){

    if(abs(sq_ctrl - p->getSq_ctrl()) < 4000 && abs(rssi[0] - p->getRssi()[0]) < 10)
        return true;
    else
        return false;
}
