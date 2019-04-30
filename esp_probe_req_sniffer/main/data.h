#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "lwip/sockets.h"
#include "string.h"
#include "sdkconfig.h"
#include "esp_wifi_types.h"
#include "esp_system.h"
#include "esp_event.h"
#include "driver/gpio.h"
#include <freertos/timers.h>
#include "lwip/apps/sntp.h"
#include "lwip/err.h"
#include <stdbool.h>

void handler(void*, wifi_promiscuous_pkt_type_t);
void send_data(void*);
int start_select();
void md5(char* , size_t);
