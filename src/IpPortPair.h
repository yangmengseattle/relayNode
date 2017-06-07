/*
 * IpPortPair.h
 *
 *  Created on: Jun 5, 2017
 *      Author: qiuping
 */

#ifndef IPPORTPAIR_H_
#define IPPORTPAIR_H_

#include <string>
using namespace std;

class IpPortPair {
public:
	IpPortPair(string, int);
	virtual ~IpPortPair();

	string ipAddr;
	int    port;
	int    connectionFd;
};

#endif /* IPPORTPAIR_H_ */
