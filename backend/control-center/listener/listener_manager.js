const mqtt = require('async-mqtt');
const config = require('../config.json');
const client = mqtt.connect(
  config.mqtt_broker,
  config.mqtt_options
);

let listeningTopic = `${config.mqtt_topic_main}/#`;

console.log('trying to connect to mqtt broker...');

const startListening = async () => {
  console.log('starting to listen...');
  await client.subscribe(listeningTopic, config.pubSubOptions);
}

client.on('connect', startListening);

client.on('message', function (topic, message) {
  if (topic.includes(listeningTopic.substr(0, listeningTopic.length - 1))) {
    if (topic.includes(config.mqtt_topic_temp)) {
      let idEnd = topic.length - (config.mqtt_topic_main.length + config.mqtt_topic_temp.length) - 2;
      console.log(`found device ${topic.substr(config.mqtt_topic_main.length + 1, idEnd)}`);
    } else if (topic.includes(config.mqtt_topic_dire)) {
      let idEnd = topic.length - (config.mqtt_topic_main.length + config.mqtt_topic_dire.length) - 2;
      console.log(`found device ${topic.substr(config.mqtt_topic_main.length + 1, idEnd)}`);
    }
  }
})