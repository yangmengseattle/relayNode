/*
 * IpPortPair.cpp
 *
 *  Created on: Jun 5, 2017
 *      Author: qiuping
 */

#include "IpPortPair.h"
#include "Socket.h"

IpPortPair::IpPortPair(string ip, int p) {
	ipAddr = ip;
	port = p;
	connectionFd = NULL_FD;
}

IpPortPair::~IpPortPair() {
	// do nothing.
}

