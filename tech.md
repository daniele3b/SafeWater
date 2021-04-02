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
