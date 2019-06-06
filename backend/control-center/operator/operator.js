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
    mapPacketOntoDevice(packet);
  };
});

function logicTick() {

}

setInterval(logicTick, 500);

function createDeviceData (){
  let deviceData = {};
  deviceData.device = {
    id: null,
    temperature_value: null,
    direction_value: null,
    state: null
  };
  deviceData.Q = [];
  return deviceData;
}

function addDeviceToMasterList(deviceID){
  deviceMasterList.push(deviceID);
}

function removeDeviceFromMasterList(deviceID) {
  deviceMasterList.delete(deviceID);
}

function checkForDeviceInMasterList(deviceID){
  return deviceMasterList.has(deviceID);
}
function mapPacketOntoDevice(packet) {
  
}