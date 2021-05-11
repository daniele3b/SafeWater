const express = require("express");
const router = express.Router();
const config = require("config");
require("dotenv").config();
const path = require("path");
var AWS = require("aws-sdk");
var awsIot = require('aws-iot-device-sdk');




let awsConfig = {
    "region": "us-east-1",
    "endpoint": "http://dynamodb.us-east-1.amazonaws.com",
    "accessKeyId": process.env.accessKeyId,
     "secretAccessKey": process.env.secretAccessKey,
     "sessionToken":process.env.sessionToken
};

var device = awsIot.device({
    keyPath: "./certs/private.key",
    certPath: "./certs/cert.crt",
    caPath: "./certs/rootCA.pem",
    clientId: process.env.id,
    host:process.env.endpoint
 });

router.get("/control",async(req,res)=>{
    res.sendFile(path.join(__dirname, "../views", "control.html"));
  })
  


router.get("/getDevices",async(req,res)=>{

  AWS.config.update(awsConfig);

  var docClient = new AWS.DynamoDB.DocumentClient();
  
  var params = {
      TableName: "safewater_temperature",
  };
  
  docClient.scan(params,await getDevices);
  var count = 0;
  
  

function getDevices(err, data) {

  return new Promise(function(resolve, reject) {     if (err) {
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
    } 
    res.status(200).send(devices_id)
  })
    
}

});


router.get("/manualOn/:id",async(req,res)=>{

    device_id=req.params.id

    
    device.publish('device/'+device_id+'/control', "MANUAL\0");


    res.status(200).send("DONE")
  
  
  });

  router.get("/manualOff/:id",async(req,res)=>{

    device_id=req.params.id

    
    device.publish('device/'+device_id+'/control', "MANUALRESET\0");


    res.status(200).send("DONE")
  
  
  });





router.get("/relayOn/:id",async(req,res)=>{

    device_id=req.params.id

    
    device.publish('device/'+device_id+'/control', "RELAYON\0");


    res.status(200).send("DONE")
  
  
  });


  
router.get("/relayOff/:id",async(req,res)=>{

    device_id=req.params.id

    
    device.publish('device/'+device_id+'/control', "RELAYOFF\0");

    res.status(200).send("DONE")
  
  
  });


  
router.get("/servoOn/:id",async(req,res)=>{

    device_id=req.params.id
  
    device.publish('device/'+device_id+'/control', "SERVOON\0");


    res.status(200).send("DONE")
  
  
  });


  router.get("/servoOff/:id",async(req,res)=>{

    device_id=req.params.id

    device.publish('device/'+device_id+'/control', "SERVOOFF\0");


    res.status(200).send("DONE")
  
  
  });





  module.exports = router;