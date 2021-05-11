# SafeWater

## Introduction

The managament of water resources is an important task in particular where there is a **shortage of water**, often rain water is not used or it's used in an inefficient way. SafeWater is an IoT solution that allows to the water resources's manager to monitor the **fill level** of the water containers and their **temperature** to choose the best moment to use them.
SafeWater is thought to maximize the capacity of the water silos and to choose the best way of use for a particular water resource, to reach these purposes it uses 2 kind of sensors:

- A DHT22: a sensor used to measure the temperature of the water;
- A Water level sensor: a sensor used to measure the fill level of the silos;

The system uses a relay in order to activate a water pump when the reserve is fill in order to use the water collected, and it use a servo motor in order to open and to close the water resource making it possible to collect water without waste.

## Purpose of the second Assignment

The purpose of the second assignment is to replace the single device (Nucleo-F401RE) by a Wireless Sensor Network comprised of multiple nodes, in particular they are ARM Cortex M3 STM32F103REY nodes. The devices are available of [Iot-Lab](https://www.iot-lab.info/) a platform used for testing and evaluation of IOT solutions.  The protocol used to create the mesh is the 802.15.4 in combination with 6loWPAN protocols in order to create a wireless sensor network and connect it to the internet in order to send data to AWS Cloud by using a MQTT/MQTT-SN broker.

In carrying out the second homework, particular attention was paid to performance evaluation in terms of power consumption, latency and throughput.

## Benefits & Limitations

Creating a Wireless Sensor Network, we can have a network of sensors in order to monitor the environment and obtain data that would normally be impossible or difficult to find by humans. The dense deployment of these sensors allows to the system to create a model of the phenomenon that you want to measure so , as a consequence it is possible to obtain qualitatively and quantitatively better data. A network of sensors allows to be very fault tollerant, in fact, using devices/sensors with overlapping observation areas also if a node crashs we can get data using another device that is next to the crashed sensor. Obviously, if many nodes crash the network couldn't work. In terms of energy efficency, a single node must be optimzed to ensure network longevity and amortize the cost of creating the network.

As the number of nodes increases, we could meet some limitations as:

- Increase of the end to end delay;
- Decrease of the throughput;
- Increase of the maintenance cost;



## IoT Architecture

![Scheme](https://github.com/daniele3b/SafeWater/blob/main/images/schema2.png)


As how it's possible to see from the above image the first component of the architecture are ARM Cortex M3 STM32F103REY nodes. These are devices that are deployed in the test environment of Iot-Lab.

In the **first point** devices'll send their packets to a border router, that is an ARM Cortex M3 STM32F103REY node that represents the router in which all nodes are connected, it represents the point of contact between the test environment and Internet. 


In the **second point** the data are sent from the border router to a Broker.

In the **third point** data sent by the border router are manageb by the broker, it is an **iotlab-a8** node in which are present a local RSBM broker and also an MQTT broker in particular **Mosquitto**.  At this level each each device has 3 topic:

- device/<ID_DEVICE>/temperature : to send data about temperature;
- device/<ID_DEVICE>/alarm : to send message of alarm when the container is filled;
- device/<ID_DEVICE>/control : to receive commands to open/close the container from remote;

<ID_DEVICE> is the identifider of each device.

At this point, there is also the communication between RSBM and MOSQUITTO. This intermediate step is needed because RSBM doesn't support a secure connection and so it's impossible to communicate directly wth AWS IoT Core in particulare with its MQTT broker. So,a bridge has been configured in order to bridge the data from/to AWS IoT Core to RSBM allowing data to reach devices and the cloud. Furthermore there is the secure communication between MOSQUITTO and AWS MQTT Broker, in order to let the data exchange we need to configure a device on AWS IoT Core in order to get certificates that will allows to MOSQUITTO to communicate in a secure way with the AWS MQTT Broker.

In the **fourth point** the system uses the Rules Engine provided by the IoT Core in order to save data about sensors in particular data that come from topics 1 and 2 (device/<ID_DEVICE>/temperature and device/<ID_DEVICE>/alarm) into DynamoDB, a NoSql database, the system uses 2 tables:

- Temperature Table: contains data about temperature sensor;
- Alarm Table: contains data about water level sensor;

Each record of these tables contains also the timestamp of the record and the identifier of the device that sent data.

In the **fifth point** the backend of the web-dashboard communicates through API with the DynamoDB endpoint to get temperature and fill level data. The backend written using NodeJS provides routes (API) in order to allow to the frontend (written in Javascript, HTML, Bootstrap and VueJS) to visualize data.

In the **sixth point** frontend can communicate through the backend (NodeJS) using the package supplied by AWS (aws-sdk-iot) with the AWS MQTT Broker in order to control the servo motor from the dashboard.


## HOW TO SETUP AND RUN ##

The system needs of third parties software in order to work, so please install:
- RIOT OS
- NodeJS
- Create an AWS account 
- Create an Fit Iot-Lab account
- Openocd

The system is composed by 2 main parts:

- Iot-Lab
- Server Web-dashboard

### Setup experiment on Iot-Lab

In order to start the experiment you need to have configured your SSH access key, for more info check this [link](https://www.iot-lab.info/docs/getting-started/ssh-access/). Run the following commands in order to setup correctly the environment to run the experiment.

- Connect to a frontend (Saclay): **ssh <login>@saclay.iot-lab.info** (where login is your username)
- Login into your account: **iotlab-auth -u <login>** (where login is your username)
- Launch an experiment you can use the GUI from the browser or the CLI. You must choose at least 2 nodes m3 and 1 node a8. All nodes must be in the same frontend (Saclay)
- Create a new directory: **mkdir -p ~/riot** 
- Change directory: **cd ~/riot**
- Clone Riot: **git clone https://github.com/RIOT-OS/RIOT.git** 
- Change directory : **cd RIOT**
- Set the source: **source /opt/riot.source**
- Compile the code of the border router using as DEFAULT_CHANNEL 14:  **make ETHOS_BAUDRATE=500000 DEFAULT_CHANNEL=<channel> BOARD=iotlab-m3 -C examples/gnrc_border_router clean all**
- Flash the .elf file in a m3 node: **iotlab-node --flash examples/gnrc_border_router/bin/iotlab-m3/gnrc_border_router.elf -l saclay,m3,1**
- Configure the network: **sudo ethos_uhcpd.py m3-1 tap0 2001:660:3207:04c1::1/64**
- Compile the main.c provided with its makefile: **make ETHOS_BAUDRATE=500000 DEFAULT_CHANNEL=14**
- Flash the code into another m3 device you can also use the GUI
- Check if the network is working launch **ifconfig** after you've connected to the node **nc m3-1 20000**
- Connect to ssh to the a8 node: **ssh root@node-a8-100**
- Create and configure as in the previous assignment the config.conf changing the address instead of 127.0.0.1 you must use ipv6 given by the network (you can check it using: ip -6 -o show addr eth0 if you've doubt check RSBM section) 
- Copy certs from local to a8 node using SSH and create the bridge.conf (**Check paths** if you don't how to do this checl the Mosquitto Section) 
- Run the RSBM and Mosquitto on a8 node using: **broker_mqtt config.conf** for RSBM and **mosquitto -c bridge.conf** for mosquitto 

For more detailed info you can check the following link: [LINK1](https://www.iot-lab.info/learn/tutorials/riot/riot-public-ipv6-m3/) and [LINK2](https://www.iot-lab.info/legacy/tutorials/riot-mqtt-sn-a8-m3/index.html)
### Setup AWS IoT Core

In order to allow to the system to work it's necessary that you have an account on AWS, you need to create a "thing" and get its certificate, you can find a tutorial [here](https://docs.aws.amazon.com/iot/latest/developerguide/iot-moisture-create-thing.html).

You need also to setup 2 rules in order to make possible to Iot Core to store data coming from the Mosquitto Broker into the DynamoDB, the topic that you need to manage are:

- device/+/temperature
- device/+/alarm

You can follow this [tutorial](https://docs.aws.amazon.com/iot/latest/developerguide/iot-ddb-rule.html) to create these rules, in the operation field of the rule set **device_data**, in particular the query must be:

- SELECT * FROM 'device/+/temperature'
- SELECT * FROM 'device/+/alarm'

The DynamoDB tables' name should be safewater_temperature and safewater_alarm

### Setup Mosquitto

In order to bridge data coming from RSBM to AWS MQTT Broker and viceversa, you need to configure Mosquitto using certificates obtained before and you need to setup 3 topics with the communication mode as **both**

- 'device/+/temperature'
- 'device/+/alarm'
- 'device/+/control'

You can find a tutorial [here](https://aws.amazon.com/it/blogs/iot/how-to-bridge-mosquitto-mqtt-broker-to-aws-iot/)

### Setup RSBM

In the configuration file (config.conf) of RSBM, you need to configure the bridge for the three topics above reported, the communication mode is **both** 

### Setup backend

To run the backend you need to install all the dependencies, you can do this using **npm install** : it'll install all the dependencies contained in the package.json. You need to create a .env file containing the information about the aws account:

- accessKeyId
- secretAccessKey
- sessionToken
- id
- endpoint

Furthermore, you need to create a folder called **certs** and load certificates of the "thing" inside it.

### Run and Test 

1. After you completed the setup, you need to connect to the SSH frontend using  **ssh <login>@saclay.iot-lab.info** 
2. Connect to a m3 node in which you've flashed the firmware during setup phase (different from the border router) using: **nc m3-1 20000**
3. Use the command: con < ipv6 addr > < port > < device_id >
4. Start the web dashboard and the backend using **node index.js**, the web-dashboard will be available at **localhost:8080**
5. Using the terminal you can see the console print messages and you'll receive also messages coming from the broker (RSBM)
6. Using the web-dashboard you can interact with the device and retrieve data

## Techincal Infomation  about the code ##

Information about the board's code can be found [here](https://github.com/daniele3b/SafeWater/blob/main/Assignment2/tech.md)

## Web Dashboard

Information about the web dashoboard interface (GUI) can be found [here](https://github.com/daniele3b/SafeWater/blob/main/Assignment2/server.md)



