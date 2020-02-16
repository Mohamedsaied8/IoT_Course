#include "main.h"

const int CONNECTED_BIT = BIT0;
//***************
char UpperT[10],LowerT[10],Wire[10]="-1",Temp[10]="-1";
char alarmT[5]="-1";
char throughputStr[10];
char serv_Reply[200],sendBuf[200],Node_Reply[4],*TEMP;
char Data[10]="M,-1,-1,-1";
char Req[20];
char sync=0;
int throughput;

//************SEND TO SERVER****************************************
void client_task(void *p){
int counter=0;

struct sockaddr_in server;
int s;
  xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
								false, true, portMAX_DELAY);

		//ESP_LOGI(TAG, "Enter Client Task");
//receive from Data concatenation task using Queue or Global Variable
  int iter=0,PORT=0;
  	char IP[50];
  	int16_t LowT=14,UppT=500,T=-1;
while(1)
{

	LowT=atoi(LowerT);
	UppT=atoi(UpperT);
	printf("lower limit%s,upper limit%s",LowerT,UpperT);
	T=atoi(Temp);
	printf("in client task temeprature=%d\n",T);

	if(T==-1){
		strcpy(alarmT,"-1");
	}
	else if(T>UppT||T<LowT){
		strcpy(alarmT,"1");
	}
	else{
		strcpy(alarmT,"0");
	}
      //concatenate data ID,Wire,Temperature,Alarm
 //for ex:M,ON,temp,OFF   M,1,32.8,0
	sprintf(sendBuf,"M,%s,%s,%s,",Wire,Temp,alarmT);
	throughput+=strlen(sendBuf);
	itoa(throughput,throughputStr,10);
	strcpy(Req,sendBuf);
	strcat(Req,throughputStr);
	printf("Send to Mirror=%s\n",Req);
			strcpy(IP,MIRROR_NODE);
			PORT=5000;


		server.sin_addr.s_addr = inet_addr(IP);
		server.sin_family = AF_INET;
		server.sin_port = htons( PORT );

		s = socket(AF_INET, SOCK_STREAM, 0);
		//unsigned long mode=1;
		//ioctlsocket(s,FIONBIO,&mode);
	   if(s < 0)
	   {
		printf("Error 2\n");
		   ESP_LOGE(TAG, "... Failed to allocate socket.");
		   close(s);
		   counter++;
		   //continue;
		   vTaskDelay(100 / portTICK_PERIOD_MS);
	   }
	   ESP_LOGI(TAG, "... allocated socket\r\n");
	  if(connect(s , (struct sockaddr *)&server , sizeof(server))!= 0)
	  {
		printf("Error connect\n");
		   ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
		   close(s);
		   counter++;
		   vTaskDelay(100 / portTICK_PERIOD_MS);
		  // continue;
	   }
	   printf("Connected\n");
	   ESP_LOGI(TAG, "... connected");

	   if (send(s,Req, strlen(Req),0) < 0)
	   {
		 printf("Error Send\n");
		   ESP_LOGE(TAG, "... socket send failed");
		   close(s);

		   vTaskDelay(100 / portTICK_PERIOD_MS);
		  // continue;
	   }
	     throughput=0;
		// bzero(serv_Reply, sizeof(serv_Reply));
		// read(s, serv_Reply, sizeof(serv_Reply)-1);
		// printf("Server Reply:%s Size%d\n",serv_Reply,strlen(serv_Reply));

		 close(s);
		vTaskDelay(2000/portTICK_PERIOD_MS);

}
}//task

//****************************************
static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
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
	printf("555555555555 I am here in wifi handler 555555555555");
	return ESP_OK;
}

