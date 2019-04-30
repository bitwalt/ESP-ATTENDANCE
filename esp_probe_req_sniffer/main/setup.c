#include "setup.h"

#define WIFI_SSID "UX305FA"
#define WIFI_PASS "ciaociao"
#define PORT 5000
#define REBOOT_WAIT_TIME 10
#define BLINK_PERIOD_EXITSTATE 125
#define STACK_SIZE 4096
#define LED_GPIO_PIN GPIO_NUM_2

time_t now;
struct tm timeinfo;
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;
char timestamp[100];
wifi_config_t sta_config = { .sta = { .ssid = WIFI_SSID, .password = WIFI_PASS, }, };
tcpip_adapter_ip_info_t ip_info;
int connected = false;
TaskHandle_t xHandle_dontcare = NULL;
TaskHandle_t xHandle = NULL;
int done_syncing = false;
int done_sending = true;
int no_connection = false;
int no_connection_status_handled = false;
uint16_t board_no;
extern int sock;
uint8_t my_mac[6];
char my_mac_str[18];
extern int n;
extern bool ack;
struct sockaddr_in serverAddress;

esp_err_t wifi_event_handler(void* ctx, system_event_t* event) {

	switch (event->event_id) {

	        case SYSTEM_EVENT_STA_START:
	            ESP_LOGI("Status", "STA_START");
	            ESP_ERROR_CHECK(esp_wifi_connect());
	            break;
	        case SYSTEM_EVENT_STA_GOT_IP:
	        	ESP_LOGI("Status", "CONNECTED TO SERVER");
	        	connected = true;
	            break;
	        case SYSTEM_EVENT_STA_DISCONNECTED:
	        	escape("CONNECTION WITH SERVER LOST");
	            break;
	        default:
	            break;

	    }

    return ESP_OK;
}

void sntp(void* pvParameters) {

	configASSERT(((uint32_t)pvParameters) == 1);

	if (!connected)
		vTaskDelete(NULL);

	struct timeval tv = {.tv_sec = 0, .tv_usec = 0};
	struct timezone tz = {.tz_minuteswest = 0, .tz_dsttime = 0};
	settimeofday(&tv, &tz);

	while (timeinfo.tm_year > 1970-1900) {

		time(&now);
		localtime_r(&now, &timeinfo);

	}

	sntp_init();

	while (timeinfo.tm_year < 2018-1900) {

		time(&now);
        localtime_r(&now, &timeinfo);

	}

	ESP_LOGI("Time syncing", "SUCCESS!");
	done_syncing = true;

	// Logging time
	strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M:%S", &timeinfo);
	ESP_LOGI("Time", "%s", timestamp);

	sntp_stop();

	vTaskDelete(NULL);

}

void sync_time(void* pvParameters) {

	struct timeval tv;
	struct timezone tz = {.tz_minuteswest = 0, .tz_dsttime = 0};

	if ((n = start_select()) == -1)
		escape("SELECT ERROR");

	if (n > 0) {

		if (read(sock, &tv, sizeof(struct timeval)) != sizeof(struct timeval)) {

			ack = false;
			if (write(sock, &ack, sizeof(bool)) != sizeof(bool))
				escape("WRITE ERROR");
			escape("READ ERROR");

		}

		ack = true;
		if (write(sock, &ack, sizeof(bool)) != sizeof(bool))
			escape("WRITE ERROR");

		settimeofday(&tv, &tz);

		time(&now);
		localtime_r(&now, &timeinfo);

		ESP_LOGI("Time syncing", "SUCCESS!");
		done_syncing = true;

		// Logging time
		strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M:%S", &timeinfo);
		ESP_LOGI("Time", "%s", timestamp);

	} else
		escape("SOCKET ERROR");

	vTaskDelete(NULL);

}

