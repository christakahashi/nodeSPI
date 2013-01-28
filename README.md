node-SPI
========

This is a library made as a wrapper around ioctl to controll SPI ports in 
embedded linux.  It has been tested with the raspberry Pi and an mcp3008 chip
only, though I expect it to work elsewhere with other ICs as well.

If you find any bugs or have any patches please let me know!


Usage
-----

rom nodejs script include with
    var spi = require('./path/to/module/rSPI')
or place module in an appropriate node_modules directory.

Functions:

    spi.configSPI(mode,max_clock,device)
        takes SPI mode, clock rate, and device path (eg /dev/spidev0.1 )
        If system defaults are okay then no need to call configSPI
    spi.readwriteSPI(tx_data,device)
        takes a device path, and an array of numbers interpreted as bytes
        writes bytes and returs the same number of read bytes from spidev

See code comments for more details.


Build Instructions
------------------

### Dependencies
* npm -- This comes with newer versions of node 
* node-gyp -- this tool automates building node modules (requires a system compiler)

    npm install -g node-gyp

### to build you must first configure
    node-gyp configure
### and finally build
    node-gyp build

the addon is now in /build/Release/*.node
to use the addon move rSPI.node to either the global node_modules directory or
place it in a your a folder called node_modules with in your project directory



