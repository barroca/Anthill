#include "Util.h"

string stringToString(string str) {
	return str;
}

void stringDCBuffer(string str, DCBuffer *buffer) {
	buffer->pack("s", str.c_str());
}

void matFunction7(string s1, string s2, string s3, string s4, string s5, string s6) {

}

void matFunction4(string s1, string s2, string s3, string s4) {

}

void matFunction1(string s1) {

}

string DCBufferString(DCBuffer *buffer) {
	string str;
	buffer->unpack("s", &str);
	return str;
}
