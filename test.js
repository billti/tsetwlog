var addon = require("./" + process.arch + "/tsetwlog.node");

const readline = require('readline');

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

rl.question('Press enter to exit\n\n', (answer) => {
    rl.close();
});

console.log(addon.logEvent("this is a test"));
