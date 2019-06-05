const util = require('util');
const mqtt = require('async-mqtt');
// const color_test = require('./color_test.js');
const config = require('./config.json');
const client = mqtt.connect(
  config.mqtt_broker,
  config.mqtt_options
);
const publishTestColors = async () => {
  console.log('starting to publish colors');
  try {
    await client.publish(
      'test/color/',
      config.test_colors[getRandomInt(0, config.test_colors.length)],
      config.publishOptions
    );
  } catch (e) {
    console.log(e.stack);
    process.exit();
  }
}

let sendingMessages = true;
const startSendingMessages = () => {
  for (let i = 0, p = Promise.resolve(); sendingMessages == true; i++) {
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