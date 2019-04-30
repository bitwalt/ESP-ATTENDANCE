#include "runtimewindow.h"

extern RuntimeWindow* rwp;
extern datastream *d;

vector<Position *> Analyzer::getPosizioni_esp() const {
    return posizioni_esp;
}

Analyzer::Analyzer(QObject *parent) : QObject(parent) {
}

void Analyzer::init() {

    connect(this, SIGNAL(update_view()), rwp, SLOT(SLOT_plot1()));
    connect(this, SIGNAL(end_analysis5min()), rwp, SLOT(SLOT_plot2()));

    switch (N) {

    case 2:
        max = posizioni_esp[1]->getX();
        break;
    case 3:
        if(posizioni_esp[1]->getX()!=0)
            max = posizioni_esp[1]->getX();
        else
            max = posizioni_esp[1]->getY();
        break;
    case 4:
        max = pitagora(posizioni_esp);
        break;
    }
}

void Analyzer::compute_analysis(const map<std::__cxx11::string, Pack *> &packs, int five_min) {

    hash_pack = packs;
    long timestamp = time(NULL);
    map<string, Pack*> mac_packs;

    //Transforma potenza segnale da dB a metri
    transform_dB_signal_to_m();

    //Filtra i pacchetti con distanza > della massima possibile
    filter_distances();

    //Trova i dispositivi che cambiano mac e assegna un mac univoco
    found_hided_mac();

    //Calcola posizione X,Y a partire dalle distanze
    calculate_positions_xy();

    //Conta e mostra i dispositivi presenti nell'ultimo minuto con posizione relativa
    count_show_attendance();

    //Scrivi mappa per il database
    mac_packs = populate_mac_map(hash_pack);

    //Salva dati sul database
    db->populate_db(mac_packs, mac_position);

    //AGGIORNA GUI
    rwp->setMac_position(mac_position);
    rwp->setNumber_devices(mac_position.size());

    emit update_view();

    cout << "ROUND = " << five_min << endl;

    if(five_min%TURN == 0) {
        estimate_continuously_present();

        storico_n_disp[timestamp] = now_present;
        db->save_presents(timestamp, now_present);

        rwp->setStorico_n_disp(storico_n_disp);
        rwp->setTimestamp(timestamp);

        mac_vector.clear();
        hiders_map.clear();

        emit end_analysis5min();

    }

    hash_pack.clear();
    hash_distances.clear();
    filtered_hash_distances.clear();
    mac_position.clear();
}

vector<double> Analyzer::transformDbTom(const vector<int> &rssi){
    vector<double> dist;
    for(auto i=0; i< (int)rssi.size() ; i++) {
        double d = calculateDistance(rssi[i], 2400);
        dist.push_back(d);
    }
    return dist;
}

void Analyzer::estimate_continuously_present(){
    vector<string> macs;
    now_present = 0;

    for(auto iter = mac_count.begin() ; iter != mac_count.end(); iter++) {
        if (iter->second == TURN) {
            now_present++;
            macs.push_back(iter->first);
        }
    }
    fprintf(stderr, "#DEVICES CONTINUOUSLY PRESENT FOR %d MIN: %d\n", TURN, now_present);
    setLog(log = "\nDEVICES CONTINUOUSLY PRESENT FOR "+QString::number(TURN)+" MIN: "+QString::number(now_present) + "\n\n");

    mac_count.clear();
}

void Analyzer::count_show_attendance() {
    int i = 1;

    QDateTime d = QDateTime::currentDateTime();
    setLog(log = "Current scan: " + d.toString("dd/MM/yyyy - hh:mm:ss") + " - CICLE: " + QString::number(turn++) + "\n");

    for (auto iter = mac_position.begin(); iter != mac_position.end(); iter++) {
        string mac = iter->first;
        printf("%d)\tMac: %s\tX= %lf\tY= %lf\n", i, mac.c_str(), iter->second->getX(), iter->second->getY());
        setLog(log = QString::number(i) + ")\tMac: " + QString::fromStdString(mac.c_str()) + "\tX: " + QString::number(iter->second->getX(), 'g' ,2) + "\tY: " +  QString::number(iter->second->getY(), 'g' , 2) + "\n");
        i++;

        if (mac_count.find(mac) == mac_count.end()) {
            mac_count[mac] = 1;
        }
        else
            mac_count[mac]++;
    }
}

void Analyzer::calculate_positions_xy() {

    string hash, mac;
    Pack* p;
    for(auto iter = filtered_hash_distances.begin(); iter != filtered_hash_distances.end(); iter++){

        hash = iter->first;
        distances = iter->second;
        p = hash_pack[hash];
        mac = p->getMac();
        Position* pos = new Position(distances, posizioni_esp); // x= 10 ; y=14 , entrambe zero se da filtrare
        if(pos->isInside())
            mac_position[mac] = pos;
    }
    printf("\033[22;34m==============================================================\n\033[0m");
    printf("\033[22;34mDEVICES PRESENT IN THE LAST MINUTE: %d\n\033[0m", (int) mac_position.size());
    printf("\033[22;34m==============================================================\n\033[0m");
}

