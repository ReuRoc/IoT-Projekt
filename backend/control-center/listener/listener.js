const util = require('util');
const mqtt = require('async-mqtt');
const config = require('../config.json');
const client = mqtt.connect(
  config.mqtt_broker,
  config.mqtt_options
);

let listeningID = process.argv[3];
let listeningTopic = `${config.mqtt_topic_main}${listeningID}/#/`;

console.log('trying to connect to mqtt broker...');

const startListening = async () => {
  console.log('starting to listen...');
}

client.on('connect', startListening);

client.on('message', function (topic, message) {
  if (topic.includes(listeningTopic.substr(0, listeningTopic.length - 2))) {
    let currentTopic = topic.substr(listeningTopic.length, topic.length - 1);
    switch (currentTopic) {
      case 'temperature':

        break;
      case 'direction':

        break;

      default:
        break;
    }
  }
})