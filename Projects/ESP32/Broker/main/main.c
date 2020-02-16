/*
 * main.c
 *
 *  Created on: Apr 27, 2018
 *      Author: Eng. Mohamed
 *Main Controller Node has the responsibility of Controlling the other nodes
 *and Send the Data to be displayed on ThingSpeak
 *and So that it has Server Task for each Node , Client task for each node
 *and client for the Server Mirror Node that sends to Cloud:
 *      NodeID: M
 *      Frame to Server Mirror:ID,Wire,Temperature,Alarm
 */
#include "main.h"



//*****************************************
void app_main(void){
	 //Initialize NVS
	   esp_err_t ret = nvs_flash_init();
	   if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
	      ESP_ERROR_CHECK(nvs_flash_erase());
	      ret = nvs_flash_init();
	    }
	    ESP_ERROR_CHECK(ret);
		wifi_conn_init();
		for(int i=0;i<10000;i++);
	xTaskCreatePinnedToCore(client_task, "client_task", 6000, NULL, 4, NULL,0);
	xTaskCreatePinnedToCore(server_4wire, "server_4wire", 4000, NULL, 4, NULL,0);
	xTaskCreatePinnedToCore(server_4Temp, "server_4Temp", 6000, NULL, 4, NULL,1);
	xTaskCreatePinnedToCore(server_4Android, "server_4Android", 4000, NULL, 4, NULL,1);

}
