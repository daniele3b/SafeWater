# Evalution of the performance

## Setup the monitoring profile

In order to monitor devices we need to create a **monitoring profile**, it can be created from the GUI. The profile used for my experiments is posted below. You can find detailed info about monitoring at this [link](https://iot-lab.github.io/docs/tools/consumption-monitoring/).

![Monitoring profile](https://github.com/daniele3b/SafeWater/blob/main/images/monitoring_profile.png)

As it is possible to see from the image above, I monitored the following metrics for each device:

- Power consumption (W);
- Current (A);
- Voltage (V);

In term of radio performance, I monitored the following metrics for each devices and in particular for the border router node:

- RSSI

I measured the performance of 2 channels: 11 and 14.

## Definition of the test environment

During the measurement session I setup the Iot-Lab environment as follows:

- 1 m3 border router
- 2 m3 device/sensor deployed 
- 1 a8 MQTT/MQTT-SN broker

The experiment lasted about 6 hours during which the devices were constantly measured.

## Results for device/sensor 

Results show that the power consumption it's constant during all the monitoring's period, it's possible to see some power consumption's peak when the device is sending some data to the border router in order to send data to AWS Cloud.  In both devices the power consumption has the same trend. At the beginning of the plot it's possible to see a little noise but the trend across the time it's stable, this noise can be explained by time occurred to setup and run the **con** command.

FIRST NODE POWER CONSUMPTION: 

![M1 POWER](https://github.com/daniele3b/SafeWater/blob/main/images/m1.png)


SECOND NODE POWER CONSUMPTION.

![M2 POWER](https://github.com/daniele3b/SafeWater/blob/main/images/m2.png)

