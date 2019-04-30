#include "setup.h"

#define PROBE_REQ 0x04
#define ROWS 1000
#define COLUMNS 128
#define STACK_SIZE 4096
#define LED_GPIO_PIN GPIO_NUM_2
#define SELECT_TIMEOUT 30
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

#pragma pack(1)

char to_send[ROWS][COLUMNS]; // Every line is a packet to be sent (max 58 bytes)
int packets_no = 0; // Number of received packets
int sock;
extern int done_syncing;
extern int done_sending;
extern time_t now;
extern struct tm timeinfo;
extern char timestamp[100];
extern int connected;
extern uint16_t board_no;
extern int n;
extern bool ack;
int n;
bool ack = false;
fd_set cset;
struct timeval tval;
uint32_t h0, h1, h2, h3; // MD5#

typedef struct {

    unsigned protocol_version:2;
    unsigned type:2;
    unsigned subtype:4;
    unsigned to_ds:1;
    unsigned from_ds:1;
    unsigned more_flag:1;
    unsigned retry:1;
    unsigned pwr_mgt:1;
    unsigned more_data:1;
    unsigned wep:1;
    unsigned order:1;

} frame_ctrl;

typedef struct {

	uint8_t src[6]; // Source (in header.addr2)
	uint8_t dest[6];
 	uint8_t ssid[30];
 	unsigned timestamp:32; // Timestamp (in packet.bd.timestamp)
 	unsigned sequence_ctrl:16; // Sequence number
 	signed rssi:8; // Signal strength (in wifi_promiscuous_pkt_t.rx_ctrl.rssi)
 	unsigned channel:4;
 	frame_ctrl frame_control; // Frame control

} data;

typedef struct {

	frame_ctrl fctrl;
	unsigned duration_id:16;
	uint8_t addr1[6]; // Receiver address
	uint8_t addr2[6]; // Sender address
	uint8_t addr3[6]; // Filtering address
	unsigned sequence_ctrl:16;

} header;

typedef struct {

	uint8_t id;
	uint8_t length;
	uint8_t payload[0]; // SSID (if length>0) and other stuff. payload[0] means its length is variable.

} management;

typedef struct {

	header hdr;
	management mgmt;

} packet;

void handler(void* buff, wifi_promiscuous_pkt_type_t type) {

	ESP_ERROR_CHECK(esp_wifi_set_event_mask(WIFI_EVENT_MASK_ALL));

	uint8_t *p;
	const wifi_promiscuous_pkt_t* ppkt = (wifi_promiscuous_pkt_t*) buff;
	const packet* ipkt = (packet*) ppkt->payload;
	const header* hdr = (header*) &ipkt->hdr;
	const frame_ctrl* fctrl = (frame_ctrl*) &hdr->fctrl;

	if (type != WIFI_PKT_MGMT || fctrl->subtype != PROBE_REQ || !done_syncing || !done_sending)
		return;

	time(&now);
	localtime_r(&now, &timeinfo);
	strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M:%S", &timeinfo);

	const management* mgmt = (management*) &ipkt->mgmt;

	char ssid_tmp[31];
	memset(ssid_tmp, 0, sizeof(ssid_tmp));

	if (mgmt->length > 0) {

		int i;
		for (i = 0; i < mgmt->length && i <= 30; i++)
			ssid_tmp[i] = mgmt->payload[i];
		ssid_tmp[i] = '\0';

	}

	else
		strcpy(ssid_tmp, "N/A");

	memset(to_send[packets_no], 0, sizeof(to_send[packets_no]));

	sprintf(to_send[packets_no], "%02x:%02x:%02x:%02x:%02x:%02x %05d %s %u %03d ",
			hdr->addr2[0], hdr->addr2[1], hdr->addr2[2], hdr->addr2[3], hdr->addr2[4], hdr->addr2[5],
			hdr->sequence_ctrl,
			timestamp,
			board_no,
			ppkt->rx_ctrl.rssi);

	char *tmp;
	tmp = malloc(60);
	sprintf(tmp, "%02x:%02x:%02x:%02x:%02x:%02x%05d%s",
			hdr->addr2[0], hdr->addr2[1], hdr->addr2[2], hdr->addr2[3], hdr->addr2[4], hdr->addr2[5],
			hdr->sequence_ctrl,
			ssid_tmp);
	ssize_t len = strlen(tmp);

	md5(tmp, len);

	char tmp1[8];
    p = (uint8_t*)&h0;
    sprintf(tmp1, "%2.2x%2.2x%2.2x%2.2x", p[0], p[1], p[2], p[3]);
    strcpy(to_send[packets_no], strcat(to_send[packets_no], tmp1));

    p = (uint8_t*)&h1;
    sprintf(tmp1, "%2.2x%2.2x%2.2x%2.2x", p[0], p[1], p[2], p[3]);
    strcpy(to_send[packets_no], strcat(to_send[packets_no], tmp1));

    p = (uint8_t*)&h2;
    sprintf(tmp1, "%2.2x%2.2x%2.2x%2.2x", p[0], p[1], p[2], p[3]);
    strcpy(to_send[packets_no], strcat(to_send[packets_no], tmp1));

    p = (uint8_t*)&h3;
    sprintf(tmp1, "%2.2x%2.2x%2.2x%2.2x ", p[0], p[1], p[2], p[3]);
    strcpy(to_send[packets_no], strcat(to_send[packets_no], tmp1));

	strcpy(to_send[packets_no], strcat(to_send[packets_no], ssid_tmp));

	free(tmp);

	packets_no++;

	ESP_ERROR_CHECK(esp_wifi_set_event_mask(WIFI_EVENT_MASK_NONE));

}

