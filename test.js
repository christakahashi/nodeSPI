var rspi = require('./build/Release/rSPI');

var result = rspi.readwriteSPI([96,0,0],'/dev/spidev0.1');
var adcread = ((result[1]<<2) | (result[2]>>>6))*3.3/1024;
var resistance = 3.3*10000/adcread - 10000;

var a = 0.00113902;
var b = 0.000232276;
var c = 9.67879E-8;
var lr = Math.log(resistance);
var temp = -273.15+1/(a+b*lr+c*lr*lr*lr)

console.log('raw data:'+ result );
console.log('voltage:'+adcread.toFixed(3));
console.log('resistance:'+resistance.toFixed(2));
console.log('Temp:'+temp.toFixed(2));