//*****************************************
void wifi_conn_init(void)
{

	tcpip_adapter_init();
	tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA);
	 tcpip_adapter_ip_info_t ipInfo;

	 inet_pton(AF_INET, DEVICE_IP, &ipInfo.ip);
	 inet_pton(AF_INET, DEVICE_GW, &ipInfo.gw);
	 inet_pton(AF_INET, DEVICE_NETMASK, &ipInfo.netmask);
	 tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA, &ipInfo);

	wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK( esp_event_loop_init(wifi_event_handler, NULL) );
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );

	wifi_config_t wifi_config = {
		.sta = {
			.ssid =  SSID,
			.password =PASS,
		},
	};
	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
	ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );

	ESP_LOGI(TAG, "start the WIFI SSID:[%s] password:[%s]\n", SSID, PASS);
	ESP_ERROR_CHECK( esp_wifi_start() );

}
//*****************************************
void server_4wire(void *p)
{
	struct sockaddr_in clientAddress;
	struct sockaddr_in serverAddress;
	socklen_t clientAddressLength;
	char recvbuf[10];
	int sock;
	int rc;
	int clientSock;
	char *id, *WIRE, *token;
	while(1)
	{
	xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
						false, true, portMAX_DELAY);
	ESP_LOGI(TAG, "Connected to AP");

	// Create a socket that we will listen upon.
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		ESP_LOGE(TAG, "socket: %d %s", sock, strerror(errno));
		goto END;
	}
	// Bind our server socket to a port.
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(WIRE_PORT);
	rc  = bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
	if (rc < 0) {
		//ESP_LOGE(TAG, "bind: %d %s", rc, strerror(errno));
		goto END;
	}

	// Flag the socket as listening for new connections.
	rc = listen(sock, 30);
	if (rc < 0) {
		//ESP_LOGE(TAG, "listen: %d %s", rc, strerror(errno));
		goto END;
	}

	while (1)
	{

		// Listen for a new client connection.
		clientAddressLength = sizeof(clientAddress);
		clientSock = accept(sock, (struct sockaddr *)&clientAddress, &clientAddressLength);
		if (clientSock < 0)
		{
			//ESP_LOGE(TAG, "accept: %d %s (%d)", clientSock, strerror(errno),errno);
			if (errno!=23)
			{
				goto END;
			}

		}
		//bzero(recv_buf, sizeof(recv_buf));
		bzero(recvbuf,sizeof(recvbuf));
	    read(clientSock, recvbuf, sizeof(recvbuf)-1);
		printf("\nReceived Data  From Wire=%s  size: %d\n",recvbuf,strlen(recvbuf));
		 //send the receive buffer to the SD card task
		throughput+=strlen(recvbuf);
		close(clientSock);


	  if(recvbuf[0]=='W')
		{
		    token = strtok(recvbuf, delm);
		 	id = token;
		 	token = strtok(NULL, delm);
		 	WIRE = token;
		    id=NULL;
		    strcpy(Wire,WIRE);
		     vTaskDelay(100 / portTICK_PERIOD_MS);
       }
	  else{
		  strcpy(Wire,"-1");
	  }

	}
	END:
	 printf(" Socket task 1 out\n");
	vTaskDelay(3000 / portTICK_PERIOD_MS);
 }
}
//******************************************************
void server_4Temp(void *p)
{
	struct sockaddr_in clientAddress;
	struct sockaddr_in serverAddress;
	socklen_t clientAddressLength;
	char recvbuf[10];
	int sock;
	int rc;
	int clientSock;
	char *id, *TEMP, *token;
	while(1)
	{
	xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
						false, true, portMAX_DELAY);
	//ESP_LOGI(TAG, "Connected to AP");

	// Create a socket that we will listen upon.
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		ESP_LOGE(TAG, "socket: ERROR");
	}
	//unsigned long mode=1;
	//ioctlsocket(sock,FIONBIO,&mode);
	// Bind our server socket to a port.
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(TEMP_PORT);
	rc  = bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
	if (rc < 0) {
		//ESP_LOGE(TAG, "bind: %d %s", rc, strerror(errno));
	//	goto END;
	}

	// Flag the socket as listening for new connections.
	rc = listen(sock, 30);
	if (rc < 0) {
		ESP_LOGE(TAG, "listen:EROR");
	}

	while (1)
	{

		// Listen for a new client connection.
		clientAddressLength = sizeof(clientAddress);
		clientSock = accept(sock, (struct sockaddr *)&clientAddress, &clientAddressLength);
		if (clientSock < 0)
		{
			ESP_LOGE(TAG, "accept: ERROR");
			if (errno!=23)
			{

			}

		}
		//bzero(recv_buf, sizeof(recv_buf));
		bzero(recvbuf,sizeof(recvbuf));
	    read(clientSock, recvbuf, sizeof(recvbuf)-1);
		printf("\nReceived Data  From Temperature=%s  size: %d\n",recvbuf,strlen(recvbuf));
		 //send the receive buffer to the SD card task
		throughput+=strlen(recvbuf);

		close(clientSock);


	  if(recvbuf[0]=='T')
		{
		    token = strtok(recvbuf, delm);
		 	id = token;
		 	token = strtok(NULL, delm);
		 	TEMP = token;
		 	//printf("Temperature %d",TEMP);
		     id=NULL;
		     strcpy(Temp,TEMP);
		     vTaskDelay(100 / portTICK_PERIOD_MS);
       }
	  else{
		strcpy(Temp,"-1");
		strcpy(alarmT,"-1");
	  }

	}

 }

}
//*****************************************************
void server_4Android(void *p)
{
	struct sockaddr_in clientAddress;
	struct sockaddr_in serverAddress;
	socklen_t clientAddressLength;

	char recvbuf[10];

	int sock;
	int rc;
	int clientSock;
	char *id, *UpperLimitPtr, *token,*LowerLimitPtr;
	while(1)
	{

	xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
						false, true, portMAX_DELAY);

	//ESP_LOGI(TAG, "Connected to AP");

	// Create a socket that we will listen upon.
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
	 ESP_LOGE(TAG, "Error Creating socket ");
	}
	// Bind our server socket to a port.
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(ANDROID_PORT);

	rc  = bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
	if (rc < 0) {
		 ESP_LOGE(TAG, "Error bind");

	}

	// Flag the socket as listening for new connections.
	rc = listen(sock, 30);
	if (rc < 0) {
		 ESP_LOGE(TAG, "Error listen ");

	}

	while (1)
	{

		// Listen for a new client connection.
		clientAddressLength = sizeof(clientAddress);
		clientSock = accept(sock, (struct sockaddr *)&clientAddress, &clientAddressLength);
		if (clientSock < 0)
		{
			 ESP_LOGE(TAG, "Error accept");

		}
		send(clientSock,sendBuf,sizeof(sendBuf),0);
		//bzero(recv_buf, sizeof(recv_buf));
		bzero(recvbuf,sizeof(recvbuf));
	    read(clientSock, recvbuf, sizeof(recvbuf)-1);
		printf("\nReceived Data  From Android=%s  size: %d\n",recvbuf,strlen(recvbuf));
		 //send the receive buffer to the SD card task
		throughput+=strlen(recvbuf);

		close(clientSock);


	  if(recvbuf[0]=='A')
		{
		    token = strtok(recvbuf, delm);
		 	id = token;
		 	token = strtok(NULL, delm);
		 	LowerLimitPtr= token;
		 	token = strtok(NULL, delm);
		 	UpperLimitPtr=token;

		     id=NULL;
		     strcpy(LowerT,LowerLimitPtr);
		     strcpy(UpperT,UpperLimitPtr);

		     vTaskDelay(100 / portTICK_PERIOD_MS);
       }
	  else{
		  strcpy(LowerT,14);
		  strcpy(UpperT,500);


	  }

	}

 }
}
//****************************************************