void open_socket() {

	ESP_LOGI("Status", "Opening socket");
    if (!(sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
    	escape("ERROR OPENING SOCKET");
    ESP_LOGI("Socket open", "SUCCESS!");
    ESP_LOGI("RC", "%d", sock);

}

void connect_socket() {

	ESP_LOGI("Status", "Connecting socket");
	serverAddress.sin_family = AF_INET;
	inet_pton(AF_INET, "10.42.0.1"/*"192.168.137.1"*/, &serverAddress.sin_addr.s_addr);
	serverAddress.sin_port = htons(PORT);
	if (connect(sock, (struct sockaddr*)&serverAddress, sizeof(struct sockaddr_in)) != 0)
		escape("ERROR CONNECTING SOCKET");
	ESP_LOGI("Socket connect", "SUCCESS!");

}

void begin_setup() {

	esp_err_t ret = nvs_flash_init();
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {

	      ESP_ERROR_CHECK(nvs_flash_erase());
	      ret = nvs_flash_init();

	}

	esp_log_level_set("wifi", ESP_LOG_NONE);
	ESP_ERROR_CHECK(ret);
	wifi_event_group = xEventGroupCreate();
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

	xTaskCreate(sta_connect, "Connect", STACK_SIZE, (void*)1, configMAX_PRIORITIES, &xHandle_dontcare);

	while (!connected) {

		// Waiting for connection

	}

	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");

	esp_wifi_set_promiscuous(true);
	esp_wifi_set_promiscuous_rx_cb(&handler);

	gpio_pad_select_gpio(LED_GPIO_PIN);
	gpio_set_direction(LED_GPIO_PIN, GPIO_MODE_OUTPUT);

}

void loop_setup() {

	open_socket();
	connect_socket();

	ESP_ERROR_CHECK(esp_wifi_get_mac(0, my_mac));
	sprintf(my_mac_str, "%02x:%02x:%02x:%02x:%02x:%02x", my_mac[0], my_mac[1], my_mac[2], my_mac[3], my_mac[4], my_mac[5]);
	if (write(sock, my_mac_str, strlen(my_mac_str)) != strlen(my_mac_str))
		escape("WRITE ERROR");
	if ((n = start_select()) == -1)
		escape("SELECT ERROR 1");

	if (n > 0) {

		if (read(sock, (void*)&ack, sizeof(bool)) != sizeof(bool))
			escape("READ ERROR");
		if (!ack)
			escape("RECEIVE ERROR 1");

	} else
		escape("SOCKET ERROR 1");

	if ((n = start_select()) == -1)
		escape("SELECT ERROR 2");

	if (n > 0) {

		if (read(sock, (void*)&board_no, sizeof(uint16_t)) != sizeof(uint16_t)) {

			ack = false;
			if (write(sock, &ack, sizeof(bool)) != sizeof(bool))
				escape("WRITE ERROR");
			escape("READ ERROR");

		}

		ack = true;
		if (write(sock, &ack, sizeof(bool)) != sizeof(bool))
			escape("WRITE ERROR");
		board_no = ntohs(board_no);
		ESP_LOGI("Board Number", "%u", board_no);

	} else
		escape("SOCKET ERROR 2");

}

void blue_blink (void* pvParameters) {

	while (1) {

		gpio_set_level(LED_GPIO_PIN, 0);
		vTaskDelay((uint32_t)pvParameters/portTICK_PERIOD_MS);
		gpio_set_level(LED_GPIO_PIN, 1);
		vTaskDelay((uint32_t)pvParameters/portTICK_PERIOD_MS);

	}

	vTaskDelete(NULL);

}

void sta_connect(void* pvParameters) {

	configASSERT(((uint32_t)pvParameters) == 1);

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info));

	vTaskDelete(NULL);

}

void timer(void* pvParameters) {

	if (!connected)
		vTaskDelete(NULL);

	vTaskDelay((uint32_t)pvParameters/portTICK_PERIOD_MS);

	no_connection = true;

	vTaskDelete(NULL);

}

void escape(char* msg) {

	xTaskCreate(blue_blink, "Blink", STACK_SIZE, (void*)BLINK_PERIOD_EXITSTATE, configMAX_PRIORITIES, &xHandle_dontcare);
	ESP_LOGE("Status", "%s", msg);
	connected = false;
	ESP_LOGE("Status", "Rebooting in %d seconds", REBOOT_WAIT_TIME);

	for (int i = REBOOT_WAIT_TIME; i > 0; i--) {

		ESP_LOGE("Rebooting", "%d", i);
		vTaskDelay(1000/portTICK_PERIOD_MS);

	}

	abort();

}
