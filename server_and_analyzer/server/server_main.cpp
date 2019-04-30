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
#include "Server.h"
#include "Position.h"
#include "server_main.h"

using namespace std;
/*
int N, PORT;

vector<Position*> pos;
*/
server_main::server_main() {

}
/*
int server_main::exist_db(char* name){
    struct stat buf;
    printf("Searching file: %s\n" , name);
    return (stat(name, &buf) == 0);
}

int server_main::read_configuration(){
    FILE *fp;
        fp = fopen("config.txt" , "r");
        if(!fp){
            printf("IMPOSSIBILE APRIRE FILE\n");
            exit(1);
        }

        fscanf(fp, "%d\n" , &N);
        fscanf(fp, "%d", &PORT );
        int x, y ;

        for(int i = 0 ; i < N ; i++){
            fscanf(fp, "%d %d" , &x, &y);
            Position* p = new Position(x,y);
            pos.push_back(p);
        }

        switch (N){

            case 1:
                break;

            case 2:
                printf("AREA DI INTERSEZIONE È UNA RETTA\n");
                break;
            case 3:
                printf("AREA DI INTERSEZIONE È UN TRIANGOLO\n");
                break;
            case 4:
                printf("AREA DI INTERSEZIONE È UN RETTANGOLO\n");
                break;
            default:
                printf("AREA DI INTERSEZIONE È UN POLIGONO");
                break;
        }


        printf("------STARTING PROGRAM------\n\t"
               "NUMBER OF DEVICES = %d\n"
               "\tPORT NUMBER: %d\n"
               "----------------------------\n" , N , PORT);
        //TO ADD: TAKE POSITION OF EACH ESP MODULE
        fclose(fp);
        return N;
}

void server_main::begin() {
    //1. READ CONFIGURATION
    cout << "Sto iniziando il server_main" << endl;
    N = server_main::read_configuration();

    //2. Start of GUI - MVC (MODEL-VIEW-CONTROLLER) PATTERN?

    //3. Here i create a DATABASE (if not already exist..)
    char database_name[20];
    Database* db;
    sprintf(database_name, "sniff.db");
    if(!exist_db(database_name)){
        printf("File NOT found!\n");
        db = new Database(false); //false create a new database
    }
    else {
        printf("File found!\n");
        db = new Database(true); //true get the existing one
    }

    //4. Here i create the Analyzer
    Analyzer* analyzer = new Analyzer(N, db, pos);
    analyzer->init();

    //5. Here i create the SERVER
    cout << "CREATING SERVER..." << endl;
    Server* server = new Server(PORT, N, analyzer);
    int err = server->init();
    cout << "Server successfully created!" << endl;

    if (err != 0) {
        cerr << strerror(err) << endl;
        exit(err);
    }


    //6. Launch Server in background
    while(!server->start()) {
           server->clean_all();
           server->init();
          // server->start();
     }

    return;
}
*/
