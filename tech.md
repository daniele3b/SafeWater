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


## Setup of the sensors and actuators

### Servo motor setup

```	
	if(SERVO_DEV)
	{
		if(servo_init(&servo,PWM_DEV(0),1,SERVO_MIN,SERVO_MAX)<0)
		{
			printf("FAILED TO CONNECT TO SERVO!\n");
			return-1;
		}
	}	
```

This code allows to initialize the servo motor, using the channel one of the PWM_DEV(0) and using the driver contained in **servo.h** it's possible to set the range of degree that the servo it's able to use.

### Relay setup

```
	if(RELAY_DEV)
	{
		relay=GPIO_PIN(PORT_A,8);
		
		if(gpio_init(relay,GPIO_OUT)<0)
		{
			printf("FAILED TO CONNECT TO RELAY\n");
			return -1;
		}
	}
```
A relay can be seen as a digital device that takes as input value 0 (LOW VOLTAGE) or 1 (HIGH VOLTAGE), so in the piece of code above the relay is initialized as an output devicE.

### MQTT SETUP

```	
	if(MQTT_DEV)
	{
		// setup MQTT-SN connection
		printf("Setting up MQTT-SN.\n");
		setup_mqtt();
	}
```
This code launchs the setup phase of the MQTT thread that as written in the section about MQTT allows to connect and to manage data from/to the local broker (RSBM):

### Floater setup

```
if(FLOAT_DEV)
	{
		//check if floater is init
		if(gpio_init(floater,GPIO_IN)<0)
		{
			printf("FAILED TO CONNECT TO FLOATER!\n");
			return -1;
		}	
	}
```

The floater is a sensor that works as a switch so it can be considered as input, and it's possible to setup it using the classic GPIO function.
	
### DHT22 setup

```
	//define dht parameters
	dht_params_t my_params;
	my_params.pin= GPIO_PIN(PORT_A,10);
	my_params.type=DHT22;
	my_params.in_mode=DHT_PARAM_PULL;
    
	
	//initialize dht
	dht_t dev;
	if(DHT_DEV)
	{	
		if(dht_init(&dev,&my_params)==DHT_OK){
			printf("DHT SENSORS CONNECTED! \n");
			rtc_init();
			rtc_get_time(&timer);
			timer.tm_sec += DELAY;
			rtc_set_alarm(&timer,callback_rtc,&dev);
		}else
		{
			printf("FAILED TO CONNECT TO DHT SENSOR! \n");
			return -1;
		}
				
	}
```
 To initialize the DHT22 sensor it's possible to use the driver supplied by **dht.h**, it's necessary to set some parameters as: pin, type of dht sensor, and mode. Subsequently, the dht sensor is initialized using the **dht_init** function provided by the driver. Furthermore, the timer variable is used to set an interval after which a callback will be called:
 
 ```
static void callback_rtc(void* args)
{
	
	dht_t* dev= (dht_t*)args;
	reading_temperature_humidity(dev);
	rtc_get_time(&timer);
	timer.tm_sec += DELAY;
	rtc_set_alarm(&timer,callback_rtc,args);

}


void reading_temperature_humidity(dht_t *dht)
{
	int16_t temp,hum;
	
	if(dht_read(dht,&temp,&hum)!=DHT_OK)
	{
		printf("Error reading dht22! \n");
		return;	
			
	}
		
	char temp_s[10];
	size_t n = fmt_s16_dfp(temp_s,temp,-1);
	temp_s[n]= '\0';
			
	char hum_s[10];
	n= fmt_s16_dfp(hum_s,hum,-1);
	hum_s[n]='\0';
		
	if(DEV_MODE)
	{
		printf("HUM %s - TEMP %s \n",hum_s,temp_s);
	}
		
	if(MQTT_DEV)
	{
	
		char data[20];
		createJSONtemperature(temp_s,data);
		if(DEV_MODE)
		{
			printf("Temperature: %s\n",data);
		}
			
		emcute_topic_t tp;
		tp.name=subscriptions[0].topic.name;
		tp.id=subscriptions[0].topic.id;
		pub(&tp,data,strlen(data));
	}	

}

 ```
 The callback calls **reading_temperature_humidity** that allows to get the value about the temperature and to publish them on the temperature topic. After that the reading_temperature_humidity function is terminated the timer is resetted.

### Water alarm management

 ```
 void water_alarm_f(gpio_t floater,servo_t* servo,int* state)
{
	
	char data[30];
	int water_alarm=0;
	water_alarm=gpio_read(floater);
	if(water_alarm==0)
	{
		if(*state!=1)
		{
			
			printf("Water Alarm !\n");
				
			if(MQTT_DEV)
			{
				
				createJSONwater(ALARM,data);
				//publish data on mqttsn
				emcute_topic_t tp;
				tp.name=subscriptions[1].topic.name;
				tp.id=subscriptions[1].topic.id;
				pub(&tp,data,strlen(data));
				
			}
		}
		
		if(SERVO_DEV)
		{
			close_gate(servo);
		}
		
		if(RELAY_DEV)
		{
			active_pump();
		}
		*state=1;
	}
	else
	{
		if(*state==1)
		{
			if(DEV_MODE)
			{
				printf("Safe Water Level! \n");
			}
			open_gate(servo);
			
			createJSONwater(NOALARM,data);
			
			//publish data on mqttsn
			emcute_topic_t tp;
			tp.name=subscriptions[1].topic.name;
			tp.id=subscriptions[1].topic.id;
			pub(&tp,data,strlen(data));
			
		}
		*state=0;
		
		if(RELAY_DEV)
		{
			disactive_pump();
		}	
	}
}
 
  ```
 This function allows to the system to check the fill level of the water container it's always monitored by the program and when the container is filled the servo motor is closed and the relay is activate in order to close the circuit to allow to the water pump to extract the water. When the water reaches a low level the servo motor opens the water container and disables the water pump.
