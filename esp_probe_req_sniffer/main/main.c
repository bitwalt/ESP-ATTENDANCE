//////////////////////////////////////
///// BOARD'S EXPECTED BEHAVIOUR /////
//////////////////////////////////////
// Red Light always on.
// Blue light turns on after waiting START_WAIT_TIME and connecting to server.
// Blue light on means the system is working, board is connected to server and time is in sync.
// BLUE LIGHT TURNS OFF: TIME SYNC PROCESS IS NOT RESPONDING. WAIT.
// BLUE LIGHT BLINKS SLOWLY: (1)SYNC OR (2)SOCKET OPENING TIMED OUT. (1)INTERNET CONNECTION IS LIKELY DOWN OR (2)SOCKET IS NOT REACHABLE.
// BLUE LIGHT BLINKS FAST: SERVER ERROR. BOARD REBOOTS IN REBOOT_WAIT_TIME (defined in setup.c).
//////////////////////////////////////
/////        GENERAL NOTES       /////
//////////////////////////////////////
// Watchdog timer has to be turned off to prevent error messages to be displayed during long cycles. See instructions below.
// Start mingw32.exe, navigate to the project folder and type 'make menuconfig'.
// Component config -> ESP32-specific -> Uncheck "Interrupt watchdog" (Space bar). Same for the following line.
#include "setup.h"
#include "data.h"

#define LOCAL 0
#define INTERNET 1
#define START_WAIT_TIME 10
#define CONNECTION_TIMEOUT 1000
#define BLINK_PERIOD_WAITSTATE 500
#define STACK_SIZE 4096 // 4096 words * 32 bits = 16 kB stack size
#define LED_GPIO_PIN GPIO_NUM_2
#define CONNECTION_TYPE LOCAL

extern int done_syncing;
extern int done_sending;
extern time_t now;
extern struct tm timeinfo;
extern int no_connection;
extern int no_connection_status_handled;

void app_main() {

	TaskHandle_t xHandle_dontcare = NULL;
	TaskHandle_t xHandle = NULL;

	gpio_set_level(LED_GPIO_PIN, 0);
	ESP_LOGI("Status", "Starting in %d seconds", START_WAIT_TIME);

	for (int i = START_WAIT_TIME; i > 0; i--) {

		ESP_LOGI("Starting", "%d", i);
		vTaskDelay(1000/portTICK_PERIOD_MS);

	}

	begin_setup();

	while (1) {

		struct tm timeinfo_old;

		gpio_set_level(LED_GPIO_PIN, 0);
		loop_setup();
		ESP_LOGI("Status", "Time syncing");
		if (CONNECTION_TYPE == INTERNET)
			xTaskCreate(sntp, "Sync Internet", STACK_SIZE, (void*)1, configMAX_PRIORITIES, &xHandle_dontcare);
		else
			xTaskCreate(sync_time, "Sync Local", STACK_SIZE, (void*)1, configMAX_PRIORITIES, &xHandle_dontcare);
		xTaskCreate(timer, "Timer", STACK_SIZE, (void*)CONNECTION_TIMEOUT, configMAX_PRIORITIES, &xHandle);

		while (!done_syncing) {

			if (no_connection & !no_connection_status_handled) {

				xTaskCreate(blue_blink, "Blink", STACK_SIZE, (void*)BLINK_PERIOD_WAITSTATE, configMAX_PRIORITIES, &xHandle);
				if (CONNECTION_TYPE == INTERNET)
					ESP_LOGE("Syncing timeout", "%s took too much to respond", sntp_getservername(0));
				else
					ESP_LOGE("Syncing timeout", "Server took too much to respond");
				no_connection_status_handled = true;

			}

		}

		if (no_connection) {

			no_connection = false;
			no_connection_status_handled = false;

		}

		vTaskDelete(xHandle);
		gpio_set_level(LED_GPIO_PIN, 1);

		ESP_LOGI("Status", "Collecting data");
		timeinfo_old = timeinfo;
		time(&now);
		localtime_r(&now, &timeinfo);

		// Collecting data until current minute ends
		while (timeinfo.tm_min == timeinfo_old.tm_min) {

			time(&now);
			localtime_r(&now, &timeinfo);

		}

		done_syncing = false;
		done_sending = false;

		xTaskCreate(send_data, "Send", STACK_SIZE, (void*)1, configMAX_PRIORITIES, &xHandle_dontcare); // Sending data to server

		while (!done_sending) {

			// Waiting for sending process to terminate

		}

	}

}
