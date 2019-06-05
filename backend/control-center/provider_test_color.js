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
      config.test_colors[getRandomInt(1, config.test_colors.length)]
    );
    await client.end();
  } catch (e) {
    console.log(e.stack);
    process.exit();
  }
}

client.on('connected', publishTestColors);


// function testfunc(){
//     color_test.postColor();
//     setTimeout(() => {
//         testfunc()
//     }, 1000);;
// }