var addon = require("./" + process.arch + "/tsetwlog.node");

const readline = require('readline');

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

rl.question('Press enter to exit\n\n', (answer) => {
    rl.close();
});

addon.logStartCommand("init", "starting to run");
console.log(addon.logEvent("this is a test"));
addon.logStopCommand("init", "OK, we're done here");
