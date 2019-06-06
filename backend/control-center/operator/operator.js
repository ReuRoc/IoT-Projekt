let pm2 = require('pm2');
const config = require('../config.json');

console.log('starting the operator...');

let initPM2 = () => {
  pm2.connect(function (err) {
    if (err) {
      console.error(err)
      process.exit(2)
    }
  })
}

process.on('message', function (packet) {
  
});

function logicTick() {

}

setInterval(logicTick, 500);