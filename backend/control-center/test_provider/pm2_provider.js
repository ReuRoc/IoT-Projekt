var pm2 = require('pm2');

pm2.connect(function (err) {
    if (err) throw err;

    setTimeout(function worker() {
        console.log("Restarting the provider...");
        pm2.restart('provider_test_color', function () { });
        setTimeout(worker, 1000 * 9999);
    }, 1000);
});