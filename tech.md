# SafeWater 

## main.c

### Constants to activate/disactive functionality

```
#define DHT_DEV						1
#define FLOAT_DEV					1
#define SERVO_DEV					1
#define RELAY_DEV					1
#define MQTT_DEV					1
#define DEV_MODE					1
```

These constants represent flag, they are used to abilitate or to disable features of the system in particular:

- DHT_DEV: active/disactive functionality about DHT22 sensor;
- FLOAT_DEV: active/disactive functionality about fill level sensor;
- SERVO_DEV: active/disactive functionality about servo motor;
- RELAY_DEV: active/disactive functionality about relay;
- MQTT_DEV: active/disactive functionality about MQTT-SN;
- DEV_MODE: active/disactive debug print


### Utility constants

```
#define DELAY 						60
#define SERVO_MIN 550
#define SERVO_MAX 2500
#define DEGREE_MAX 180
#define DEGREE_TO_US(x) ((x*(SERVO_MAX-SERVO_MIN)/DEGREE_MAX)+SERVO_MIN)
#define CLOSE_DEGREE 90
#define OPEN_DEGREE 20

#ifndef EMCUTE_ID
#define EMCUTE_ID           ("waterstation")
#endif
#define EMCUTE_PRIO         (THREAD_PRIORITY_MAIN - 1)

#define NUMOFSUBS           (3)
#define TOPIC_MAXLEN        (64U)
```
These are utility constants, in particular **DELAY** represents the time between one reading and the next of the temperature sensor, **NUMOFSUBS** represents the number of topics in which the device is registered and **EMCUTE_ID** represents the ID of the device in this case is "waterstation"

## Global Variables
```
struct tm timer;

//define servo
servo_t servo;

//define relay
gpio_t relay;

//define floater
gpio_t floater=GPIO_PIN(PORT_B,5);

int state=0;

static char stack[THREAD_STACKSIZE_DEFAULT];
static emcute_sub_t subscriptions[NUMOFSUBS];
static char topics[NUMOFSUBS][TOPIC_MAXLEN];
char* MQTT_TOPICS [] ={"device/1/temperature","device/1/alarm","device/1/control"};
char* ALARM= "WATER ALARM!";
char* NOALARM= "WATER ALARM RESET!";
```

These are global variables that threads share among them, in particular the **timer** is used to interact with the temperature sensor with regular intervals using the **rtc** module. Variables as **servo**, **floater** and **relay** represent pins in which these devices are connected.

An important variable is the MQTT_TOPICS, it is an array of topics to distinguish the devices, each topic contain an incremental number, in this case 1.

## MQTT COMMUNICATION

```
static void *emcute_thread(void *arg)
{
    (void)arg;
    emcute_run(CONFIG_EMCUTE_DEFAULT_PORT, EMCUTE_ID);
    return NULL;    /* should never be reached */
}
```
This function allows to create a thread in order to manage the MQTT communication with the local broker.

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

int setup_mqtt(void)
{
    /* initialize our subscription buffers */
    memset(subscriptions, 0, (NUMOFSUBS * sizeof(emcute_sub_t)));

    /* start the emcute thread */
    thread_create(stack, sizeof(stack), EMCUTE_PRIO, 0,
                  emcute_thread, NULL, "emcute");

    // connect to MQTT-SN broker
    printf("Connecting to MQTT-SN broker %s port %d.\n",
           SERVER_ADDR, SERVER_PORT);

    sock_udp_ep_t gw = { .family = AF_INET6, .port = SERVER_PORT };
    char *topic = MQTT_TOPIC;
    char *message = "connected";
    size_t len = strlen(message);

    /* parse address */
    if (ipv6_addr_from_str((ipv6_addr_t *)&gw.addr.ipv6, SERVER_ADDR) == NULL) {
        printf("error parsing IPv6 address\n");
        return 1;
    }

    if (emcute_con(&gw, true, topic, message, len, 0) != EMCUTE_OK) {
        printf("error: unable to connect to [%s]:%i\n", SERVER_ADDR,
               (int)gw.port);
        return 1;
    }

    printf("Successfully connected to gateway at [%s]:%i\n",
           SERVER_ADDR, (int)gw.port);

    // setup subscription to topics 
    unsigned flags = EMCUTE_QOS_0;
    
    for (int i=0;i< NUMOFSUBS;i++)
    {
		subscriptions[i].cb=on_pub;
		strcpy(topics[i],MQTT_TOPICS[i]);
		subscriptions[i].topic.name=MQTT_TOPICS[i];
	}
    
	//subscribe to topics
	for (int i=0;i<NUMOFSUBS;i++)
	{
		if (emcute_sub(&subscriptions[i], flags) != EMCUTE_OK) 
		{
			printf("error: unable to subscribe to %i topic\n", i);
			return 1;
		}
	}
    
    if(DEV_MODE)
    {
		printf("Now subscribed!\n");
	}
    return 1;
}
```

The function **setup_mqtt** allows to the emcute thread to connect to the local broker, to subscribe to each topic indicated in the MQTT_TOPICS array and to set as callback the function **on_pub**: this function allows to control the device by remote using the web dashboard monitoring the message oon the topic "device/1/control".

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

This function allows to publish data on a certain topic, the QoS used is 0 and it takes as parameters: a topic structure that contains the ID and the name of the topic, the data to be sent and the size of the data.

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




