let mqtt = require('mqtt')
let config = require('./config.json')
let client = mqtt.connect(
    config.mqtt_broker,
    config.mqtt_options
  );
 
client.on('connect', function () {
  client.publish('test/color', 'green');
  client.end();
})
 
client.on('message', function (topic, message) {
  // message is Buffer
  console.log(message.toString())
  client.end()
})