# tsetwlog

**tsetwlog** is a native Node.js module for logging ETW events. 

## Installing
Install from npm via: `npm install tsetwlog`

ETW is a Windows technology, so this module only works on Windows (Win7 or later). The module is built using
the N-API interface and should work for Node.js 10.2 or later. (See https://nodejs.org/docs/v10.2.0/api/n-api.html)

## Building
From a Visual Studio 2017 Developer Command prompt, run `build.bat` to build the 32/64 bit binaries.
These will be placed in the `ia32` and `x64` directories respectively. (Note: This module is not built using the
`node-gyp` method - which is common for native modules - due to being Windows only and having minimal dependencies).

## Misc
See the `notes.txt` file for details on the implementation, TODOs, etc.
