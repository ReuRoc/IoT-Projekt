let config = require('./config.json');
let mqtt = require('mqtt');
let client = mqtt.connect(
  config.mqtt_broker,
  config.mqtt_options
);

let init = () => {
  client.on('connect', () => {
    console.log("connected to mqtt broker");
  });
}

let postColor = () => {
  if (client.connected == true) {
    client.publish(
      'test/color/',
      config.test_colors[getRandomInt(0, config.test_colors.length)]
    );
    client.end();
  }
}

let subToTestColor = () => {
  client.subscribe('test/color/', () => {
    client.on('message', (topic, message) => {
      console.log(message.toString())
    });
  });
}


function getRandomInt(min, max) {
  min = Math.ceil(min);
  max = Math.floor(max);
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

module.exports.init = init;
module.exports.postColor = postColor;
module.exports.subToTestColor = subToTestColor;