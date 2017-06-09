/*
 * BroadcastPacket is used to represent the message that are being broadcast.
 * With the 'serialize' & 'deserialize' method, it could transform between byte array and C++ class with ease.
 *  Created on: Jun 6, 2017
 *      Author: qiuping
 */

#ifndef BROADCASTPACKET_H_
#define BROADCASTPACKET_H_

#include "IpAddress.h"
#include <string>
#include <vector>
using namespace std;

class BroadcastPacket {
public:
	BroadcastPacket();
	BroadcastPacket(char* serialization);
	virtual ~BroadcastPacket();
	void addIp(string ip);
	bool containsIp(string ip);

	char* serialize();
	void deserialize(char* serialization);

	int getLengthInBytes();

    string& getMessage();
	void setMessage(string& message);

	char header[3];
	// char hop; should be the size of relayIPs
	vector<unsigned int> relayIPs;
	string message;
};

#endif /* BROADCASTPACKET_H_ */
