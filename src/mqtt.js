var awsIot = require('aws-iot-device-sdk');
const config = require("config");
require("dotenv").config()

//
// Replace the values of '<YourUniqueClientIdentifier>' and '<YourCustomEndpoint>'
// with a unique client identifier and custom host endpoint provided in AWS IoT.
// NOTE: client identifiers must be unique within your AWS account; if a client attempts
// to connect with a client identifier which is already in use, the existing
// connection will be terminated.
//
var device = awsIot.device({
   keyPath: "./certs/private.key",
  certPath: "./certs/cert.crt",
    caPath: "./certs/rootCA.pem",
  clientId: process.env.id,
    host:process.env.endpoint
});

//
// Device is an instance returned by mqtt.Client(), see mqtt.js for full
// documentation.
//
device
  .on('connect', function() {
    console.log('connect');
    device.subscribe('device/1/control');
    device.publish('device/1/control', JSON.stringify({ test_data: 1}));
  });

device
  .on('message', function(topic, payload) {
    console.log('message', topic, payload.toString());
  });