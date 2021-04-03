# SafeWater

## Introduction

The managament of water resources is an important task in particular where there is a **shortage of water**, often rain water is not used or it's used in an inefficiently. SafeWater is an IoT solution that allows to the water resources's manager to monitor the **fill level** of the water containers and their **temperature** to choose the best moment to use them.
SafeWater is thought to maximize the capacity of the water silos and to choose the best way of use for a particular water resource, to reach these purposes it uses 2 kind of sensors:

- A DHT22: a sensor used to measure the temperature of the water;
- A Water level sensor: a sensor used to measure the fill level of the silos;

The system uses a relay in order to activate a water pump when the reserve is fill in order to use the water collected, and it use a servo motor in order to open and to close the water resource making it possible to collect water without waste.

## Sensors and Actuators

**DHT22** is the sensor used to measure the temperature in order to choose the best use of a certain water resource. It is used for periodic sensing, the time can be specified through a constant DELAY in the code for the embedded system. The accuracy of this sensor is +-0.5 Celsius instead it's sensitivity is 0.1 Celsius and it's sensing period has an average of 2s. You can find more info [here](https://learn.adafruit.com/dht?view=all)

![DHT22](https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcTl4hbNtcYRyk61Te1Oc6CpURJg1IHGcW0OeUMNck-hJ4uDKsvC4lXkHNmg6mThXXZPjs8&usqp=CAU)

Water level sensor **ZP4510** is used to measure the fill level and it's an event driven sensor: when  the water reaches a level sufficient to close the circuit through the float, the system will detect the level of the water. Its body dimension is 36mm so it's possible to measure a growth of the water level equals to its body dimension, the fill level is constantly monitored in order to avoid waste of water. You can find more info [here](https://www.amazon.it/gp/product/B01MTYPK9I/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1)

![Water level sensor](https://images-na.ssl-images-amazon.com/images/I/61%2BHMmSEonL._AC_SX450_.jpg)

Actuators of the system are: 

- A **servo motor**: used to simulate the closure of the water container;
- A **relay**: used to activate a water pump;

When the water resource is full the water sensor level will activate the closure of the container through the servo motor and it'll activate the water pump closing the circuit using the relay in order to use the water collected. In particular, if the water container isn't filled, it's possible to close and to open manually through the web dashboard the servo motor in order to choose if the operator want that more water is collected inside that water resource or not. Furthermore, the system collects the temperature periodically in order to monitor the water and th environment status.

The system is designed in order to guarantee the correct operation also with multiple SafeWater devices.

## IoT Architecture

![Scheme](https://github.com/daniele3b/SafeWater/blob/main/images/schema.png)

As how it's possible to see from the above image the first component of the architecture is the Nucleo f401re. These are the devices that will be deployed in the water containers in order to get data from the sensor and to control the actuators to manage water resources.

In the **first point** devices'll communicate with a local MQTT-SN in particular RSBM, in order to allow this exchange of data we need to provide a simulation of an ethernet/wifi module because the Nucleo f401re doesn't provide the connectivity module as other board do. To simulate this internet module during the development Ethos has been used, it allows to multiplex the USB channel in more parts and use these part as we want: in this case Ethos is used to create an udp channel and to provide an ipv6 that it's needed to communicate with the network. At this level each each device has 3 topic:

- device/<ID_DEVICE>/temperature : to send data about temperature;
- device/<ID_DEVICE>/alarm : to send message of alarm when the container is filled;
- device/<ID_DEVICE>/control : to receive commands to open/close the container from remote;

<ID_DEVICE> is the identifider of each device.

In the **second point** there is the communication between RSBM and MOSQUITTO, this intermediate step is needed because RSBM doesn't support the secure connection and so it's impossible to communicate directly wth AWS IoT Core in particulare with its MQTT broker. So,a bridge has been configured in order to bridge the data from/to AWS IoT Core to RSBM allowing data to reach devices and the cloud.

In the **third point** there is the secure communication between MOSQUITTO and AWS MQTT Broker, in order to let the data exchange we need to configure a device on AWS IoT Core in order to get certificates that will allows to MOSQUITTO to communicate in a secure way with the AWS MQTT Broker.

In the **fourth point** the system uses the Rules Engine provided by the IoT Core in order to save data about sensors in particular data that come from topics 1 and 2 (device/<ID_DEVICE>/temperature and device/<ID_DEVICE>/alarm) into DynamoDB, a NoSql database, the system uses 2 tables:

- Temperature Table: contains data about temperature sensor;
- Alarm Table: contains data about water level sensor;

Each record of these tables contains also the timestamp of the record and the identifier of the device that sent data.

In the **fifth point** the backend of the web-dashboard communicates through API with the DynamoDB endpoint to get temperature and fill level data. The backend written using NodeJS provides routes (API) in order to allow to the frontend (written in Javascript, HTML, Bootstrap and VueJS) to visualize data.

In the **sixth point** frontend can communicate through the backend (NodeJS) using the package supplied by AWS (aws-sdk-iot) with the AWS MQTT Broker in order to control the servo motor from the dashboard.


## HOW TO SETUP AND RUN ##

In order to use the system we need to setup a circuit as in the image below:

![Fritzing image](https://github.com/daniele3b/SafeWater/blob/main/images/circuit.png)

**NOTE**
The temperature is a DHT22/AM2302 instead in the image above is an RHT3, DHT22/AM2302 has three pins: VCC, GND,OUT.
The relay module used it's different from that reported in the scheme, it has 2 relay and has 4 pins: VCC,GND,IN1,IN2. IN1 and IN2 are used to close/open the relay. Furthermore, there are also pins for external source power.
The fill level sensor doesn't exist in Fritzing, but it's wired as in the image above.

The system needs of third parties software in order to work, so please install:
- RIOT OS
- RSBM
- Mosquitto
- NodeJS
- Create an AWS account 
- Openocd

The system is composed by 2 main parts:

- Nucleo f401re
- Server Web-dashboard

### Setup AWS IoT Core

In order to allow to the system to work it's necessary that you have an account on AWS, you need to create a "thing" and get its certificate, you can find a tutorial [here](https://docs.aws.amazon.com/iot/latest/developerguide/iot-moisture-create-thing.html).

You need also to setup 2 rules in order to make possible to Iot Core to store data coming from the Mosquitto Broker into the DynamoDB, the topic that you need to manage are:

- device/+/temperature
- device/+/alarm

You can follow this [tutorial](https://docs.aws.amazon.com/iot/latest/developerguide/iot-ddb-rule.html) to create these rules, in the operation field of the rule set **device_data**, in particular the query must be:

- SELECT * FROM 'device/+/temperature'
- SELECT * FROM 'device/+/alarm'

The DynamoDB tables should name safewater_temperature and safewater_alarm

### Setup Mosquitto

In order to bridge data coming from RSBM to AWS MQTT Broker and viceversa, you need to configure Mosquitto using certificates obtained before and you need to setup 3 topics with the communication mode as **both**

- 'device/+/temperature'
- 'device/+/alarm'
- 'device/+/control'

You can find a tutorial [here](https://aws.amazon.com/it/blogs/iot/how-to-bridge-mosquitto-mqtt-broker-to-aws-iot/)

### Setup RSBM

In the configuration file (config.conf) of RSBM, you need to configure the bridge for the three topics above reported, the communication mode is **both** 

### Setup backend

To run the backend you need to install all the dependencies, you can do this using **npm install** it'll install all the dependencies contained in the package.json. You need to create a .env file containing the information about the aws account:

- accessKeyId
- secretAccessKey
- sessionToken
- id
- endpoint

Furthermore, you need to create a folder called **certs** and load certificates of the "thing" inside it.

### Run and Test 

1. Start the mosquitto broker using **sudo service mosquitto start** or **sudo service mosquitto restart** 
2. Start RSBM using **./bridge_mqtt config.conf** in the RSBM folder
3. Flash and run the code into the nucleo f401re board using: **make all flash term**: it'll flash the application into the device and it will open the terminal.
4. Start the web dashboard and the backend using **node index.js**, the web-dashboard will be available at **localhost:8080**
5. Using the nucleo f401re terminal you can see the console print messages and you'll receive also messages coming from the broker (RSBM)
6. Using the web-dashboard you can interact with the device and retrieve data

## Techincal Infomation  about the code ##

Information about the board's code can be found [here](https://github.com/daniele3b/SafeWater/blob/main/tech.md)




