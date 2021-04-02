# SafeWater

## Introduction

The managament of water resources is an important task in particular where there is a **shortage of water**. SafeWater is an IoT solution that allows to the water resources's manager to monitor the **fill level** of the water resources and their **temperature** to choose the best moment to use them.
SafeWater is thought to maximize the capacity of the water silos and to choose the best way of use for a particular water resource, to reach these purposes it uses 2 kind of sensors:

- A DHT22: a sensor used to measure the temperature of the water;
- A Water level: a sensor used to measure the fill level of the silos;

The system uses a relay in order to activate a water pump when the reserve is fill in order to use the water collected, and it use a servo motor in order to open and to close the water resource making it possible to collect water without waste.

## Sensors and Actuators

**DHT22** is the sensor used to measure the temperature in order to choose the best use of a certain water resource. It is used for periodic sensing, the time can be specified through a constant DELAY in the code for the embedded system. The accuracy of this sensor is +-0.5 Celsius instead it's sensitivity is 0.1 Celsius and it's Sensing period has an average of 2s

![DHT22](https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcTl4hbNtcYRyk61Te1Oc6CpURJg1IHGcW0OeUMNck-hJ4uDKsvC4lXkHNmg6mThXXZPjs8&usqp=CAU)

Water level sensor **ZP4510** is used to measure the fill level and it's an event driven sensor: when  the water reaches a level sufficient to close the circuit through the float, the system will detect the level of the water. Its body dimension is 36mm so it's possible to measure a growth of the water level equals to its body dimension, the fill level is constantly monitored.

![Water level sensor](https://images-na.ssl-images-amazon.com/images/I/61%2BHMmSEonL._AC_SX450_.jpg)

Actuators of the system are: 

- A **servo motor**: used to similute the closure of the water reserve;
- A **relay**: used to activate a water pump;

When the water resource is full the water sensor level will activate the closure of the container through the servo motor and it'll activate the water pump closing the circuit using the relay in order to use the water collected. In particular, if the water container isn't filled, it's possible to close and to open manually through the web dashboard the servo motor in order to choose if the operator want that more water is collected inside that water resource or not.

The system is designed in order to guarantee the correct operation also with multiple SafeWater devices.

## IoT Architecture

![Scheme](https://github.com/daniele3b/SafeWater/blob/main/images/schema.png)

As how it's possible to see from the above image the first component of the architecture is the Nucleo f401re. These are the devices that will be deployed in the water container in order to get data from the sensor and to control the actuators to manage water resources.

In the **first point** devices'll communicate with a local MQTT-SN in particular RSBM, in order to allow this exchange of data we need to provide a simulation of an ethernet/wifi module because the Nucleo f401re doesn't provide the connectivity module as other board do. To simulate this internet module during the development Ethos has been used, it allows to multiplex the USB channel in more parts and use these part as we want: in this case Ethos is used to create an udp channel and to provide an ipv6 that it's needed to communicate with the network. At this level each each device has 3 topic:

- device/<ID_DEVICE>/temperature : to send data about temperature;
- device/<ID_DEVICE>/alarm : to send message of alarm when the container is filled;
- device/<ID_DEVICE>/control : to receive commands to open/close the container from remote;


In the **second point** there is the communication between RSBM and MOSQUITTO, this intermediate step is needed because RSBM doesn't support the secure connection and so it's impossible to communicate directly wth AWS IoT Core in particulare with its MQTT broker. So,a bridge has been configured in order to  
bridge the data from/to AWS IoT Core to RSBM allowing data to reach devices and the cloud.

In the **third point** there is the secure communication between MOSQUITTO and AWS MQTT Broker, in order to let the data exchange we need to configure a device on AWS IoT Core in order to get certificates that will allows to MOSQUITTO to communicate in a secure way with the AWS MQTT Broker.

In the **fourth point** the system uses the Rules Engine provided by the IoT Core in order to save data about sensors in particular data that come from topics 1 and 2 (device/<ID_DEVICE>/temperature and device/<ID_DEVICE>/alarm) into DynamoDB, the system uses 2 tables:

- Temperature Table: contains data about temperature sensor
- Alarm Table: contains data about water level sensor

Each record of these tables contains also the timestamp of the record and the identifier of the device that sent data.

In the **fifth point** the backend of the web-dashboard communicates through API with the DynamoDB endpoint to get temperature and fill level data. The backend written using NodeJS provides routes (API) in order to allow to the frontend (written in Javascript and HTML using VueJS) to visualize data.

In the **sixth point** frontend can communicate through the backend (NodeJS) with the AWS MQTT Broker in order to control the servo motor from the dashboard.
