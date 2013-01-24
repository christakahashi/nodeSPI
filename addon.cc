#define BUILDING_NODE_EXTENSION
#include <node.h>

#include<fcntl.h>
//#include<stdio.h> //for debug only
#include<string>
extern "C" {
#include "spi.h"
}

using namespace v8;

/*
function name: configSPI
inputs:
	args[0]: spi mode 
	args[1]: max clock speed
	args[2]: device name (eg '/dev/spidev0.1')
returns:
	undefined
*/
Handle<Value> configSPI(const Arguments& args) {
	HandleScope scope;

	//Check inputs
	if (args.Length() < 2) {
		ThrowException(
			Exception::TypeError(String::New("Wrong number of arguments")));
		return scope.Close(Undefined());
	}
	if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
		ThrowException(Exception::TypeError(String::New("Wrong arguments")));
		return scope.Close(Undefined());
	}

	//unpack args
	int spi_mode = args[0]->Int32Value();
	int clk_speed = args[1]->Int32Value();
	if (spi_mode<0 || spi_mode>4){
		ThrowException(Exception::Error(String::New("invalid spi mode")));
		return scope.Close(Undefined());
	}
	if (clk_speed<0 ){
		ThrowException(Exception::Error(String::New("invalid clock speed")));
		return scope.Close(Undefined());
	}

	v8::String::Utf8Value inputString(args[2]->ToString());
	std::string devicePath = std::string(*inputString);
	int fd = open(devicePath.c_str(), O_RDWR);
	if (fd <0) {
		ThrowException(Exception::Error(String::New("Can't open SPI device")));
		return scope.Close(Undefined());
	}

	if (!initSPI(fd, (uint8_t)spi_mode, (int32_t)clk_speed)) {
		ThrowException(
			Exception::Error(String::New("unable to configure SPI")));
		return scope.Close(Undefined());
	}

	return scope.Close(Undefined());
}



/*
function name: readwriteSPI
inputs:
	args[0]: js array of numbers to transmit over SPI
	args[1]: js string of the spi device to use (eg '/dev/spidev0.1' )
returns:
	js array of bytes received (as Numbers).
*/
Handle<Value> readwriteSPI(const Arguments& args) {
	HandleScope scope;

	//Check inputs
	if (args.Length() < 2) {
		ThrowException(
			Exception::TypeError(String::New("Wrong number of arguments")));
		return scope.Close(Undefined());
	}
	if (!args[0]->IsArray() ) {
		ThrowException(
			Exception::TypeError(String::New("Wrong arguments")));
		return scope.Close(Undefined());
	}

	//unwrap inputs
	Local<Array> inputArray = Local<Array>::Cast(args[0]);
	v8::String::Utf8Value inputString(args[1]->ToString());
	std::string devicePath = std::string(*inputString);

///////////////////ACTUAL WORK///////////////////  
	unsigned int message_length = inputArray->Length();
	uint8_t tx_buf[message_length];
	uint8_t rx_buf[message_length];
	int fd = open(devicePath.c_str(), O_RDWR);
	if (fd <0) {
		ThrowException(Exception::Error(String::New("Can't open SPI device")));
		return scope.Close(Undefined());
	}

	//fill TX buffer with data from user
	for (unsigned int i = 0; i<message_length ; i++){
	tx_buf[i] = (uint8_t)inputArray->Get(Integer::New(i))->NumberValue();
	}

	//do the spi RW
	if(spiRW(fd,message_length,tx_buf,rx_buf)) {
		ThrowException(Exception::Error(String::New("SPI read/write failed.")));
		return scope.Close(Undefined());
	}

//////////////////PACK UP OUTPUTS/////////////////////
	Local<Array> outputArray = Array::New(message_length);
	for (unsigned int i=0; i<message_length ; i++){
		outputArray->Set(i,Number::New(rx_buf[i]));
	}
			
	return scope.Close(outputArray);
}

void Init(Handle<Object> target) {
	target->Set(String::NewSymbol("readwriteSPI"),
			FunctionTemplate::New(readwriteSPI)->GetFunction());
	target->Set(String::NewSymbol("configSPI"),
			FunctionTemplate::New(configSPI)->GetFunction());
}

NODE_MODULE(rSPI, Init)

