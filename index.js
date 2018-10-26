// Attempt to load & return the native module if supported on this platform.
// There will be no exports if the module cannot be loaded

// Only runs on Windows, on 32/64-bit Intel architectures
var loaded = false;
if (process && (process.arch === "ia32" || process.arch === "x64") && process.platform === "win32") {
    // Check Node.js requirements. The N-API usage in the module requires Node.js 10.2 or later
    if (process.versions && process.versions.node) {
        var nodeVersionParts = process.versions.node.split('.');
        if (parseInt(nodeVersionParts[0]) === 8 && parseInt(nodeVersionParts[1]) >= 9 || parseInt(nodeVersionParts[0]) > 9) {
            module.exports = require(`./${process.arch}/tsetwlog.node`);
            loaded = true;
        }
    }
};
// Return no module whatsoever if the native module didn't get loaded
if (!loaded) module.exports = undefined;
