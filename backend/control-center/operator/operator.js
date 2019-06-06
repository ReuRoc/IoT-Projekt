let pm2 = require('pm2');
const config = require('../config.json');

console.log('starting the operator...');

// list of all the devices we are managing
let deviceMasterList = new Array();

// initializes a pm2 instance to make use of certain
// functions to control other processes
let initPM2 = () => {
  pm2.connect(function (err) {
    if (err) {
      console.error(err)
      process.exit(2)
    }
  })
}

// triggers when a message is being received
// should only be spoken to by the listeners
process.on('message', function (packet) {
  if(!checkForDeviceInMasterListByID(packet.device)){
    let device = createDeviceData(packet.device);
    addDeviceToMasterList(device);
  }
  addPacketToDeviceQ(packet);
});

function logicTick() {
  updateAllDeviceStatuses();
}

setInterval(logicTick, 500);

// creates a device data object
function createDeviceData (deviceID){
  let deviceData = {};
  deviceData.device = {
    // the id of the device | int
    id: deviceID,
    // latest known temperature | float
    temperature_value: null,
    // latest known direction | int
    direction_value: null,
    // latest known state | int
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

// adds a packet, received by the respective listener
// to the corresponding devices queue
function addPacketToDeviceQ(packet) {
  deviceMasterList.forEach((entry)=>{
    if(entry.device.id == packet.device){
      entry.Q.push(packet);
      return;
    }
  });
}

// applies the latest item of the queue of a device to its
// respective properties
function updateAllDeviceStatuses() {
  
}