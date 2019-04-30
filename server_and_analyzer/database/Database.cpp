#include "Database.h"

static int callback(void *, int argc, char **argv, char **azColName) {   

    for(int i = 0; i<argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;

}

int Database::open_database(const char *filename) {
    int rc = sqlite3_open(filename, &conn);
    if(rc) {
        printf("Couldn't open database: %s\n", sqlite3_errmsg(conn));
        return -1;
    }
    return rc;
}

void Database::sql_create_table_sniff(char *zErrMsg) {
    char *sql;

    sql = (char *)"CREATE TABLE SNIFF("  \
                  "HASH       CHAR(50)  NOT NULL,"  \
                  "MAC        CHAR(50)   NOT NULL," \
                  "DATE 	    CHAR(50)  NOT NULL,"  \
                  "TIMESTAMP 	CHAR(50)  NOT NULL,"  \
                  "SYSTIME  BIGINT    NOT NULL,"  \
                  "SSID       CHAR(50)  NOT NULL,"  \
                  "X          REAL       NOT NULL,"     \
                  "Y          REAL       NOT NULL)";

    /* Execute SQL statement */
    if(sqlite3_exec(conn, sql, callback, 0, &zErrMsg)!= SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else
        printf("Table successfully created!\n");
}

vector<pair<string, int>> Database::topN(const string &start, const string &start_time, const string &end, const string &end_time, int n_limit){
    int rc;
    char sql[200];
    sqlite3_stmt *stmt;
    vector<pair<string, int>> top;

    sprintf(sql, "SELECT MAC, COUNT(*) AS FREQ\n"
                 "FROM SNIFF\n"
                 "WHERE DATE BETWEEN '%s' AND '%s'\n"
                 "AND TIMESTAMP BETWEEN '%s' AND '%s'\n"
                 "GROUP BY MAC\n"
                 "HAVING FREQ >= %d\n"
                 "ORDER BY FREQ DESC\n" , start.c_str(), end.c_str(), start_time.c_str(), end_time.c_str(), n_limit) ;

    rc = sqlite3_prepare_v2(conn, sql, strlen(sql), &stmt, NULL);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char *mac = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
        int count = sqlite3_column_int(stmt, 1);
        top.push_back(make_pair(mac, count));
    }

    if (rc != SQLITE_DONE) {
        cerr << "SELECT failed: " << sqlite3_errmsg(conn) << endl;
        return top;
    }

    sqlite3_finalize(stmt);
    return top;
}

void Database::get_specific_situation(const string &start, const string &start_time, const string &end, const string &end_time,
                                      map<long, map<string, Position *>> & maps ){
    int rc;
    char sql[300];
    long next = 0;
    sqlite3_stmt *stmt;

    sprintf(sql, "SELECT MAC, SYSTIME, X, Y\n"
                 "FROM SNIFF\n"
                 "WHERE DATE BETWEEN '%s' AND '%s'\n"
                 "AND TIMESTAMP BETWEEN '%s' AND '%s'\n" , start.c_str(), end.c_str(), start_time.c_str(), end_time.c_str()) ;

    rc = sqlite3_prepare_v2(conn, sql, strlen(sql), &stmt, NULL);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char *mac = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
        const long systime = (long) sqlite3_column_double(stmt, 1);
        int x = sqlite3_column_int(stmt, 2);
        int y = sqlite3_column_int(stmt, 3);

        Position *pos = new Position(x, y);

        if(systime != next){
            next = systime;
        }
        //il numero di dispositivi presenti per quel timestamp lo si trova andando a prendere il size della mappa specifica
        maps[next][mac] = pos;

    }
    if (rc != SQLITE_DONE) {
        cerr << "SELECT failed: " << sqlite3_errmsg(conn) << endl;
        return;
    }
    sqlite3_finalize(stmt);
}

void Database::create_database(){
    char *zErrMsg = 0;
    open_database("sniff.db");
    sql_create_table_sniff(zErrMsg);
    create_table_present();
}

bool Database::log_in(const string &name, const string &password){

    int rc;
    char sql[300];
    const char * pass_hash;
    sqlite3_stmt *stmt = NULL;

    sprintf(sql, "SELECT user_name, pass "
                 "FROM USERS "
                 "WHERE user_name = '%s'", name.c_str());

    rc = sqlite3_prepare_v2(conn, sql, strlen(sql), &stmt, NULL);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW){
        pass_hash = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
    }

    if (rc != SQLITE_DONE) {
        cerr << "SELECT failed: " << sqlite3_errmsg(conn) << endl;
        return false;
    }

    sqlite3_finalize(stmt);

    string hash = md5(string(password));

    if(pass_hash == hash)
        return true;

    return false;
}

void Database::populate_db(map<string, Pack *> mac_packs , map<string, Position *> &mac_position) {

    char *zErrMsg = 0;
    char *sql;
    string mac;
    string hash, date, timestamp, ssid;
    Position *pos;
    Pack *p;
    double x, y;
    long system_time = time(NULL);

    for (auto &it : mac_position) {

        mac = it.first;
        pos = it.second;
        x = pos->getX();
        y = pos->getY();
        p = mac_packs[mac];
        hash = p->getHash();
        date = p->getDate();
        timestamp = p->getTimestamp();
        ssid = p->getSsid();

        sql = sqlite3_mprintf("INSERT INTO SNIFF(HASH, MAC, DATE, TIMESTAMP, SSID, SYSTIME, X , Y) "
                              "VALUES('%s', '%s', '%s', '%s', '%s', %ld, %lf, %lf);",
                              hash.c_str(), mac.c_str(), date.c_str(), timestamp.c_str(), ssid.c_str(), system_time, x,
                              y);

        if (sqlite3_exec(conn, sql, callback, nullptr, &zErrMsg) != SQLITE_OK) {
            printf("SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
    }

    printf("SNIFFED PACKS CORRECTLY SAVED ON DATABASE\n");
}

void Database::create_table_present(){
    char *sql, *zErrMsg;

    sql = (char *)"CREATE TABLE ATTENDANCE("           \
                  "TIMESTAMP  BIGINT     NOT NULL,"  \
                  "PRESENT    INT        NOT NULL)";

    /* Execute SQL statement */
    if(sqlite3_exec(conn, sql, callback, 0, &zErrMsg)!= SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else
        printf("Table successfully created!\n");
}

void Database::save_presents(long timestamp , int present) {

    char *sql, *zErrMsg ;
    sql = sqlite3_mprintf("INSERT INTO ATTENDANCE(TIMESTAMP, PRESENT)"
                          "VALUES('%ld', '%d');",
                          timestamp, present);

    if (sqlite3_exec(conn, sql, callback, nullptr, &zErrMsg) != SQLITE_OK) {
        printf("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
}

Database::~Database(){
    sqlite3_close(conn);
}
