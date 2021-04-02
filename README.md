# SafeWater

## Introduction

The managament of water resources is an important task which helps to improve the environment quality. SafeWater is an IoT solution that allows to the water resources's manager to monitor the fill level of the water resources and their temperature to choose the best moment to use them.
SafeWater is thought to maximize the capacity of the water silos and to choose the best way of use for a particular water resource, to reach these purposes it uses 2 kind of sensors:

- A DHT22: a sensor used to measure the temperature of the water;
- A Water level: a sensor used to measure the fill level of the silos;

The system uses a relay in order to activate a water pump when the reserve is fill in order to use the water collected, and it use a servo motor in order to open and to close the water resource making it possible to collect water without waste.

## Sensors and Actuators

DHT22 is the sensor used to measure the temperature in order to choose the best use of a certain water resource. It is used for periodic sensing, the time can be specified through a constant DELAY in the code for the embedded system. The accuracy of this sensor is +-0.5 Celsius instead it's sensitivity is 0.1 Celsius and it's Sensing period has an average of 2s

![DHT22](https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcTl4hbNtcYRyk61Te1Oc6CpURJg1IHGcW0OeUMNck-hJ4uDKsvC4lXkHNmg6mThXXZPjs8&usqp=CAU)

Water level sensor ZP4510 is used to measure the fill level and it's an event driven sensor: when  the water reaches a level sufficient to close the circuit through the float, the system will detect the level of the water. Its body dimension is 36mm so it's possible to measure a growth of the water level equals to its body dimension, the fill level is constantly monitored.

![Water level sensor](https://images-na.ssl-images-amazon.com/images/I/61%2BHMmSEonL._AC_SX450_.jpg)

Actuators of the system are: 

- A servo motor: used to similute the closure of the water reserve;
- A relay: used to activate a water pump;

When the water resource is full the water sensor level will activate the closure of the silos through the servo motor and it'll activate the water pump closing the circuit using the relay in order to use the water collected. In particular it's possible to close and to open manually through the web dashboard the servo motor in order to choose if the operator want that more water is collected inside that water resource or not.

The system is designed in order to guarantee the correct operation also with multiple SafeWater devices.
