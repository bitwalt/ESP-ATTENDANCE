ESP ATTENDANCE PROJECT

Require: Ubuntu, ESP32 Module (2 to 4), Sqlite3, QT
Written in: C,C++,SQL

This is an university project able to detect on short/long period an estimate of the number of people inside the intersection area of ESP32 modules. 
It's possibile to use from 2 to 4 ESP modules, for every minute each esp syncronize itself with the server and collect all Wifi Probe-Request packs that can listen in promisc mode, then send all the data in a TCP connection to the Server. 
The server collect data every minute from the ESPs, filter packs not listened from all them, and try to make an estimate of the relative position of each device listened and the total number of device. MAC address with position and number of attendance are saved on a Database, that its possibile to be queried by the provided GUI for search or long period statistics.

-On /run you'll find executable file, mac.txt that contains MAC ADDRESS of ESP module and our template database.

-On /esp_probe_req_sniffer you'll find code for the ESP modules, search on https://docs.espressif.com/projects/esp-idf/ how to flash them.

-On /server_and_analyzer you'll find code for Server, Analyzer, Database in C++ and GUI made with QT libraries. 


Walter Maffione, Stefano Tata, Marco Bergesio, Luigi Napoleone Capasso


Politecnico di Torino - Programmazione di Sistema 2018/2019
