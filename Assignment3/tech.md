# SafeWater

## makefile

These are the modules used in the system, in particular modules sx1267 and semteck_lormac are used in order to perform the communication between nodes of IoT-Lab and The Things Network by LoRaWAN. LORA_REGION indicates the region on which LoRaWAN is used: in this case Europe with 868 Mhz.

```

BOARD ?= b-l072z-lrwan1
RIOTBASE ?= ../../RIOT

USEMODULE += xtimer

USEMODULE += sx1276
USEMODULE += semtech_loramac_rx

USEPKG += semtech-loramac

LORA_REGION ?= EU868

USEMODULE += shell
USEMODULE += ps


ifneq (,$(TTN_DEV_ID))
  CFLAGS += -DTTN_DEV_ID=\"$(TTN_DEV_ID)\"
endif

DEVELHELP ?= 1

QUIET ?= 1

include $(RIOTBASE)/Makefile.include


```


## main.c


### Utility constants

```

#ifndef EMCUTE_ID
#define EMCUTE_ID           ("waterstation")
#endif
#define EMCUTE_PRIO         (THREAD_PRIORITY_MAIN - 1)

#define NUMOFSUBS           (3)
#define TOPIC_MAXLEN        (64U)
#define MAIN_QUEUE_SIZE     (8)

```
These are utility constants, in particular **NUMOFSUBS** represents the number of topics in which the device is registered, **EMCUTE_ID** represents the ID of the device: the default value is "waterstation". 

## Global Variables

```

char* ALARM= "WATER ALARM!";
char* NOALARM= "WATER ALARM RESET!";

char* MQTT_TOPICS [] ={"device/x/temperature","device/x/alarm","device/x/control"};
static char stack[THREAD_STACKSIZE_DEFAULT];
static msg_t queue[8];

static emcute_sub_t subscriptions[NUMOFSUBS];
static char topics[NUMOFSUBS][TOPIC_MAXLEN];
char EMCUTE_ID[20]="w";

char topic0[33];
char topic1[30];	
char topic2[30];

```

These are global variables that threads share among them, in particular we can see the three topics used for the communication by MQTT-SN: each of this topic is **dynamically built** in order to avoid that two devices use the same topic to send their data.

## MQTT COMMUNICATION

```
static void *emcute_thread(void *arg)
{
    (void)arg;
    emcute_run(CONFIG_EMCUTE_DEFAULT_PORT, EMCUTE_ID);
    return NULL;    /* should never be reached */
}
```
This function allows to create a thread in order to manage the MQTT-SN communication with the local broker (RSBM).

```
static void on_pub(const emcute_topic_t *topic, void *data, size_t len)
{
    char *in = (char *)data;

    printf("### got publication for topic '%s' [%i] ###\n",
           topic->name, (int)topic->id);
    for (size_t i = 0; i < len; i++) {
        printf("%c", in[i]);
    }

    
    if(topic->name==MQTT_TOPICS[2])
    {	
		
			if(strcmp(in,"SERVOON")==0)			
					open_gate(&servo);
				
			
				
			if(strcmp(in,"SERVOOFF")==0)
				close_gate(&servo);			
		
	}
	
}


void createTopics(char* id_device)
{
	char app[64]="";
	strcat(app,"device/");
	strcat(app,id_device);
	strcat(app,"/temperature");
	

	

	
	strcpy(topic0,app);
	printf("%s \n",topic0);
	strcpy(app,"");
	
	strcat(app,"device/");
	strcat(app,id_device);
	strcat(app,"/alarm");
	
	
	strcpy(topic1,app);
	strcpy(app,"");
	
	
	strcat(app,"device/");
	strcat(app,id_device);
	strcat(app,"/control");

	
	strcpy(topic2,app);
	
	
}


```

The function **createTopics** allows to the create topics based on the device_id that it's given by command line when the command **con** is used. The emcute thread is used to connect to the local broker, to subscribe to each topic indicated in the array and to set as callback the function **on_pub**: this function allows to control the device by remote using the web dashboard monitoring the message on the topic "device/**device_id**/control", in particular if the device receives "SERVOON", if the fill level is low, it'll open the water container using the servo motor, instead if it receives "SERVOOFF", it'll close the water container.

```
static int pub( emcute_topic_t *topic,void* data,size_t len)
{
	if(DEV_MODE)
	{
		printf("SENDING DATA!\n");
	}
	char *in = (char *)data;
    unsigned flags = EMCUTE_QOS_0;
    
    if (emcute_pub(topic, in, len, flags) != EMCUTE_OK) {
        printf("error: unable to publish data to topic '%s [%i]'\n",
                topic->name, (int)topic->id);
        return 1;
    }

    printf("Published %i bytes to topic '%s [%i]'\n",
            (int)len, topic->name, topic->id);
    return 0;
}
```

This function allows to publish data on a certain topic, the QoS used is 0, means **At most once**, and it takes as parameters: a topic structure that contains the **ID** and the **name** of the topic, the data to be sent and the size of the data.

## Utility functions for MQTT

