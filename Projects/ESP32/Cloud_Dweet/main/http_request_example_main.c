/* HTTP GET Example using plain POSIX sockets

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "driver/spi_master.h"

/* The examples use simple WiFi configuration that you can set via
   'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define WIFI_SSID "AMIT_KITS"
#define WIFI_PASS "159753159"
//static IP
#define DEVICE_IP  "192.168.3.7"
#define DEVICE_GW  "192.168.3.2"
#define DEVICE_NETMASK  "255.255.255.0"
/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;

/* Constants that aren't configurable in menuconfig */
#define WEB_SERVER "dweet.io"
#define WEB_PORT 80
#define WEB_URL "http://dweet.io"
#define DATA "/dweet/for/Broker?Temperature="
static const char *TAG = "DWEET";

static const char *REQUEST = "GET "DATA;
char SEND_BUF[200]="";
char temperature[5]="";

static const char tail[100]=" HTTP/1.0\r\n"
							"Host: "WEB_SERVER"\r\n"
							"\r\n";
//HW SPI
#define MAX6675_MISO 19
#define MAX6675_SCK 18
#define MAX6675_CS 5
#define MAX6675_SPI_HOST   HSPI_HOST  // VSPI_HOST
#define GPIO_OUTPUT_PIN_MASK  ((1<<MAX6675_CS))

#ifndef HIGH
    #define HIGH 1
#endif
#ifndef LOW
    #define LOW 0
#endif

static spi_device_handle_t spi_handle; // SPI handle.
uint16_t data,temp=0;
    spi_bus_config_t bus_config;
	spi_device_interface_config_t dev_config;
	spi_transaction_t trans_word;


static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void)
{

    tcpip_adapter_init();
    tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA);
    	 tcpip_adapter_ip_info_t ipInfo;

    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
	 inet_pton(AF_INET, DEVICE_IP, &ipInfo.ip);
	 inet_pton(AF_INET, DEVICE_GW, &ipInfo.gw);
	 inet_pton(AF_INET, DEVICE_NETMASK, &ipInfo.netmask);
	 tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA, &ipInfo);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

uint8_t sync;
/***************************** */
 void client_task(void *p){

struct sockaddr_in server;
int s;
xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,false, true, portMAX_DELAY);

		ESP_LOGI(TAG, "Enter Client Task");
//receive from Data concatenation task using Queue or Global Variable

while(1)
{
	while(!sync);
	sync=0;

	itoa(temp,temperature,10);
	printf("Data to be Send: %s,size of Data=%d\n",temperature, strlen(temperature));

		strcat(SEND_BUF,REQUEST);
		strcat(SEND_BUF,temperature);
		strcat(SEND_BUF,tail);

		server.sin_addr.s_addr = inet_addr("3.216.169.188");
		server.sin_family = AF_INET;
		server.sin_port = htons( 80 );

		s = socket(AF_INET, SOCK_STREAM, 0);
	   if(s < 0)
	   {
		printf("Error 2\n");
		   ESP_LOGE(TAG, "... Failed to allocate socket.");
		   close(s);
           //continue;
		   vTaskDelay(100 / portTICK_PERIOD_MS);
	   }
	   ESP_LOGI(TAG, "... allocated socket\r\n");
	  if(connect(s , (struct sockaddr *)&server , sizeof(server))!= 0)
	  {
		printf("Error connect\n");
		   ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
		   close(s);
		   vTaskDelay(100 / portTICK_PERIOD_MS);
       }
	   printf("Connected\n");
	   ESP_LOGI(TAG, "... connected");

	   if (send(s,SEND_BUF, strlen(SEND_BUF),0) < 0)
	   {
		 printf("Error Send\n");
		   ESP_LOGE(TAG, "... socket send failed");
		   close(s);

		   vTaskDelay(100 / portTICK_PERIOD_MS);
	   }

	   	   char Reply[100];
		 bzero(Reply, sizeof(Reply));
		 read(s, Reply, sizeof(Reply)-1);
		 printf("Server Reply:%s Size%d\n",Reply,strlen(Reply));

		 close(s);
		 bzero(SEND_BUF, strlen(SEND_BUF));
		// sync=0;
		vTaskDelay(2000/portTICK_PERIOD_MS);

}
}//task

