const util = require('util');
const mqtt = require('async-mqtt');
// const color_test = require('./color_test.js');
const config = require('../config.json');
const client = mqtt.connect(
  config.mqtt_broker,
  config.mqtt_options
);
console.log('trying to connect to mqtt broker...');
const publishTestColors = async () => {
  console.log('publishing a color');
  try {
    await client.publish(
      'test/color/',
      config.test_colors[getRandomInt(0, config.test_colors.length - 1)],
      config.pubSubOptions
    );
  } catch (e) {
    console.log(e.stack);
    process.exit();
  }
}

let sendingMessages = true;
let maxMessages = 9999;
const startSendingMessages = () => {
  console.log('successfully connected to mqtt broker!');
  for (let i = 0, p = Promise.resolve(); i < maxMessages; i++) {
    p = p.then(_ => new Promise(resolve =>
      setTimeout(function () {
        publishTestColors();
        resolve();
      }, 1000)
    ));
  }
}

function stopSendingMessages() {
  sendingMessages = false;
}

client.on('connect', startSendingMessages);

function getRandomInt(min, max) {
  min = Math.ceil(min);
  max = Math.floor(max);
  return Math.floor(Math.random() * (max - min + 1)) + min;
}