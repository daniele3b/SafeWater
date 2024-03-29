# Web Dahsboard

In order to start the server and to reach the web dashboard, you must follow the steps indicated in the read me. The dashboard will be available at localhost:8080. 
The first interface is this: 

![Home](https://github.com/daniele3b/SafeWater/blob/main/images/home.png)

In this page you can find the last temperature measurement available for each device, for each measurement you can see also the time of registration. By clicking on "Aggregated Data" you'll reach the new interface on which you can see all aggregated data about the temperature

![Agg_temp](https://github.com/daniele3b/SafeWater/blob/main/images/agg_temp.png)

In this page you can find the last hour aggregated data, if there are no data the row is empty. The data are: minimum temperature, maximum temperature and average.
If in the homepage you interact with "Last hour" you'll reach another interface

![Last_hour](https://github.com/daniele3b/SafeWater/blob/main/images/last_temp.png)

Here, you can find all measurements of the last hour of all devices: you have the temperature value and also the time on which it was recorded.
By clicking on "Water Alarm Management" you'll reach another interface, that is the page about the fill level sensor.

![Water-alarm](https://github.com/daniele3b/SafeWater/blob/main/images/alarm.png)

Here, you can find for each device the last status of the fill level sensor, the values that can be assumed are:

- "WATER ALARM": it means that water level is high;
- "WATER ALARM RESET": it means that water level is low, so the alarm is resetted;

By clicking on "Aggregated Data" you'll reach the interface with the aggregated data of the last hour

![alarm_aggrefated](https://github.com/daniele3b/SafeWater/blob/main/images/agg_al.png)

In this page you can find how many times the alarm is setted and resetted in the last hour.
By interacting with "Last Hour" of the Water alarm homepage you'll reach this page.

![alarm_hour](https://github.com/daniele3b/SafeWater/blob/main/images/hour_al.png)

Here, you can find the alarm of the last hour for all the devices.
Lastly, by clicking on "Control actuators" you'll reach this page

![control actuators](https://github.com/daniele3b/SafeWater/blob/main/images/control%20act.png)

In this last interface you can interact with all the devices: you can open or close the gate (simulated by the servo motor). Obviously if the water level is not high and it doesn't reach the warming level otherwise the command will still sent but the device will not execute the action because considered unsafe.

In order to satisfy the request to visualize the aggregated data of the last hour obtained by all devices, it has been update the first view of Water Alarm Management and the Temperature Management, as in the images below:

![temperature_update](https://github.com/daniele3b/SafeWater/blob/main/images/temp_new.jpeg)

![water_update](https://github.com/daniele3b/SafeWater/blob/main/images/wat_man_new.jpeg)