//*******************

 static void init_gpio() {
 	gpio_config_t io_conf;

     io_conf.intr_type = GPIO_PIN_INTR_DISABLE; //disable interrupt
     io_conf.mode = GPIO_MODE_OUTPUT; //set as output mode
     io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_MASK; //bit mask of the pins
     io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE; //disable pull-down mode
     io_conf.pull_up_en = GPIO_PULLUP_DISABLE;   //disable pull-up mode
     ESP_LOGD(TAG, "gpio_config");
     gpio_config(&io_conf); //configure GPIO with the given settings

 }
 //********************8


 //*********************************
 void disable_spi(){
 	 ESP_LOGD(TAG, "spi_bus_remove_device");
 	  //  ESP_ERROR_CHECK(spi_bus_remove_device(spi_handle));
 	 spi_bus_remove_device(spi_handle);
 	    ESP_LOGD(TAG, "spi_bus_free");
 	    //ESP_ERROR_CHECK(spi_bus_free(MAX6675_SPI_HOST));
 	   spi_bus_free(MAX6675_SPI_HOST);
 }
 //*****************************
 uint16_t readMax6675() {
 	uint16_t data,rawtemp,temp=0;
     spi_bus_config_t bus_config;
 	spi_device_interface_config_t dev_config;
 	spi_transaction_t trans_word;

 	ESP_LOGD(TAG, "readMax6675 start");

     gpio_set_level(MAX6675_CS, HIGH); // MAX6675_CS
 	bus_config.sclk_io_num   = MAX6675_SCK; // CLK
 	bus_config.mosi_io_num   = -1; // MOSI not used
 	bus_config.miso_io_num   = MAX6675_MISO; // MISO
 	bus_config.quadwp_io_num = -1; // not used
 	bus_config.quadhd_io_num = -1; // not used
     ESP_LOGD(TAG, "spi_bus_initialize");
 	 //ESP_ERROR_CHECK(spi_bus_initialize(MAX6675_SPI_HOST, &bus_config, 2));//
 	spi_bus_initialize(MAX6675_SPI_HOST, &bus_config, 2);
 	dev_config.address_bits     = 0;
 	dev_config.command_bits     = 0;
 	dev_config.dummy_bits       = 0;
 	dev_config.mode             = 0; // SPI_MODE0
 	dev_config.duty_cycle_pos   = 0;
 	dev_config.cs_ena_posttrans = 0;
 	dev_config.cs_ena_pretrans  = 0;
 	//dev_config.clock_speed_hz   = 2000000;  // 2 MHz
 	dev_config.clock_speed_hz   = 10000;  // 10 kHz
 	dev_config.spics_io_num     = -1; // CS External
 	dev_config.flags            = 0; // SPI_MSBFIRST
 	dev_config.queue_size       = 100;
 	dev_config.pre_cb           = NULL;
 	dev_config.post_cb          = NULL;
     ESP_LOGD(TAG, "spi_bus_add_device");
 	//ESP_ERROR_CHECK(spi_bus_add_device(MAX6675_SPI_HOST, &dev_config, &spi_handle));
 	spi_bus_add_device(MAX6675_SPI_HOST, &dev_config, &spi_handle);


     ESP_LOGD(TAG, "MAX6675_CS prepare");
     gpio_set_level(MAX6675_CS, LOW); // MAX6675_CS prepare
     vTaskDelay(500 / portTICK_RATE_MS);  // see MAX6675 datasheet

     rawtemp = 0x000;
     data = 0x000;  // write dummy

 	//trans_word.address   = 0;
 	//trans_word.command   = 0;
 	trans_word.flags     = 0;
 	trans_word.length    = 8 * 2; // Total data length, in bits NOT number of bytes.
 	trans_word.rxlength  = 0; // (0 defaults this to the value of ``length``)
 	trans_word.tx_buffer = &data;
 	trans_word.rx_buffer = &rawtemp;
    ESP_LOGD(TAG, "spi_device_transmit");
 //	ESP_ERROR_CHECK(spi_device_transmit(spi_handle, &trans_word));
 	spi_device_transmit(spi_handle, &trans_word);
 	gpio_set_level(MAX6675_CS, HIGH); // MAX6675_CS prepare

     temp = ((((rawtemp & 0x00FF) << 8) | ((rawtemp & 0xFF00) >> 8))>>3)*25;
    // temp = ((rawtemp)>>3)*25;
 	ESP_LOGI(TAG, "readMax6675 spiReadWord=%x temp=%d.%d",rawtemp,temp/100,temp%100);

     ESP_LOGD(TAG, "spi_bus_remove_device");
     //ESP_ERROR_CHECK(spi_bus_remove_device(spi_handle));
     spi_bus_remove_device(spi_handle);
     ESP_LOGD(TAG, "spi_bus_free");
    // ESP_ERROR_CHECK(spi_bus_free(MAX6675_SPI_HOST));
     spi_bus_free(MAX6675_SPI_HOST);

     return temp;

 }
 //*************************
 uint16_t Temperature=0;

 static void temp_get_task(void *pvParameters) {

 	while(1) {


 		temp=readMax6675();

 		sync=1;
 		if(temp==0){
 			disable_spi();
 			goto END;
 		}
 	   ESP_LOGI(TAG, "Temp Task temp=%d.%d",temp/100,temp%100);
 		vTaskDelay(1000 / portTICK_RATE_MS);
     }
     END:
 	   //disable_spi();
 	   ESP_LOGI(TAG, "End Temp Task");

     vTaskDelete(NULL);
 }
 //*******************

void app_main()
{
    ESP_ERROR_CHECK( nvs_flash_init() );
      init_gpio();
    //initialise_wifi();
    xTaskCreate(&temp_get_task, "Temperature", 9000, NULL, 3, NULL);
   // xTaskCreate(&client_task, "client_task", 4096, NULL,4, NULL);

}
