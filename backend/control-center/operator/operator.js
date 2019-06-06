let pm2 = require('pm2');
const config = require('../config.json');

console.log('starting the operator...');

// list of all the devices we are managing
let deviceMasterList = new Array();

let initPM2 = () => {
  pm2.connect(function (err) {
    if (err) {
      console.error(err)
      process.exit(2)
    }
  })
}

process.on('message', function (packet) {
  if (checkForDeviceInMasterList(packet.device)){
    addPacketToDevice(packet);
  } else {
    let device = createDeviceData(packet.device);
    addDeviceToMasterList(device);
  };
});

function logicTick() {

}

setInterval(logicTick, 500);

function createDeviceData (deviceID){
  let deviceData = {};
  deviceData.device = {
    id: deviceID,
    temperature_value: null,
    direction_value: null,
    state: null
  };
  deviceData.Q = [];
  return deviceData;
}

function addDeviceToMasterList(device){
  deviceMasterList.push(device);
}

function removeDeviceFromMasterListByID(deviceID) {
  deviceMasterList.forEach((entry, currentIndex)=>{
    if(entry.device.id == deviceID){
      deviceMasterList.splice(currentIndex, 1);
      return;
    }
  });
}

function removeDeviceFromMasterList(device){
  removeDeviceFromMasterListByID(device.device.id);
}

function checkForDeviceInMasterListByID(deviceID){
  return deviceMasterList.find((e)=>{return e == deviceID}) ? true : false;
}

function checkForDeviceInMasterList(device) {
  return checkForDeviceInMasterListByID(device.device.id);
}

function addPacketToDevice(packet) {
  deviceMasterList.forEach((entry)=>{
    if(entry.device.id == packet.device){
      entry.Q.push(packet);
      return;
    }
  });
}