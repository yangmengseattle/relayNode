/*
 * BroadcastPacket.cpp
 *
 *  Created on: Jun 6, 2017
 *      Author: qiuping
 */

#include "BroadcastPacket.h"
#include "IpAddress.h"
#include "utils.h"
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

/**
 * get the message content
 */
string& BroadcastPacket::getMessage() {
	return message;
}

/**
 * set the message content
 */
void BroadcastPacket::setMessage(string& message) {
	this->message = message;
}

/**
 * add the specified 'ip' into the UDP header.
 */
void BroadcastPacket::addIp(string ip) {
	IpAddress ipAddr(ip);
	relayIPs.push_back(ipAddr.toInteger());
}

/**
 * return true, if the specified 'ip' is listed in UDP header; otherwise, return false.
 */
bool BroadcastPacket::containsIp(string ip) {
	IpAddress ipAddr(ip);
	for(vector<unsigned int>::iterator iter = relayIPs.begin(); iter != relayIPs.end(); iter++) {
		if(*iter == ipAddr.toInteger()) {
			return true;
		}
	}
	return false;
}

/**
 * return the lenght of byte array if it's serialized.
 */
int BroadcastPacket::getLengthInBytes() {
	return 4 + 4 * relayIPs.size() + message.length() + 1;  //header(3) + size(1) + 4 * count of IPs + message length + 1 for '\0'
}

/**
 * get a byte array out of the object. For the convenience of sending via socket.
 */
char* BroadcastPacket::serialize() {
	char* bytes = new char[getLengthInBytes()];
	char* dst = bytes;

	// write the '-32,-31,-30' to byte array.
	memcpy(dst, header, sizeof(header));
	dst += sizeof(header);

	// write the count of IP addresses to the byte array.
	char size = (char)relayIPs.size();
	memcpy(dst, &size, sizeof(char));
	dst++;

	// write the IP addresses to the byte array.
	for(vector<unsigned int>::iterator iter = relayIPs.begin(); iter != relayIPs.end(); iter++) {
		unsigned int ip = swapEndian(*iter);
		memcpy(dst, &ip, sizeof(unsigned int));
		dst += sizeof(unsigned int);
	}
	// write the message content to the byte array.
	const char* str = message.c_str();
	int len = strlen(str);
	memcpy(dst, str, len);
	dst += len;
	*dst = '\0';
	return bytes;
}

/**
 * build the BroadcastPacket object from the byte array.
 */
void BroadcastPacket::deserialize(char* serialization) {
	//assert header is correct.
	if(serialization[0] != -32 || serialization[1] != -31 || serialization[2] != -30) {
		return;
	}

	// set the "-32,-31,-30".
	header[0] = (char) -32;
	header[1] = (char) -31;
	header[2] = (char) -30;

	// read the count of IP address in the UDP header
	int ipCount = serialization[3];

	// read the IP addresses in the UDP header, and push them into the vector.
	relayIPs.clear();
	int* ipSegs = (int*)(serialization + 4);
	for(int i = 0; i < ipCount; i++) {
		int ip = ipSegs[i];
		swapEndian(ip);
		relayIPs.push_back(ip);
	}

	// read the message content.
	char* strMsg = serialization + 4 + 4 * ipCount;
	message = string(strMsg);
}

BroadcastPacket::~BroadcastPacket() {
	// Auto-generated destructor stub.
}

