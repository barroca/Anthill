#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include "DCBuffer.h"
#include "api_util.h"

using namespace std;

string stringToString(string str);
void stringDCBuffer(string str, DCBuffer *buffer);
void matFunction1(string s1);
void matFunction4(string s1, string s2, string s3, string s4);
void matFunction7(string s1, string s2, string s3, string s4, string s5, string s6);
string DCBufferString(DCBuffer *buffer);

#endif
