// Attempt to load & return the native module if supported on this platform.
// There will be no exports if the module cannot be loaded

// Only runs on Windows, on 32/64-bit Intel architectures
if (process && (process.arch === "ia32" || process.arch === "x64") && process.platform === "win32") {
    // Check Node.js requirements. The N-API usage in the module requires Node.js 10.2 or later
    if (process.versions && process.versions.node) {
        var nodeVersionParts = process.versions.node.split('.');
        if (parseInt(nodeVersionParts[0]) === 10 && parseInt(nodeVersionParts[1]) >= 2 || parseInt(nodeVersionParts[0]) > 10) {
            module.exports = require(`./${process.arch}/tsetwlog.node`);
        }
    }
};