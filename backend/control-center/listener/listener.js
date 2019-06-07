let pm2 = require('pm2');
const mqtt = require('async-mqtt');
const config = require('../config.json');
const client = mqtt.connect(
  config.mqtt_broker,
  config.mqtt_options
);

function pm2init() {
  pm2.connect(function (err) {
    if (err) {
      console.error(err);
      process.exit(2);
    }
  });
}
pm2init();

let listeningID = process.argv[3];
let operatorID = process.argv[4];
let listeningTopic = `${config.mqtt_topic_main}/${listeningID}/#/`;

console.log('trying to connect to mqtt broker...');

const startListening = async () => {
  console.log('starting to listen...');
  await client.subscribe(listeningTopic, config.pubSubOptions);
}

client.on('connect', startListening);

client.on('message', function (topic, message) {
  if (topic.includes(listeningTopic.substr(0, listeningTopic.length - 2))) {
    let currentTopic = topic.substr(listeningTopic.length, topic.length - 1);
    switch (currentTopic) {
      case 'temperature':
        let packet = buildPacket(message, toString(), currentTopic);
        sendPacket(packet);
        console.log(message.toString());
        break;
        case 'direction':
        let packet = buildPacket(message, toString(), currentTopic);
        sendPacket(packet);
        console.log(message.toString());
        break;
      default:
        break;
    }
  }
})

function buildPacket(message, topic) {
  return {
    device: listeningID,
    type: topic,
    value: message,
    timestamp: Date.now()
  }
}

function sendPacket(packet) {
  pm2.sendDataToProcessId(
    operatorID,
    {
      type: 'process:msg',
      data: packet,
      topic: "listener_operator"
    },
    function (err, res) {
      console.log(err);
    }
  );
}