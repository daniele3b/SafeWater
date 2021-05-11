const express = require("express");
const router = express.Router();
const config = require("config");
require("dotenv").config();
const path = require("path");
var AWS = require("aws-sdk");



let awsConfig = {
    "region": "us-east-1",
    "endpoint": "http://dynamodb.us-east-1.amazonaws.com",
    "accessKeyId": process.env.accessKeyId,
     "secretAccessKey": process.env.secretAccessKey,
     "sessionToken":process.env.sessionToken
};




router.get("/", async (req, res) => {
  res.sendFile(path.join(__dirname, "../views", "homepage.html"));
});


router.get("/getAllDataTemperature",async(req,res)=>{

  AWS.config.update(awsConfig);

  var docClient = new AWS.DynamoDB.DocumentClient();
  
  var params = {
      TableName: "safewater_temperature",
  };
  
  docClient.scan(params,await getTemperatureData);
  var count = 0;
  
  

function getTemperatureData(err, data) {

  return new Promise(function(resolve, reject) { 
    list=[]
    if (err) {
      res.status(400).send("Bad Request")
    } else {        
      
      n=data.Count
      data=data.Items


      
    devices_id=[]
    for (i=0;i<n;i++){

      if (! devices_id.includes(data[i].device_id))
      {
        devices_id.push(data[i].device_id)
      }

    }


    for (j=0;j<devices_id.length;j++)
    {
      temp=0;
      timestamp_max=0;
     for (i=0;i<n;i++)
     {

      if(data[i].device_id==devices_id[j])
      {
        if(timestamp_max<data[i].sample_time)
        {
          timestamp_max=data[i].sample_time
          temp=data[i].device_data.temp
        }
      }
    }

      unix_timestamp=timestamp_max;
      var date = new Date(unix_timestamp);
      
      obj={
        "device_id": devices_id[j],
        "temp": temp,
        "time":date
      }

  
      list.push(obj);
    }
  


    
    res.status(200).send(list)
   
   }});
     
  };

});


router.get("/getAggregateDataTemperature",async(req,res)=>{

  AWS.config.update(awsConfig);

  var docClient = new AWS.DynamoDB.DocumentClient();
  
  var params = {
      TableName: "safewater_temperature",
  };
  
  docClient.scan(params,await getAggTemperatureData);
  var count = 0;
  
  

function getAggTemperatureData(err, data) {
  var ONE_HOUR = 60 * 60 * 1000;
  let current_date = new Date();

  return new Promise(function(resolve, reject) { 
    list=[]
    if (err) {
      res.status(400).send("Bad Request")
    } else {        
      
      n=data.Count
      data=data.Items

      devices_id=[]
    for (i=0;i<n;i++){

      if (! devices_id.includes(data[i].device_id))
      {
        devices_id.push(data[i].device_id)
      }

    }

    for (i=0; i<devices_id.length;i++)
    {
      min=999;
      max=0;
      avg=0;
      cont=0;
      sum=0;
      for(j=0;j<n;j++)
      {
        var date = new Date(data[j].sample_time);
        if( data[j].device_id==devices_id[i] && ((current_date) - date) < ONE_HOUR)
        {
          if(data[j].device_data.temp<min)
              min=data[j].device_data.temp
          if(data[j].device_data.temp>max)
              max=data[j].device_data.temp
          sum+=parseFloat((data[j].device_data.temp))
          cont++;
        }
      }
  
      avg=(sum/cont).toPrecision(4)

      if(min==999){
        min=NaN
        max=NaN
        avg=NaN
      }
      obj={
        "device_id": devices_id[i],
        "max": max,
        "min":min,
        "avg":avg
      }
      list.push(obj);

    }
    res.status(200).send(list)
   
   }});
     
  };

});

router.get("/homepage_alarm",async(req,res)=>{
  res.sendFile(path.join(__dirname, "../views", "homepage_alarm.html"));
})


