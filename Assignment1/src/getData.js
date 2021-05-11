var AWS = require("aws-sdk");
const config = require("config");
require("dotenv").config()


let awsConfig = {
    "region": "us-east-1",
    "endpoint": "http://dynamodb.us-east-1.amazonaws.com",
    "accessKeyId": process.env.accessKeyId,
     "secretAccessKey": process.env.secretAccessKey,
     "sessionToken":process.env.sessionToken
};


AWS.config.update(awsConfig);

var docClient = new AWS.DynamoDB.DocumentClient();

var params = {
    TableName: "safewater_temperature",
};

docClient.scan(params, this.getTemperatureData);
var count = 0;


exports.getTemperatureData=function getTemperatureData(err, data) {
    if (err) {
        console.error("Unable to scan the table. Error JSON:", JSON.stringify(err, null, 2));
    } else {        
        console.log("Scan succeeded.");
        data.Items.forEach(function(itemdata) {
           console.log("Item :", ++count,JSON.stringify(itemdata));
        });

        // continue scanning if we have more items
        if (typeof data.LastEvaluatedKey != "undefined") {
            console.log("Scanning for more...");
            params.ExclusiveStartKey = data.LastEvaluatedKey;
            docClient.scan(params, onScan);
        }
    }

    return data;
} 




