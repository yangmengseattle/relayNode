/*
 * BroadcastPacket.cpp
 *
 *  Created on: Jun 6, 2017
 *      Author: qiuping
 */

#include "BroadcastPacket.h"
#include "IpAddress.h"
#include <vector>
#include <string.h>
using namespace std;

BroadcastPacket::BroadcastPacket() {
	// TODO Auto-generated constructor stub
	header[0] = (char) -32;
	header[1] = (char) -31;
	header[2] = (char) -30;
}

BroadcastPacket::BroadcastPacket(char* serialization) {
	deserialize(serialization);
}

string& BroadcastPacket::getMessage() {
	return message;
}

void BroadcastPacket::setMessage(string& message) {
	this->message = message;
}

void BroadcastPacket::addIp(string ip) {
	IpAddress ipAddr(ip);
	relayIPs.push_back(ipAddr.toInteger());
}

bool BroadcastPacket::containsIp(string ip) {
	IpAddress ipAddr(ip);
	for(vector<unsigned int>::iterator iter = relayIPs.begin(); iter != relayIPs.end(); iter++) {
		if(*iter == ipAddr.toInteger()) {
			return true;
		}
	}
	return false;
}
int BroadcastPacket::getLengthInBytes() {
	return 4 + 4 * relayIPs.size() + message.length() + 1;  //header(3) + size(1) + 4 * count of IPs + message length + 1 for '\0'
}

char* BroadcastPacket::serialize() {
	char* bytes = new char[getLengthInBytes()];
	char* dst = bytes;
	memcpy(dst, header, sizeof(header));
	dst += sizeof(header);
	char size = (char)relayIPs.size();
	memcpy(dst, &size, sizeof(char));
	dst++;
	for(vector<unsigned int>::iterator iter = relayIPs.begin(); iter != relayIPs.end(); iter++) {
		unsigned int ip = * iter;
		int masks[4] = {0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff};
		for(int i = 0; i < 4; i++) {
			*dst++ = (char)((ip & masks[i]) >> (8 * (3-i)));
			//ip = ip >> 8;
		}
	}
	const char* str = message.c_str();
	int len = strlen(str);
	memcpy(dst, str, len);
	dst += len;
	*dst = '\0';
	return bytes;
}

void SwapEndian(int &val)
{
    val = (val<<24) | ((val<<8) & 0x00ff0000) |
          ((val>>8) & 0x0000ff00) | (val>>24);
}
void BroadcastPacket::deserialize(char* serialization) {
	//assert header is correct.
	if(serialization[0] != -32 || serialization[1] != -31 || serialization[2] != -30) {
		return;
	}

	header[0] = (char) -32;
	header[1] = (char) -31;
	header[2] = (char) -30;

	int ipCount = serialization[3];
	relayIPs.clear();
	int* ipSegs = (int*)(serialization + 4);
	for(int i = 0; i < ipCount; i++) {
		int ip = ipSegs[i];
		SwapEndian(ip);
		relayIPs.push_back(ip);
	}
	char* strMsg = serialization + 4 + 4 * ipCount;
	message = string(strMsg);
}



BroadcastPacket::~BroadcastPacket() {
	// TODO Auto-generated destructor stub
}

