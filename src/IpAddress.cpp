/*
 * IpAddress.cpp
 *
 *  Created on: Jun 6, 2017
 *      Author: qiuping
 */

#include "IpAddress.h"
#include "utils.h"
#include <vector>
using namespace std;

IpAddress::IpAddress(int ipInInteger) {
	ipInteger = ipInInteger;
}

IpAddress::IpAddress(string ipStr) {
	readFrom(ipStr);
}

void IpAddress::readFrom(string ipStr) {
	ipInteger = 0;
	vector<string> segs = split(ipStr, ".");
	for(vector<string>::iterator iter = segs.begin(); iter != segs.end(); iter++) {
		int seg = stoi(*iter, nullptr, 0);
		ipInteger = (ipInteger << 8) ^ seg;
	}
}


unsigned int IpAddress::toInteger() {
	return ipInteger;
}
string IpAddress::toString() {
	string s = "";
	for(int i = 0; i < 4; i++) {
		int seg = 0xff & (ipInteger >> (i * 8));
		s.insert(0, std::to_string(seg));
		if(i != 3) {
			s.insert(0, ".");
		}
	}
	return s;
}


IpAddress::~IpAddress() {
	// TODO Auto-generated destructor stub
}

