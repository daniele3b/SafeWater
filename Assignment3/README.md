
# SafeWater

**DUE TO THE FACT THAT THE LORA GATEWAY OF FIT/IOT-LAB  DOESN'T WORK ANY MORE, IT WAS NOT POSSIBLE TO TEST THE CODE, AND TO CARRY OUT THE PERFORMANCE EVALUATION**

## Post and demo

- [Post](https://medium.com/@danielebufalieritkd/safewater-de26e3ca427c)
- [Demo](https://youtu.be/ycpq_8E6XEE)

## Introduction

The managament of water resources is an important task in particular where there is a **shortage of water**, often rain water is not used or it's used in an inefficient way. SafeWater is an IoT solution that allows to the water resources's manager to monitor the **fill level** of the water containers and their **temperature** to choose the best moment to use them.
SafeWater is thought to maximize the capacity of the water silos and to choose the best way of use for a particular water resource, and to reach these purposes it uses 2 kind of sensors:

- A DHT22: a sensor used to measure the temperature of the water;
- A Water level sensor: a sensor used to measure the fill level of the silos;

The system uses a relay in order to activate a water pump when the reserve is fill in order to use the water collected, and it use a servo motor in order to open and to close the water resource making it possible to collect water without waste.

## Purpose of the third Assignment

The purpose of the third assignment is to replace a Wireless Sensor Network comprised of multiple node with a STM32L072CZ, a Nucleo board that has a LoraWan module incorporated. The devices are available of [Iot-Lab](https://www.iot-lab.info/), a platform used for testing and evaluation of IOT solutions. In this assignment we'll replace the MQTT protocol and the wireless 802.15.4 technology with LoRaWAN and [TheThingsNetwork](https://www.thethingsnetwork.org/) to reach the AWS cloud infrastructure.

LoRaWAN is a technology that allows to have a long range communication (around 45 km) with a low throughput but in this way it is possibile to have a lower power consumption with respect to other technologies as Bluetooth, Wifi, 4G and so on.

In carrying out the third homework, particular attention was paid to the development of IoT Edge Analytics.

## Benefits & Limitations

Deploying multiple sensors with overlapping observation areas in SafeWater using LoRaWAN as communication technology, this will result in a system that is able to obtain more data but with an important limitation due to the network capacity of LoRaWAN. Firstly, we need to consider the fact that if more devices try to send data by LoRaWAN with an high likelihood will result in network congestion, in order to do that we can use several techniques (used also in the Internet infrastructure):

- We can set a random delay before to send data;
- We can initilizate devices with some delay in order to avoid overlapping time in which 2 devices are sending data;

In SafeWater the messages sent are small in terms of bytes, so it's possible to use SF9 and a maximum payload size equals to 123-byte. In safeWater there are 2 sensors: a DHT22 and a water level sensor. In order to minimize the power consumption and to avoid the network congestion the sample time of the DHT22 is increased also if it will cause an accuracy loss, instead for the water sensor level there is not a problem due to the fact that is an event driven sensor so it will triggered only if it's necessary. 

## IoT Architecture

![Scheme](https://github.com/daniele3b/SafeWater/blob/main/images/schema3.png)


As it's possible to see from the above image the first components of the architecture are  ARM CortexM0+ B-L072Z-LRWAN1 nodes of the IOT-LAB. These are devices that are deployed in the test environment of Iot-Lab.

In the **first point** devices will send their packets to LoRaWAN Gateway of TTN,  These devices are ARM CortexM0+ B-L072Z-LRWAN1 nodes that represent SafeWater devices equipped with a LoRaWAN module, it represents the point of contact between the test environment and The Things Network. 

In the **second point** data are managed by The Things of Network using its API in order to route packets towards the AWS Cloud .

In the **third point** data sent by TTN are managed by TTN.  There are 3 topics:

- device/<ID_DEVICE>/temperature : to send data about temperature;
- device/<ID_DEVICE>/alarm : to send message of alarm when the container is filled;
- device/<ID_DEVICE>/control : to receive commands to open/close the container from remote;

<ID_DEVICE> is the identifider of each device.

At this point, there is also the communication between TTN and AWS IoT Core using MQTT. 

In the **fourth point** the system uses the Rules Engine provided by the IoT Core in order to save data about sensors in particular data that come from topics 1 and 2 (device/<ID_DEVICE>/temperature and device/<ID_DEVICE>/alarm) into DynamoDB, a NoSql database. The system uses 2 tables:

- Temperature Table: contains data about temperature sensor;
- Alarm Table: contains data about water level sensor;

Each record of these tables contains also the timestamp of the record and the identifier of the device that sent data.

In the **fifth point** the backend of the web-dashboard communicates through API with the DynamoDB endpoint to get temperature and fill level data. The backend written using NodeJS provides routes (API) in order to allow to the frontend (written in Javascript, HTML, Bootstrap and VueJS) to visualize data.

In the **sixth point** frontend can communicate through the backend (NodeJS) using the package supplied by AWS (aws-sdk-iot) with the AWS MQTT Broker in order to control the servo motor from the dashboard.


## HOW TO SETUP AND RUN ##

The system needs of third parties software in order to work, so please install/subscribe:
- RIOT OS
- NodeJS
- Create an AWS account 
- Create a Fit Iot-Lab account
- Create a The Things Network account
- Openocd

The system is composed by 4 main parts:

- Iot-Lab
- The Things Network
- AWS Cloud
- Server Web-dashboard

### Setup experiment on Iot-Lab

In order to start the experiment you need to have configured your SSH access key, for more info check this [link](https://www.iot-lab.info/docs/getting-started/ssh-access/). Run the following commands in order to setup correctly the environment to run the experiment.

#### Configure a TTN Application
- Add an application to your TTN account by following this [doc](https://www.thethingsnetwork.org/docs/applications/add/)
- Register a device in your [TTN application](https://www.thethingsnetwork.org/docs/devices/registration/), Keep the default OTAA procedure and set the following 3 informations: DEVICE EUI, APPLICATION EUI, Application Key
- Be sure to select LoRaWAN Version = MAC V1.0.2, Regional Parameters Version = PHY V1.0.2 REV B,Frequency plan = Europe 863-870MHz (SF9 for RX2) and Activation mode = OTAA

#### Book and configure the LoRA node

- Connect to a frontend (Saclay): **ssh < login >@saclay.iot-lab.info** (where login is your username)
- Login into your account: **iotlab-auth -u <login>** (where login is your username)
- Launch an experiment: you can use the GUI from the browser or the CLI. You must choose a node STM32L072CZ in order to allow the LoRaWAN communication
- Compile the code of the node available on this repository for the node
- Flash the .elf file in the node
- Set the LoRaWAN parameter if you don't set them in the code  after you've connected to the node 
- So the node will be connected 

For more detailed info you can check the following link: [LINK](https://www.iot-lab.info/legacy/tutorials/riot-ttn/index.html) .
  
#### Configure TTN-AWS communication
  You can follow this [tutorial](https://www.thethingsindustries.com/docs/integrations/cloud-integrations/aws-iot/default/)
 
### Setup AWS IoT Core
In order to allow to the system to work it's necessary that you have an account on AWS, you need to create a "thing" and get its certificate, you can find a tutorial [here](https://docs.aws.amazon.com/iot/latest/developerguide/iot-moisture-create-thing.html).

You need also to setup 2 rules in order to make possible to Iot Core to store data coming from the Mosquitto Broker into the DynamoDB, the topic that you need to manage are:

- device/+/temperature
- device/+/alarm

You can follow this [tutorial](https://docs.aws.amazon.com/iot/latest/developerguide/iot-ddb-rule.html) to create these rules, in the operation field of the rule set **device_data**, in particular the query must be:

- SELECT * FROM 'device/+/temperature'
- SELECT * FROM 'device/+/alarm'

The DynamoDB tables' name should be safewater_temperature and safewater_alarm

### Setup backend

To run the backend you need to install all the dependencies, you can do this using **npm install** : it'll install all the dependencies contained in the package.json. You need to create a .env file containing the information about the aws account:

- accessKeyId
- secretAccessKey
- sessionToken
- id
- endpoint

Furthermore, you need to create a folder called **certs** and load certificates of the "thing" inside it.

## Technical Infomation about the iotlab-m3 code ##

Information about the board's code can be found [here](https://github.com/daniele3b/SafeWater/blob/main/Assignment3/tech.md)

## Web Dashboard

Information about the web dashoboard interface (GUI) can be found [here](https://github.com/daniele3b/SafeWater/blob/main/Assignment3/server.md)

## Performance evaluation

It was not possible to make a performance evaluation due to the Iot Lab problems.


