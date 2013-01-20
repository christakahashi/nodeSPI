#define BUILDING_NODE_EXTENSION
#include <node.h>

#include<fcntl.h>
#include<string>
extern "C" {
#include "spi.h"
}

using namespace v8;


/*
function name: readSPI
inputs:
	args[0]: js array of numbers to transmit over SPI
	args[1]: js string of the spi device to use (eg '/dev/spidev0.1' )
returns:
	js array of bytes received (as Numbers).
*/
Handle<Value> readSPI(const Arguments& args) {
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
	if(spiRW(fd,tx_buf,rx_buf)) {
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
	target->Set(String::NewSymbol("readSPI"),
			FunctionTemplate::New(readSPI)->GetFunction());
}

NODE_MODULE(rSPI, Init)

