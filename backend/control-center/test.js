// this file can be used to test if
// the connection to the mqtt broker works
let mqtt = require('mqtt')
let config = require('./config.json')
let client = mqtt.connect(
    config.mqtt_broker,
    config.mqtt_options
  );
  let messageContent = 'secret message';
  let channel = 'test/message/';
console.log('Trying to connect to mqtt broker...');
client.on('connect', function () {
  console.log(`successfully connected!
  publishing a message to ${channel} with content:
  ${messageContent}`);
  client.publish(channel, messageContent);
  client.end();
})
 
client.on('message', function (topic, message) {
  // message is Buffer
  console.log(`recieved message, with the following content:
  ${message.toString()}`);
  client.end()
})