router.get("/getAllDataTemperature",async(req,res)=>{

  AWS.config.update(awsConfig);

  var docClient = new AWS.DynamoDB.DocumentClient();
  
  var params = {
      TableName: "safewater_temperature",
  };
  
  docClient.scan(params,await getTemperatureData);
  var count = 0;
  
  

function getTemperatureData(err, data) {

  return new Promise(function(resolve, reject) { 
    list=[]
    if (err) {
      res.status(400).send("Bad Request")
    } else {        
      
      n=data.Count
      data=data.Items


      
    devices_id=[]
    for (i=0;i<n;i++){

      if (! devices_id.includes(data[i].device_id))
      {
        devices_id.push(data[i].device_id)
      }

    }


    for (j=0;j<devices_id.length;j++)
    {
      temp=0;
      timestamp_max=0;
     for (i=0;i<n;i++)
     {

      if(data[i].device_id==devices_id[j])
      {
        if(timestamp_max<data[i].sample_time)
        {
          timestamp_max=data[i].sample_time
          temp=data[i].device_data.temp
        }
      }
    }

      unix_timestamp=timestamp_max;
      var date = new Date(unix_timestamp);
      
      obj={
        "device_id": devices_id[j],
        "temp": temp,
        "time":date
      }

  
      list.push(obj);
    }
  


    
    res.status(200).send(list)
   
   }});
     
  };

});


router.get("/getAggregateDataTemperatureAllDevices",async(req,res)=>{

  AWS.config.update(awsConfig);

  var docClient = new AWS.DynamoDB.DocumentClient();
  
  var params = {
      TableName: "safewater_temperature",
  };
  
  docClient.scan(params,await getAggTemperatureData);
  var count = 0;
  
  

function getAggTemperatureData(err, data) {
  var ONE_HOUR = 60 * 60 * 1000;
  let current_date = new Date();

  return new Promise(function(resolve, reject) { 
    list=[]
    if (err) {
      res.status(400).send("Bad Request")
    } else {        
      
      n=data.Count
      data=data.Items

      min=999;
      max=0;
      avg=0;
      cont=0;
      sum=0;

    for (i=0; i<n;i++)
    {
 
    
        var date = new Date(data[i].sample_time);
        if( ((current_date) - date) < ONE_HOUR)
        {
          if(data[i].device_data.temp<min)
              min=data[i].device_data.temp
          if(data[i].device_data.temp>max)
              max=data[i].device_data.temp
          sum+=parseFloat((data[i].device_data.temp))
          cont++;
        }
      
  
      
    }

    avg=(sum/cont).toPrecision(4)

      if(min==999){
        min=NaN
        max=NaN
        avg=NaN
      }

      obj={
        "max": max,
        "min":min,
        "avg":avg
      }


    res.status(200).send(obj)
   
   }});
     
  };

});







router.get("/getLastHourDataTemperature",async(req,res)=>{

  AWS.config.update(awsConfig);

  var docClient = new AWS.DynamoDB.DocumentClient();
  
  var params = {
      TableName: "safewater_temperature",
  };
  
  docClient.scan(params,await getLastHourTemperatureData);
  var count = 0;
  
  

function getLastHourTemperatureData(err, data) {

  var ONE_HOUR = 60 * 60 * 1000;

  let current_date = new Date();
  let hours = current_date.getHours();

  return new Promise(function(resolve, reject) { 
    list=[]
    if (err) {
      res.status(400).send("Bad Request")
    } else {        
      
      n=data.Count
      data=data.Items
     
      for(j=0;j<n;j++)
      {
        var date = new Date(data[j].sample_time);
        if(((current_date) - date) < ONE_HOUR )
        {
          obj={
            "device_id": data[j].device_id,
            "temp":data[j].device_data.temp,
            "time":date
          }
          list.push(obj);
        }
      }

    }
    res.status(200).send(list)
   
   });
     
  };
});



/* 
  COLLECTING DATA ABOUT WATER ALARMS

*/




router.get("/getAllDataAlarm",async(req,res)=>{

  AWS.config.update(awsConfig);

  var docClient = new AWS.DynamoDB.DocumentClient();
  
  var params = {
      TableName: "safewater_alarm",
  };
  
  docClient.scan(params,await getAlarmData);
  var count = 0;
  
  

function getAlarmData(err, data) {

  return new Promise(function(resolve, reject) { 
    list=[]
    if (err) {
      res.status(400).send("Bad Request")
    } else {        
      
      n=data.Count
      data=data.Items


      
    devices_id=[]
    for (i=0;i<n;i++){

      if (! devices_id.includes(data[i].device_id))
      {
        devices_id.push(data[i].device_id)
      }

    }


    for (j=0;j<devices_id.length;j++)
    {
      status=""
      timestamp_max=0;
     for (i=0;i<n;i++)
     {

      if(data[i].device_id==devices_id[j])
      {
        if(timestamp_max<data[i].sample_time)
        {
          timestamp_max=data[i].sample_time
          status=data[i].device_data.alarm
        }
      }
    }

      unix_timestamp=timestamp_max;
      var date = new Date(unix_timestamp);
      
      obj={
        "device_id": devices_id[j],
        "status": status,
        "time":date
      }

  
      list.push(obj);
    }
    
    res.status(200).send(list)
   
   }});
     
  };

});


