#include "data.h"

esp_err_t wifi_event_handler(void*, system_event_t*);
void sntp(void*);
void sync_time(void*);
void begin_setup();
void open_socket();
void connect_socket();
void loop_setup();
void blue_blink (void*);
void sta_connect(void*);
void timer(void*);
void escape(char*);
int start_select();
