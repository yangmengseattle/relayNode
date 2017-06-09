/*
 * IpAddress, it's mainly wrapping some methods around an unsigned int.
 *
 *  Created on: Jun 6, 2017
 *      Author: qiuping
 */

#ifndef IPADDRESS_H_
#define IPADDRESS_H_

#include <string>

using namespace std;

class IpAddress {
public:
	IpAddress(int ipInInteger);
	IpAddress(string ipStr);
	virtual ~IpAddress();
	void readFrom(string ipStr);

	unsigned int toInteger();
	string toString();

private:
	unsigned int ipInteger;

};

#endif /* IPADDRESS_H_ */