router.get("/getAggregateDataAlarm",async(req,res)=>{

  AWS.config.update(awsConfig);

  var docClient = new AWS.DynamoDB.DocumentClient();
  
  var params = {
      TableName: "safewater_alarm",
  };
  
  docClient.scan(params,await getAggAlarmData);
  var count = 0;
  
  

function getAggAlarmData(err, data) {

  var ONE_HOUR = 60 * 60 * 1000;

  let current_date = new Date();
  let hours = current_date.getHours();
  return new Promise(function(resolve, reject) { 
    list=[]
    if (err) {
      res.status(400).send("Bad Request")
    } else {        
      
      n=data.Count
      data=data.Items

      devices_id=[]
    for (i=0;i<n;i++){

      if (! devices_id.includes(data[i].device_id))
      {
        devices_id.push(data[i].device_id)
      }

    }
    

    for (i=0; i<devices_id.length;i++)
    {
      cont_act=0
      cont_disact=0

      for(j=0;j<n;j++)
      {
        var date = new Date(data[j].sample_time);
        if( data[j].device_id==devices_id[i] && ((current_date) - date) < ONE_HOUR)
        {
          if(data[j].device_data.alarm=="WATER ALARM!")
              cont_act++
          if(data[j].device_data.alarm=="WATER ALARM RESET!")
              cont_disact++
        }
      }
  
      
      obj={
        "device_id": devices_id[i],
        "cont_act": cont_act,
        "cont_disact":cont_disact
      }
      list.push(obj);

    }
    res.status(200).send(list)
   
   }});
     
  };

});



router.get("/getAggregateDataAlarmAllDevices",async(req,res)=>{

  AWS.config.update(awsConfig);

  var docClient = new AWS.DynamoDB.DocumentClient();
  
  var params = {
      TableName: "safewater_alarm",
  };
  
  docClient.scan(params,await getAggAlarmData);
  var count = 0;
  
  

function getAggAlarmData(err, data) {

  var ONE_HOUR = 60 * 60 * 1000;

  let current_date = new Date();
  let hours = current_date.getHours();
  return new Promise(function(resolve, reject) { 
    list=[]
    if (err) {
      res.status(400).send("Bad Request")
    } else {        
      
      n=data.Count
      data=data.Items
      cont_act=0
      cont_disact=0
  

    for (i=0; i<n;i++)
    {
        var date = new Date(data[i].sample_time);
        if( ((current_date) - date) < ONE_HOUR)
        {
          if(data[i].device_data.alarm=="WATER ALARM!")
              cont_act++
          if(data[i].device_data.alarm=="WATER ALARM RESET!")
              cont_disact++
        }

    }

    obj={
      "cont_act": cont_act,
      "cont_disact":cont_disact
    }
    res.status(200).send(obj)
   
   }});
     
  };

});



router.get("/getLastHourDataAlarm",async(req,res)=>{

  AWS.config.update(awsConfig);

  var docClient = new AWS.DynamoDB.DocumentClient();
  
  var params = {
      TableName: "safewater_alarm",
  };
  
  docClient.scan(params,await getLastHourAlarmData);
  var count = 0;
  
  

function getLastHourAlarmData(err, data) {

  var ONE_HOUR = 60 * 60 * 1000;

  let current_date = new Date();
  let hours = current_date.getHours();

  return new Promise(function(resolve, reject) { 
    list=[]
    if (err) {
      res.status(400).send("Bad Request")
    } else {        
      
      n=data.Count
      data=data.Items
     
      for(j=0;j<n;j++)
      {
        var date = new Date(data[j].sample_time);
        if(((current_date) - date) < ONE_HOUR )
        {
          obj={
            "device_id": data[j].device_id,
            "status":data[j].device_data.alarm,
            "time":date
          }
          list.push(obj);
        }
      }

    }
    res.status(200).send(list)
   
   });
     
  };
});

  module.exports = router;