# Evalution of the performance

## Setup the monitoring profile

In order to monitor devices we need to create a **monitoring profile**, it can be created from the GUI. The profile used for my experiments is posted below. You can find detailed info about monitoring at this [link](https://iot-lab.github.io/docs/tools/consumption-monitoring/). (Note: if you want to plot data you need to connect to ssh using the parameter -X)

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

Also, in the other metrics the trend is stable for both nodes:

FIRST NODE : 

![M1](https://github.com/daniele3b/SafeWater/blob/main/images/m1-all.png)

SECOND NODE:

![M2](https://github.com/daniele3b/SafeWater/blob/main/images/m2-all.png)

To get a better view of the power consumption I realized a zoomed version of the node 1 but both nodes have the same trend:


![M1 ZOOM](https://github.com/daniele3b/SafeWater/blob/main/images/node_zoom.png)

From the image above it's easy to identify that the time in which the power consumption increase it's concurrent with the action of sending data.


Obviously, the power consumption of the "client" nodes isn't affected by the number of nodes.

## Results for border router

Results show that the power consumption it's not very constant during all the monitoring's period, it's possible to see some power consumption's peak when the device is receiving data from the "client" nodes. At the beginning of the plot it's possible to see a little noise but the trend across the time it'stable, this noise can be explained by time occurred to setup and run the **con** command in the client nodes.


![BORDER](https://github.com/daniele3b/SafeWater/blob/main/images/border.png)

Obviously, the power consumption increase when the packets that need to be forwarded increase and so also the capacity of the network decrease because there are too packets to manage and so the throughput is limited. In the image below, it is possible to see a zoomed version of the power consumption:

![BORDER ZOOM](https://github.com/daniele3b/SafeWater/blob/main/images/border_zoom.png)

If we increase the number of "client" devices the power consumption increase but it is bounded by the capacity of the 6LowPAN protocol to manage packets so, the Watt consumed can increase but limited by an upper bound determined by the network protocol.

From the point of network performance the following behaviours has been identification:

- Increasing the number of "client" nodes the latency increase because the border route has some difficult to forward packets;
- Increasing the number of "client" nodes as said before increase the latency and the throughput decrease because the queue of the router grows fast;
- Also the end-to-end delay increase as the number of nodes increase.



