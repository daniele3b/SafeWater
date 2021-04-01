#include <stdio.h>
#include <string.h>
#include <time.h>
#include "xtimer.h"
#include "periph/adc.h"
#include "periph/gpio.h"
#include "analog_util.h"
#include "dht.h"
#include "dht_params.h"
#include "fmt.h"
#include "servo.h"
#include "periph/pwm.h"
#include "periph/rtc.h"
#include "net/emcute.h"
#include "periph/pm.h"
#include "msg.h"
#include "net/ipv6/addr.h"

#define DHT_DEV						1
#define FLOAT_DEV					1
#define SERVO_DEV					1
#define RELAY_DEV					1
#define MQTT_DEV					1
#define DEV_MODE					1



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



struct tm timer;

//define servo
servo_t servo;

//define relay
gpio_t relay;

//define floater
gpio_t floater=GPIO_PIN(PORT_B,5);



int precstate=0;
int state=0;

int mode=0;


static char stack[THREAD_STACKSIZE_DEFAULT];
static emcute_sub_t subscriptions[NUMOFSUBS];
static char topics[NUMOFSUBS][TOPIC_MAXLEN];
char* MQTT_TOPICS [] ={"device/1/temperature","device/1/alarm","device/1/control"};
char* ALARM= "WATER ALARM!";
char* NOALARM= "WATER ALARM RESET!";

static void *emcute_thread(void *arg)
{
    (void)arg;
    emcute_run(CONFIG_EMCUTE_DEFAULT_PORT, EMCUTE_ID);
    return NULL;    /* should never be reached */
}


void close_gate(servo_t* servo){
	servo_set(servo,DEGREE_TO_US(CLOSE_DEGREE));
}

void open_gate(servo_t *servo){
	servo_set(servo,DEGREE_TO_US(OPEN_DEGREE));
}


void active_pump(void)
{
	gpio_set(relay);	
}

void disactive_pump(void)
{
	gpio_clear(relay);
}

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




/**
 * Setup the EMCUTE, open a connection to the MQTT-S broker,
 * subscribe to the default topic and publish a message.
 */
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

static void callback_rtc(void* args)
{
	
	dht_t* dev= (dht_t*)args;
	reading_temperature_humidity(dev);
	rtc_get_time(&timer);
	timer.tm_sec += DELAY;
	rtc_set_alarm(&timer,callback_rtc,args);

}
 


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

int main(void)
{
	
		
	if(SERVO_DEV)
	{
		if(servo_init(&servo,PWM_DEV(0),1,SERVO_MIN,SERVO_MAX)<0)
		{
			printf("FAILED TO CONNECT TO SERVO!\n");
			return-1;
		}
	}
	
	
	if(RELAY_DEV)
	{
		relay=GPIO_PIN(PORT_A,8);
		
		if(gpio_init(relay,GPIO_OUT)<0)
		{
			printf("FAILED TO CONNECT TO RELAY\n");
			return -1;
		}
	}
	
	if(MQTT_DEV)
	{
		// setup MQTT-SN connection
		printf("Setting up MQTT-SN.\n");
		setup_mqtt();
	}
	

	//define dht parameters
	dht_params_t my_params;
	my_params.pin= GPIO_PIN(PORT_A,10);
	my_params.type=DHT22;
	my_params.in_mode=DHT_PARAM_PULL;
    

	
	
	
	if(FLOAT_DEV)
	{
		//check if floater is init
		if(gpio_init(floater,GPIO_IN)<0)
		{
			printf("FAILED TO CONNECT TO FLOATER!\n");
			return -1;
		}	
	}

	
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
	
    while (1) 
    {
	
		
		if(FLOAT_DEV && SERVO_DEV && mode!=1 )
		{
				water_alarm_f(floater,&servo,&state);		
			
		}
		
				
		
	}

}
