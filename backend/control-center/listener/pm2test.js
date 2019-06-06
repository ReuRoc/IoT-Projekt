let pm2 = require('pm2');
let deviceID = 4;
pm2.connect(function (err) {
  if (err) {
    console.error(err)
    process.exit(2)
  }
})
// pm2.start({
//   script: './test.js',
//   name: `listening2_${deviceID}`,
//   args: `${deviceID}`
// }, (err, apps) => {
//   pm2.disconnect()
//   if (err) { throw err }
// })

// output the first given process' id
pm2.list((e, v) => {
  console.log(v.pop().pm_id);
});