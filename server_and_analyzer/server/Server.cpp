#include "runtimewindow.h"

#define SERVICE_OK 0
#define FILE_ERROR 1
#define READ_ERROR 2
#define SEND_ERROR 3
#define SELECT_TIMEOUT 4
#define ERROR_ID 5
#define MAX_N 4

extern RuntimeWindow* rwp;
extern datastream* d;

int Server::init() {
    
    //SOCKET CREATE
    socketfd = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
    if(socketfd < 0) {
        std::cerr << "Could not create socket" << std::endl;
        setError(error = "Could not create socket");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    //SOCKET OPT
    int flag = 1 ;
    if(setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == -1){
        std::cerr << "Cannot set socket options" << std::endl;
        setError(error = "Cannot set socket options");
        return 1;
    }

    //BIND
    if(bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Cannot bind" << std::endl;
        setError(error = "Cannot bind");
        return 1;
    }

    //LISTEN
    if(listen(socketfd, backlog) < 0) {
        std::cerr << "Could not listen" << std::endl;
        setError(error = "Cannot listen");
        return 1;
    }

    read_macs();

    //COLLEGO SEGNALI A SLOT DELLA GUI
    connect(this, SIGNAL(launch_analyzer(map<string,Pack*>,int)), analyzer, SLOT(compute_analysis(map<string,Pack*>,int)));


    vector<Position*> pos = analyzer->getPosizioni_esp();
    rwp->setPos(pos);

    return 0;
}

void Server::read_macs() {
    int i, n_esp ;
    FILE *f = fopen("mac.txt" , "r");

    if(!f){
        printf("IMPOSSIBILE APRIRE FILE\n");
        setError("Cannot open file");
        exit(1);
    }

    fscanf(f, "Number of ESP: %d", &n_esp);

    char tmp[MAC_SIZE];
    int t;
    for(i=0; i < n_esp ; i++){
        fscanf(f, "%d) %s", &t, tmp);
        vector_mac.push_back(tmp);
        count++;
    }
    fclose(f);
}

int Server::accept_s(int sock , struct sockaddr_in* c_addr) {

    int s;
    struct sockaddr_in caddr =  *c_addr;
    socklen_t addrlen;

    addrlen = sizeof(struct sockaddr_in);
    s = accept(sock, (struct sockaddr *) &caddr, &addrlen);

    return s;
}

bool Server::errors_present(vector<future<int>>& results) {
    int r;
    for(future<int>& res : results){
        r = res.get();
        if(r!=0){
            show_error(r);
            return true;
        }
    }
    return false;
}

int Server::start() {

    std::cout << "STARTING SERVER...\nREADY TO ACCEPT CONNECTION!" << std::endl;
    printf("_____________________________________________________\n\n");

    setLog(log = "STARTING SERVER...\nServer started!\n\nREADY TO ACCEPT CONNECTION!\n\n");

    uint16_t esp_id;
    int round = 0 ;
    string mac;

    vector<std::thread> threads;
    vector<future<int>> futures;

    for(;;) {

        while ((int)connected.size() < N) {

            sockconn = accept_s(socketfd, &client_addr);

            //Read del MAC dal socket
            mac = read_mac(sockconn);
            esp_id = find_mac(mac);

            if(esp_id == ERROR_ID) {
                vector_mac.push_back(mac);
                esp_id = count +1 ;
            } else
                esp_id++;

            setLog(log = "ESP#" + QString::number(esp_id-1) + " CONNECTED\n");

            printf(" with ID: %d\n", esp_id-1);
            //INVIO ID ALLA SCHEDA
            if (!send_id(sockconn, esp_id)) {
                printf("Cannot send ID...\n");
                setError(error = "Cannot send ID");
                return 0;
            }

            //LEGGO ACK
            if(!read_ack(sockconn)) {
                fprintf(stderr, "ESP#%d can not set ID...\n" , esp_id);
                setError(error = "ESP#" + QString::number(esp_id) + "cannot set ID");
                return 0;
            }

            connected.insert(esp_id);

            fprintf(stderr, "CONNECTED DEVICE: %d\n" , (int) connected.size());
            device_conn = "CONNECTED DEVICE: " + QString::number((int)connected.size()) + "\n";
            rwp->setDevice_conn(device_conn);

            promise<int> p;
            futures.push_back(p.get_future());
            threads.push_back(std::thread(&Server::service, this, sockconn,esp_id, move(p)));
        }

        printf("Sniffing....\n\n");
        setLog(log = "\nSniffing...\n");

        for_each(threads.begin(), threads.end(), do_join);

        if(!errors_present(futures))
        {
            //NESSUN ERRORE -> SALVO I PACCHETTI RICEVUTI

            if (!readfile_tomaps()) {
                fprintf(stderr, "Impossible to save packs to maps!!\n");
                setError(error = "Impossible to save packs to maps!!\n");
                return 0;
            }

            //FILTRO I DATI
            filter();

            round++;

            if (round % TURN == 0)
                d->cleanData();

            emit launch_analyzer(pack_map, round);
        }

        connected.clear();
        hash_map.clear();
        pack_map.clear();
        futures.clear();
        threads.clear();
    }
}

void Server::service(int sockconn, int esp_id, promise<int>&& res) {

    char file[20];
    uint32_t npack, i;
    fd_set cset;
    bool error = false;
    struct timeval tval;

    try
    {
        sprintf(file, "sniff%d.txt", esp_id);
        FILE *fp = fopen(file, "w");
        if (!fp) {
            printf("Couldn't open file '%s' PID:%d\n", file, getpid());
            setError(this->error = "Cannot open file " + QString::fromStdString(file));
            res.set_value(FILE_ERROR);
            return;
        }

        //INVIO ORA ALLA SCHEDA
        if (!send_time(sockconn)) {
            printf("COULD NOT SEND TIME\n");
            setError(this->error = "Cannot send time to ESP");
            fclose(fp);
            res.set_value(SEND_ERROR);
            return;
        }

        //RICEVO ACK ORA
        if(!read_ack(sockconn)){
            fprintf(stderr, "ESP#%d COULD NOT SET TIME\n" , esp_id);
            setError(this->error = "ESP#" + QString::number(esp_id) + " cannot set time");
            res.set_value(READ_ERROR);
            return;
        }

        //----sniffing-------

        //RICEVO NUMERO DI PACCHETTI
        if(start_select(sockconn, cset, tval, true) <= 0){
            //true wait for 80 seconds
            fprintf(stderr, "IMPOSSIBLE TO RECEIVE NUMBER OF PACKS FROM ESP#%d\n", esp_id);
            setError(this->error = "Cannot receive number of packs fomr ESP#" + QString::number(esp_id));
            fclose(fp);
            res.set_value(SELECT_TIMEOUT);
            return;
        }

        if (read(sockconn, &npack, sizeof(int)) <= 0) {
            printf("recv failed - Read of number of packs failed\n");
            setError(this->error = "Recv failed");
            send_ack(sockconn, false);
            fclose(fp);
            res.set_value(READ_ERROR);
            return;
        }

        npack = ntohl(npack);

        if (npack > 1000) {
            printf("Number of packs received is wrong!\n");
            setError(this->error = "Number of packs received is wrong!");
            send_ack(sockconn, false);
            fclose(fp);
            res.set_value(READ_ERROR);
            return;
        }

        send_ack(sockconn, true);

        //printf("\033[22;34m-----Received %d packs from ESP#%d (PID=%d)------\n\033[0m", npack, esp_id, getpid());
        //printf("==============================================================\n");

        error = false;

        for(i=0 ; i < npack ; i++)
        {
            if(start_select(sockconn, cset, tval , true) <= 0 ){
                fprintf(stderr, "IMPOSSIBLE RECEIVE PACKS FROM ESP#%d\n", esp_id);
                setError(this->error = "IMPOSSIBLE RECEIVE PACKS FROM ESP#" + QString::number(esp_id));
                fclose(fp);
                res.set_value(SELECT_TIMEOUT);
                return;
            }

            if (recv(sockconn, recvbuf, COLUMN, MSG_WAITALL) < 0) {
                fprintf(stderr, "FOUND A WRONG PACK BY ESP...\nClosing connection!\n");
                setError(this->error = "FOUND A WRONG PACK BY ESP. Closing connection!");
                error = true;
                fclose(fp);
                break;
            }

            recvbuf[COLUMN+1]  = '\0';
            //printf("%d)\t%s\n", i+1, recvbuf);
            fprintf(fp, "%s\n", recvbuf);
        }

        //invio ack
        if(error){
            send_ack(sockconn, false);
            fclose(fp);
            res.set_value(READ_ERROR);
            return;
        }

        send_ack(sockconn, true);
//        printf("ALL PACKS HAVE BEEN RECEIVED!\n");
        fclose(fp);
        close(sockconn);
        res.set_value(SERVICE_OK);
        return;
    } catch (...) {
        res.set_exception(current_exception());
    }
}

void Server::filter(){
    //Take only Packs listened from all esp
    auto itr = pack_map.begin();
    while (itr != pack_map.end()) {
        if ((int)itr->second->getRssi().size() != N ) {
            itr = pack_map.erase(itr);
        } else {
            ++itr;
        }
    }
}

int Server::readfile_tomaps() {

    FILE* fp;
    char file[20];
    int i;
    int rssi, sq_ctrl, esp_id;
    char mac[20], hash[32], ssid[30] , date[20], timestamp[20];

    for (i = 0; i < N; i++) {

        sprintf(file, "sniff%d.txt", i+1);
        fp = fopen(file, "r");
        if (!fp) {
            fprintf(stderr, "Impossibile aprire file %s\n", file);
            setError(error = "Cannot open file " + QString::fromStdString(file));
            return 0;
        }

        //Salvo nella mappa degli hash (hash, +1)
        //Salvo nella mappa dei pacchetti (hash , pack) aggiungendo ogni volta RSSI di ogni scheda

        while (fscanf(fp, "%s %d %s %s %d %d %s", mac, &sq_ctrl, date, timestamp, &esp_id, &rssi, hash) != EOF) {

            fgets(ssid, 50, fp);

            //SE NON È ANCORA PRESENTE L'HASH NELLA MAPPA LO AGGIUNGO
            if (hash_map.find(hash) == hash_map.end() ) {
                hash_map[hash] = 1;
            }
            //SE È GIÀ PRESENTE LO AGGIORNO
            else {
                hash_map[hash]++;
            }

            //SALVO I DATI DEL PACCHETTO (Uguali per tutte e 4 le schede a parte l'RSSI che va salvato per ogni scheda)
            if (pack_map.find(hash) == pack_map.end() ) {
                Pack* p = new Pack(string(hash), string(mac), rssi, sq_ctrl, string(date), string(timestamp), string(ssid));
                pack_map[hash] = p;
            }
            //Se è già presente il pacchetto devo solo aggiungere l'rssi nella posizione espID nel vettore dei segnali di Pack p
            else {
                Pack* p = pack_map[hash];
                p->addRSSI(rssi);
                pack_map[hash] = p;
            }
        }
    }
    return 1;
}

string Server::read_mac(int sock) {

    char mac[MAC_SIZE];
    fd_set cset;
    struct timeval tval;

    if(start_select(sock, cset, tval, true) <= 0) {
        //true wait for 80 seconds
        fprintf(stderr, "IMPOSSIBLE TO RECIVE MAC ADDRESS FROM ESP\n");
        setError(error = "IMPOSSIBLE TO RECIVE MAC ADDRESS FROM ESP");
        exit(0);
    }

    if (read(sockconn, mac, MAC_SIZE) <= 0) {
        printf("recv failed - Ricezione mac fallita\n");
        setError(error = "recv failed - Ricezione mac fallita");
        send_ack(sock, false);
        exit(0);
    }
    send_ack(sock,true);

    mac[MAC_SIZE-1] = '\0';

    printf("ESP %s connected" , mac);
    
    return string(mac);
}

int Server::send_id(int sock, uint16_t id) {

    uint16_t id_net = htons(id);
    if(send(sock, &id_net , SIZE_ID, 0) < SIZE_ID){
        printf("ERRORE INVIO ID=%u alla scheda\n", id);
        setError(error = "Cannot send ID=" + QString::number(id) + "to ESP");
        return 0;
    } else {
        // printf("ID=%u assegnato alla scheda\n", id);
        return 1;
    }
}

int Server::send_time(int sock) {

    struct timeval tv;
    gettimeofday(&tv, NULL);
    tv.tv_sec += 7200; //+1 of GMT + 1 of legal hour
    if (write(sock, &tv, sizeof(struct timeval)) <= 0) {
        printf("ERRORE INVIO TIMESTAMP alla scheda\n");
        setError(error = "Error in sending timestamp to ESP");
        return 0;
    } else
        return 1;
}

int Server::start_select(int s, fd_set cset, struct timeval tval, bool size) {
    FD_ZERO(&cset);
    FD_SET(s, &cset);
    if(size)
        tval.tv_sec = TIMEOUT_SIZE;
    else
        tval.tv_sec = TIMEOUT_PACK;

    tval.tv_usec = 0;

    return select(FD_SETSIZE, &cset, NULL, NULL, &tval);
}

int Server::find_mac(string mac) {
    int i;

    for(i = 0 ; i< N ; i++) {
        if(vector_mac[i] == mac) {
            return i;
        }
    }
    return ERROR_ID ;
}

void Server::show_error(int err) {

    cout << "PACKS WERE NOT SAVED ON DB CAUSE ERROR:" << endl;

    switch(err) {
    case 1:
        fprintf(stderr, "\t-ERROR OPENING FILE\n");
        setError(error = "\t-ERROR OPENING FILE\n");
        break;
    case 2:
        fprintf(stderr, "\t-ERROR FROM RECEIVING DATA\n");
        setError(error = "\t-ERROR FROM RECEIVING DATA\n");
        break;
    case 3:
        fprintf(stderr, "\t-ERROR FROM SENDING DATA\n");
        setError(error = "\t-ERROR FROM SENDING DATA\n");
        break;
    case 4:
        fprintf(stderr, "\t-ESP NOT REACHABLE\n");
        setError(error = "\t-ESP NOT REACHABLE\n");
        break;
    default:
        fprintf(stderr, "\t-ERROR\n");
        setError(error = "\t-ERROR\n");
    }
}

void Server::send_ack(int sockconn, bool error) {

    if (send(sockconn, &error, sizeof(bool), 0) < 0) {
        fprintf(stderr,"IMPOSSIBLE TO SEND ACK (%d)\n" , error);
        setError(this->error = "IMPOSSIBLE TO SEND ACK " + error);
    }
}

bool Server::read_ack(int sock) {

    bool ack = true;
    if(read(sock, (void*)&ack, sizeof(bool)) <= 0) {
        fprintf(stderr, "\nIMPOSSIBLE READ ACK FROM ESP\n(PID:%d)\n" , getpid());
        setError(error = "IMPOSSIBLE READ ACK FROM ESP (PID:" + QString::number(getpid()) + ")");
    }
    return ack;
}

void Server::stop() {
    hash_map.clear();
    pack_map.clear();
    connected.clear();
    close(socketfd);
}

void Server::close_s() {
    if (socketfd == -1) {
        return;
    }
    close(socketfd);
}


void Server::setError(const QString &error) {
    rwp->setError(error);
}

void Server::setLog(const QString &value) {
    d->setData(value);
}

Server::~Server() {
    analyzer = nullptr;
    close_s();
}
