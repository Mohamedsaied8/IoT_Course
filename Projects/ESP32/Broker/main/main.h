#include <stdlib.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>

#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_heap_alloc_caps.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <lwip/sockets.h>
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include <esp_log.h>
#include "lwip/err.h"
#include "apps/sntp/sntp.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include "string.h"
#include "freertos/croutine.h"

#include "esp_err.h"
#include "esp_log.h"
#include "tcpip_adapter.h"


#define  SSID 	"AMIT_KITS"
#define  PASS 	"159753159"
//static IP
#define DEVICE_IP  "192.168.1.111"
#define DEVICE_GW  "192.168.1.1"
#define DEVICE_NETMASK  "255.255.255.0"

#define TAG "Main_Controller"
#define delm   ","
#define GPIO_4 4

//IPS
#define MIRROR_NODE "192.168.1.17"
#define TEMP_NODE "192.168.1.113"
#define WIRE_NODE "192.168.1.114"
#define ANDROID   "192.168.1.18"
//PORTS
#define WIRE_PORT 1000
#define TEMP_PORT 2000
#define ANDROID_PORT 3000
//variables

EventGroupHandle_t wifi_event_group;

//tcpip_adapter_ip_info_t ipInfo;
void vApplicationIdleHook( void );
void wifi_conn_init(void);
void server_4wire(void *p);
void server_4Temp(void *p);
void server_4Android(void *p);
void client_task(void *p);
void cloud_task(void *p);

