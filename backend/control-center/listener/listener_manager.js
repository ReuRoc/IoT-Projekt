const mqtt = require('async-mqtt');
let pm2 = require('pm2');
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
  await initPM2();
}

let initPM2 = async () => {
  pm2.connect(function (err) {
    if (err) {
      console.error(err)
      process.exit(2)
    }
  })
}

client.on('connect', startListening);

client.on('message', function (topic, message) {
  if (topic.includes(listeningTopic.substr(0, listeningTopic.length - 1))) {
    if (topic.includes(config.mqtt_topic_temp)) {
      let idEnd = topic.length - (config.mqtt_topic_main.length + config.mqtt_topic_temp.length) - 2;
      console.log(`found device ${topic.substr(config.mqtt_topic_main.length + 1, idEnd)}`);
      callForDeviceProcess();
    } else if (topic.includes(config.mqtt_topic_dire)) {
      let idEnd = topic.length - (config.mqtt_topic_main.length + config.mqtt_topic_dire.length) - 2;
      console.log(`found device ${topic.substr(config.mqtt_topic_main.length + 1, idEnd)}`);
      callForDeviceProcess();
    }
  }
})

function callForDeviceProcess(deviceID) {
  if(findDeviceID(deviceID)){
    registerDevice(deviceID);
  } else {
    // congratz, there is nothing for you todo
  }
}

function registerDevice(deviceID) {
  pm2.start({
    script: './listener/listener.js',
    name: `listening2_${deviceID}`,
    args: `${deviceID}`
  }, (err, apps) => {
    pm2.disconnect()
    if (err) { throw err }
  })
}
function findDeviceID (deviceID){
  let allIDs = [];
  pm2.list((e, v) => {
    v.forEach(el => {
      allIDs.push(el.pm_id);
    });
  });
  return allIDs.find((e)=>{return e == deviceID}) ? true : false;
};