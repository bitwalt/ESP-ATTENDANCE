#include "system.h"

using namespace std;

extern datastream *d;
extern RuntimeWindow* rwp;

System::System() {
}

int System::exist_db(char* name) {
    struct stat buf;
    printf("Searching file: %s\n", name);
    setLog(log = "Searching file: " + QString::fromStdString(name) + "\n");
    return (stat(name, &buf) == 0);
}

int System::read_configuration() {
    FILE *fp;
    fp = fopen("config.txt" , "r");
    if(!fp){
        printf("IMPOSSIBLE OPEN FILE CONFIG\n");
        QString str = "IMPOSSIBLE OPEN FILE CONFIG\n";
        rwp->setError(str);
        exit(1);
    }

    fscanf(fp, "Number of ESP modules: %d\n" , &N);
    fscanf(fp, "PORT NUMBER: %d\n", &PORT );

    int x, y;

    for(int i = 0 ; i < N ; i++) {
        fscanf(fp, "%d %d" , &x, &y);
        Position* p = new Position(x,y);
        pos.push_back(p);
    }

    QDateTime d = QDateTime::currentDateTime();
    setLog(log = "starting_time: " + d.toString("dd/MM/yyyy - hh:mm:ss") + "\n");

    printf("----------STARTING PROGRAM----------\n\t"
           "NUMBER OF ESP MODULES = %d\n"
           "\tPORT NUMBER: %d\n"
           "------------------------------------\n", N, PORT);

    setLog(log = "-----------STARTING PROGRAM-------------\n#ESP MODULES: " + QString::number(N)
            + "\nPORT NUMBER: " + QString::number(PORT) +
            "\n--------------------------------------------------------\n");


    fclose(fp);
    return N;
}

void System::start() {

    //1. READ CONFIGURATION
    N = read_configuration();

    //2. Create a DATABASE (if not already exist..)
    char database_name[20];
    sprintf(database_name, "sniff.db");


    std::cout << "OPENING DATABASE..." << std::endl;
    db = new Database(true);
    setLog(log = "Database opened!\n");

    //3. Create the Analyzer
    analyzer = new Analyzer(N, db, pos);
    analyzer->init();

    //4. Create the SERVER
    cout << "CREATING SERVER..." << endl;
    setLog(log = "CREATING SERVER...\n");

    server = new Server(PORT, N, analyzer);
    int err = server->init();
    cout << "Server successfully created!" << endl;
    setLog(log = "Server successfully created!\n");
    if (err != 0) {
        cerr << strerror(err) << endl;
        exit(err);
    }

    return;
}

void System::sever_start() {
    server->start();
}

void System::setLog(const QString &value) {
    d->setData(value);
}

System::~System() {

    delete server ;
    server = nullptr;
    delete analyzer;
    analyzer = nullptr;
    delete db;
    db = nullptr;

}