```
void createJSONwater(char *state,char*s)
{
	
		//define text to send
		char* prefix="{\"alarm\": \"";
		char* suffix="\"}";
		int i=0;
		
		for (i=0;i<(int)strlen(prefix);i++){
			s[i]=prefix[i];
		}
		
		
		for( int j=0;j<(int)strlen(state);j++){
			s[i++]=state[j];
		}
		
		for(int j=0;j<(int)strlen(suffix);j++){
			s[i++]=suffix[j];
		}
	
	
		s[i]='\0';
		
}



void createJSONtemperature(char* temp_s,char* s)
{
		//define text to send
		char* prefix="{\"temp\": \"";
		char* suffix="\"}";
		int i=0;
		
		for (i=0;i<(int)strlen(prefix);i++){
			s[i]=prefix[i];
		}
		
		
		for( int j=0;j<(int)strlen(temp_s);j++){
			s[i++]=temp_s[j];
		}
		
		for(int j=0;j<(int)strlen(suffix);j++){
			s[i++]=suffix[j];
		}
		s[i]='\0';
		
}
```
The data to be sent are formatted in JSON, because the AWS MQTT broker works in this format. This kind of file allows also an easier management of the data when the rules are applied by the IoT Core.


## Starting the node by command line

### Servo motor setup

```	

static const shell_command_t shell_commands[] = {
    { "con", "connect to MQTT broker", cmd_con },
    { NULL, NULL, NULL }
};

int main(void)
{

    /* the main thread needs a msg queue to be able to run `ping6`*/
    msg_init_queue(queue, ARRAY_SIZE(queue));

    /* initialize our subscription buffers */
    memset(subscriptions, 0, (NUMOFSUBS * sizeof(emcute_sub_t)));

    /* start the emcute thread */
    thread_create(stack, sizeof(stack), EMCUTE_PRIO, 0,
                  emcute_thread, NULL, "emcute");

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
   
    
    return 0;
}

```

The main function is used to initialize a shell with the "classic commands" of a Linux terminal for network monitoring and configuration, but with a personalized command: **con**. It is used to connect to the broker and start to generate random data to send to AWS by Mosquitto broker. Below, it's possible to see the **con function** explained.



```	
	char id_device[1];
	strcpy(id_device,argv[3]);
	printf("%s\n ",id_device);
	
	strcat(EMCUTE_ID,id_device);

	createTopics(id_device);
	printf("Topics created !\n");
	
	printf("ID: %s \n",EMCUTE_ID);
		
    sock_udp_ep_t gw = { .family = AF_INET6, .port = CONFIG_EMCUTE_DEFAULT_PORT };
    char *topic = NULL;
    char *message = NULL;
    size_t len = 0;

    if (argc < 3) {
        printf("usage: %s <ipv6 addr> <port> <device_id> \n", argv[0]);
        return 1;
    }

    /* parse address */
    if (ipv6_addr_from_str((ipv6_addr_t *)&gw.addr.ipv6, argv[1]) == NULL) {
        printf("error parsing IPv6 address\n");
        return 1;
    }

    if (argc >= 3) {
        gw.port = atoi(argv[2]);
    }
    
```

In the first part of the function we get the device_id from the command line, it is the third parameter. Using the device_id we configure the EMCUTE_ID and topics using the **strcat** function available thanks to string.h, we check also if the number of parameters given from the command line is lower than 3. We parse the first argument that represents the **ipv6 of the broker**.


```	
if (emcute_con(&gw, true, topic, message, len, 0) != EMCUTE_OK) {
        printf("error: unable to connect to [%s]:%i\n", argv[1], (int)gw.port);
        return 1;
    }
    printf("Successfully connected to gateway at [%s]:%i\n",
           argv[1], (int)gw.port);

	unsigned flags = EMCUTE_QOS_0;
	
	int i=0;
	for(i=0;i<NUMOFSUBS;i++)
	{
		subscriptions[i].cb = on_pub;
		if(i==0)
			strcpy(topics[i],topic0);
		if(i==1)
			strcpy(topics[i],topic1);
		if(i==2)
			strcpy(topics[i],topic2);
			
		subscriptions[i].topic.name = topics[i];
		if (emcute_sub(&subscriptions[i], flags) != EMCUTE_OK) {
			printf("error: unable to subscribe to %s\n", argv[1]);
			return 1;
		}
	}

    printf("Now subscribed to all topics of %s\n", argv[1]);

```

We try to connect to the broker, and we subscribe the device to all the topics created previously by createTopics function.


```	

while(1)
	{
		char data[30];
		if(rand()%2==0)
			createJSONwater(ALARM,data);
		else
			createJSONwater(NOALARM,data);
		//publish data on mqttsn
		emcute_topic_t tp;
		tp.name=subscriptions[1].topic.name;
		tp.id=subscriptions[1].topic.id;
		pub(&tp,data,strlen(data));
		xtimer_sleep(60);
		
		
		int value=rand()%31+5;
		char temp_s[5];
		snprintf(temp_s,5,"%d",value);
		createJSONtemperature(temp_s,data);
		tp.name=subscriptions[0].topic.name;
		tp.id=subscriptions[0].topic.id;
		pub(&tp,data,strlen(data));
		xtimer_sleep(60);
	}


```

In the end we simulate using random generated data the reading of the sensors and we sent them to the broker using the **pub function**.