void send_data(void* pvParameters) {

	configASSERT(((uint32_t)pvParameters) == 1);

	if (!connected)
		vTaskDelete(NULL);

	int size = htonl(packets_no);

	ESP_LOGI("Packets received", "%d", packets_no);
	ESP_LOGI("Status", "Sending data");
	if (write(sock, &size, sizeof(int)) != sizeof(int))
		escape("WRITE ERROR");
	if ((n = start_select()) == -1)
		escape("SELECT ERROR 3");

	if (n > 0) {

		if (read(sock, (void*)&ack, sizeof(bool)) != sizeof(bool))
			escape("READ ERROR");
		if (!ack)
			escape("RECEIVE ERROR 3");

	} else
		escape("SOCKET ERROR 3");

	if (write(sock, to_send, packets_no * COLUMNS) < packets_no * COLUMNS)
		escape("WRITE ERROR");

	vTaskDelay(1000/portTICK_PERIOD_MS);

	if ((n = start_select()) == -1)
		escape("SELECT ERROR 4");

	if (n > 0) {

		if (read(sock, (void*)&ack, sizeof(bool)) != sizeof(bool))
			escape("READ ERROR");
		if (!ack)
			escape("RECEIVE ERROR 4");

	} else
		escape("SOCKET ERROR 4");

	ESP_LOGI("Sending data", "SUCCESS!");
	close(sock);
	packets_no = 0;
	done_sending = true;

	vTaskDelete(NULL);

}

int start_select() {

	FD_ZERO(&cset);
	FD_SET(sock, &cset);
	tval.tv_sec = SELECT_TIMEOUT;
	tval.tv_usec = 0;
	int ret_val = select(FD_SETSIZE, &cset, NULL, NULL, &tval);
	FD_CLR(sock, &cset);
	return ret_val;

}

void md5(char *initial_msg, size_t initial_len) {

	uint8_t *msg = NULL;

	uint32_t r[] = {

			7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
			5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
			4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
			6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21

	};

	uint32_t k[] = {

			0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
			0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
			0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
			0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
			0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
			0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
			0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
			0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
			0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
			0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
			0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
			0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
			0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
			0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
			0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
			0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391

	};

	h0 = 0x67452301;
	h1 = 0xefcdab89;
	h2 = 0x98badcfe;
	h3 = 0x10325476;

	int new_len;
	for(new_len = initial_len*8 + 1; new_len%512!=448; new_len++);
	new_len /= 8;

	msg = calloc(new_len + 64, 1);
	memcpy(msg, initial_msg, initial_len);
	msg[initial_len] = 128;

	uint32_t bits_len = 8*initial_len;
	memcpy(msg + new_len, &bits_len, 4);

	int offset;
	for(offset=0; offset<new_len; offset += (512/8)) {

		uint32_t* w = (uint32_t*)(msg + offset);

		uint32_t a = h0;
		uint32_t b = h1;
		uint32_t c = h2;
		uint32_t d = h3;

		uint32_t i;

		for(i = 0; i < 64; i++) {

			uint32_t f, g;

			if (i < 16) {

				f = (b & c) | ((~b) & d);
				g = i;

			} else if (i < 32) {

				f = (d & b) | ((~d) & c);
				g = (5*i + 1) % 16;

			} else if (i < 48) {

				f = b ^ c ^ d;
				g = (3*i + 5) % 16;

			} else {

				f = c ^ (b | (~d));
				g = (7*i) % 16;

			}

			uint32_t temp = d;
			d = c;
			c = b;
			b = b + LEFTROTATE((a + f + k[i] + w[g]), r[i]);
			a = temp;

		}

		h0 += a;
		h1 += b;
		h2 += c;
		h3 += d;

	}

	free(msg);

}
