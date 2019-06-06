const mqtt = require('async-mqtt');
const config = require('../config.json');
const client = mqtt.connect(
  config.mqtt_broker,
  config.mqtt_options
);

let listeningTopic = `${config.mqtt_topic_main}#/`;

console.log('trying to connect to mqtt broker...');

const startListening = async () => {
  console.log('starting to listen...');
}

client.on('connect', startListening);

client.on('message', function (topic, message) {
  if (listeningTopic.substr(0, listeningTopic.length - 2).includes(topic)) {

  }
})