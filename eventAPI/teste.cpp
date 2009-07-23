#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <assert.h>
#include <stdio.h>

#define KB_1          1024
#define KB_2          2048
#define KB_4          4096
#define KB_8          8192
#define KB_16        16384
#define KB_32        32768
#define KB_64        65536
#define KB_128      131072
#define KB_256      262144
#define KB_512      524288
#define MB_1       1048576
#define MB_2       2097152
#define MB_4       4194304
#define MB_8       8388608
#define MB_16     16777216 
#define MB_32     33554432
#define MB_64     67108864
#define MB_128   134217728
#define MB_256   268435456
#define MB_512   536870912
#define GB_1    1073741824



using namespace std;

typedef int streamHandlerInput, streamHandlerOutput, streamHandlerType;

// Warnning: Take care when u are using this class, because we do not make 
// copies or free the data in all its functions. We do it to have better 
// performance. Case you need it you can implement these funcions
class AHEvent
{
	private:
		void *data;
		unsigned int dataSize;
		string label;
		streamHandlerType streamHandler;

	public:
		AHEvent(){
			dataSize = 20;	
			data = NULL;
			streamHandler = -1;
		};
		AHEvent(void *data, int dataSize, streamHandlerType streamHandler, string *label = NULL){
			// Warnning: I am not copying the data to have better perfomance
			// so take care when you are using this funcion
			this->data = data;	
			this->dataSize = dataSize;
			if(streamHandler != -1){
				this->streamHandler = streamHandler;
				if(label != NULL){
					this->label = *label;
				}
			}else{
				if(label != NULL){
					printf("Warnning: You have seted the label, but not the streamHandler.\n");	
					this->label = *label;
				}
			}
			
		};
		~AHEvent(){};

		void *getData(){
			return data;
		}
		void setData(void *data){
			this->data = data;
		}
		string getLabel(){
			return label;	
		}
		void setLabel(string label){
			this->label = label;
		}
		streamHandlerType getStreamHandler(){
			return streamHandler;	
		}
		void setStreamHandler(streamHandlerType streamHandler){
			this->streamHandler = streamHandler;
		}

		unsigned int size(){
			return (unsigned int)(dataSize + label.size() + sizeof(streamHandlerType));
		}
		void *serialize(int *size){
			*size = dataSize;			
			return data;
		}
		
};
AHEvent lala(){
	AHEvent lau;
	return lau;
}

int main(){
	
	AHEvent ah;
	printf("AHEvent = %d\n", ah.size());
	AHEvent lau = lala();
	lau.setStreamHandler((streamHandlerType)1);

	printf("AHEventLAu = %d\n", lau.getStreamHandler());
	AHEvent* teste = new AHEvent();
	delete teste;

}