void Analyzer::transform_dB_signal_to_m() {

    int i=0 ;
    Pack *p;
    string hash;

    for (auto &iter : hash_pack) {

        hash = iter.first;
        p = iter.second;

        distances = transformDbTom(p->getRssi());
        hash_distances[hash] = distances;
        i++;
    }
}

void Analyzer::filter_distances() {
    string hash;
    bool erase = false;

    //CANCELLO DISTANZE MAGGIORI DI MAX
    for (auto &hash_distance : hash_distances) {

        hash = hash_distance.first;
        vector<double> d = hash_distance.second;
        erase = false;

        for (int i = 0; i < N; i++) {
            if(d[i] > max){
                erase = true;
                break;
            }
        }

        if(!erase)
            filtered_hash_distances[hash] = d;
    }
}

void Analyzer::print_distances() {

    int i=0;
    string hash;
    printf("==============================================================\n");
    printf("DIMENSION OF FILTERED MAP HASH-PACK: %d\n", (int) filtered_hash_distances.size());
    printf("==============================================================\n");

    for (auto &filtered_hash_distance : filtered_hash_distances) {
        hash = filtered_hash_distance.first;

        vector<double> d = filtered_hash_distance.second;

        for (int j = 0; j < N; j++) {
            printf("\t%.1lfm \t", d[j]);
        }
        printf("\n");
        i++;
    }
}

double Analyzer::calculateDistance(int signal, double freqMhz) {
    double exp = (27.55 - (20 * log10(freqMhz)) + abs(signal)) / 20.0;
    return pow(4.0, exp);
}

double Analyzer::pitagora(const vector<Position *> &pos){
    Position* p = pos[3];
    double x = p->getX();
    double y = p->getY();
    return sqrt(pow(x,2)+pow(y,2));
}

void Analyzer::print_topN(const vector<pair<std::__cxx11::string, int> > &topN) {
    int i = 1;
    for(auto iter = topN.begin(); iter != topN.end(); iter++) {
        cout << "#" << i << "\t MAC: " << iter->first << "\t FREQUENCE:\t" << iter->second << endl;
        i++;
    }
}

map<string, Pack *> Analyzer::populate_mac_map(const map<std::__cxx11::string, Pack *> &hash_packs) {

    Pack* p;
    string mac;
    map<string, Pack*> mac_packs;

    for (auto iter = hash_packs.begin(); iter != hash_packs.end(); iter++) {
        p = iter->second;
        mac = p->getMac();
        mac_packs[mac] = p;
    }

    return mac_packs;
}

int Analyzer::getIndexMac(Pack* p) {

    //check nel vettore dei mac e ritorniamo l'indice
    string mac = p->getMac();
    string new_mac;
    bool found = false;
    auto iter = find(mac_vector.begin(), mac_vector.end(), mac);
    if(iter!=mac_vector.end())
        return distance(mac_vector.begin(), iter);

    //check della compatibilità

    for(auto &it : hiders_map) {
        Pack *pack = it.second;

        if(pack->isCompatible(p)){
            new_mac = pack->getMac();
            found = true;
            break;
        }
    }

    if(found){
        auto iter = find(mac_vector.begin(), mac_vector.end(), new_mac);
        if(iter!=mac_vector.end())
            return distance(mac_vector.begin(), iter);
        else return -1;
    }

    return -1;
}

void Analyzer::found_hided_mac(){

    //1. Iteriamo sulla mappa pack_map e creiamo un vettore di Pack con possibili prede (secondo bit a 1)
    //2. Cerchiamo correlazioni tra i mac (SEQ NUMBER VICINO, SEGNALE VICINO, PRINT DEL COSTRUTTORE)
    //3. Se le troviamo cambiamo il mac in pack_map per quei Pack.

    string mac, new_mac;
    Pack *p;
    int index ;
    map<int, Pack*> seq_pack ;

    for(auto &iter : hash_pack){
        p = iter.second;
        mac = p->getMac();
        char c = mac[1] ;
        if(c == '2' || c == '6' || c == 'a' || c == 'e'){
            seq_pack[p->getSq_ctrl()] = p;
        }
    }

    for(auto &it : seq_pack)
    {
        p = it.second;
        mac = p->getMac();
        index = getIndexMac(p);

        if(index >= 0)
        {   //compatibile con un pacchetto precedente
            new_mac = mac_vector[index];
            if(mac != new_mac)
            {  //aggiorno mappa prinicipale e hider
                p->setMac(new_mac);
                hash_pack[p->getHash()] = p;
                Pack *pack = hiders_map[new_mac];
                pack->setSq_ctrl(p->getSq_ctrl());
                pack->setRssi(p->getRssi());
                hiders_map[new_mac] = pack;
            } else  //aggiorno solo l'hider
                hiders_map[new_mac] = p;
        }
        else {   //non compatibile con nessuno in precedenza -> nuovo hider
            hiders_map[mac] = p;
            mac_vector.push_back(mac);
        }
    }
}

Analyzer::~Analyzer(){
    this->db = nullptr;
}

void Analyzer::setLog(const QString &value)
{
    d->setData(value);
}
