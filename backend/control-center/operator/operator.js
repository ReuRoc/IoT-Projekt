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
  if (!checkForDeviceInMasterListByID(packet.device)) {
    let device = createDeviceData(packet.device);
    addDeviceToMasterList(device);
  }
  addPacketToDeviceQ(packet);
});

function logicTick() {
  updateAllDeviceProperties();
  manageDeviceStates();
  cleanupDeviceQ();
}

// initiate logic loop
setInterval(logicTick, 500);

// creates a device data object
function createDeviceData(deviceID) {
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

function addDeviceToMasterList(device) {
  deviceMasterList.push(device);
}

function removeDeviceFromMasterListByID(deviceID) {
  deviceMasterList.forEach((entry, currentIndex) => {
    if (entry.device.id == deviceID) {
      deviceMasterList.splice(currentIndex, 1);
      return;
    }
  });
}

function removeDeviceFromMasterList(device) {
  removeDeviceFromMasterListByID(device.device.id);
}

function checkForDeviceInMasterListByID(deviceID) {
  return deviceMasterList.find((e) => { return e == deviceID }) ? true : false;
}

function checkForDeviceInMasterList(device) {
  return checkForDeviceInMasterListByID(device.device.id);
}

// adds a packet, received by the respective listener
// to the corresponding devices queue
function addPacketToDeviceQ(packet) {
  deviceMasterList.forEach((entry) => {
    if (entry.device.id == packet.device) {
      entry.Q.push(packet);
      return;
    }
  });
}

// applies the latest item of the queue of a device to its
// respective properties
function updateAllDeviceProperties() {
  deviceMasterList.forEach((device) => {
    filterDevicePacketQ(device);
  });
}

// searches for the two latest entries for a given device,
// regarding its temperature and its direction
function filterDevicePacketQ(device) {
  let mostRecentTemperatureEntry;
  let mostRecentDirectionEntry;
  let foundBothEntries = false;
  let qCopy = device.Q;
  while (!foundBothEntries && !qCopy.length == 0) {
    let currentEntryOfQ = qCopy.pop();
    if (currentEntryOfQ.type == config.listener_operator.type_temp) {
      mostRecentTemperatureEntry = currentEntryOfQ;
    } else if (currentEntryOfQ.type == config.listener_operator.type_dire) {
      mostRecentDirectionEntry = currentEntryOfQ;
    } else {
      // TODO: error handling
      // unknown packet type
    }
    if(mostRecentTemperatureEntry && mostRecentDirectionEntry){
      foundBothEntries = true;
    }
  }
  let updatedDevice = applyDevicePackets(device, mostRecentTemperatureEntry. mostRecentDirectionEntry);
  // TODO: use a direct reference instead of two separate objects
  removeDeviceFromMasterList(device);
  addDeviceToMasterList(updatedDevice);
}

// creates an updated device object, by applying the values of two
// given packets, for temperature and direction respectively
function applyDevicePackets(device, packet_temp, packet_dire) {
  device.device.temperature_value = packet_temp.value;
  device.device.direction_value = packet_dire.value;
  return device;
}

function manageDeviceStates() {
  deviceMasterList.forEach((entry)=>{
    updateDeviceState(entry);
    sendDeviceStateToProvider(entry);
  });
}

function updateDeviceState(device) {
  
}

// TODO: calculate the difference across all data points within the last
// minute (only those should still be in the list anyways)
// currently it just checks the oldest and newest entry and compares those
function calcTemperatureDifference(device){
  for (let i = 0; i < device.Q.length; i++) {
    if (device.Q[i].type == config.listener_operator.type_temp) {
      let temperature1 = device.Q[i];
      break;
    }
  }
  for (let i = device.length.Q; i > 0; i--) {
    if (device.Q[i].type == config.listener_operator.type_temp) {
      let temperature2 = device.Q[i];
      break;
    }
  }
  if (temperature1 >= temperature2) {
    return (temperature1 - temperature2 >= config.listener_operator.alert_threshold ) ? true : false;
  }
  if (temperature1 < temperature2) {
    return (temperature1 - temperature2 >= config.listener_operator.alert_threshold) ? true : false;
  }
}

function sendDeviceStateToProvider(device) {
  
}

// remove all packets that have exceeded their lifetime
function cleanupDeviceQ(device) {
  for (let i = 0; i < device.Q.length; i++) {
    if(device.Q[device.Q.length-1] - device.Q[0].timestamp  > config.listener_operator.lifetime_in_ms){
      device.Q.splice(0,1);
    } else {
      return;
    }
  }
}