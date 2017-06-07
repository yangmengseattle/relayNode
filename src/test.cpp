#include <iostream>   // cerr
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "IpAddress.h"
#include "BroadcastPacket.h"
using namespace std;

int main( int argc, char *argv[] ) {
	IpAddress ip1(0x12345699);
	cout << ip1.toString() << endl;;
	string ip = "66.67.68.69";
	string ipp = "72.73.74.75";
	IpAddress ip2(ip);
	cout << ip2.toInteger() << ", " << ip2.toString() <<endl;
	string s = "hello, world";
	cout << strlen( s.c_str()) << endl;

	BroadcastPacket packet;
	packet.addIp(ip);
	packet.addIp(ipp);
	packet.setMessage(s);
	char* serialized = packet.serialize();
	int len = strlen(serialized);
	for(int i = 0; i < len; i++) {
		cout << hex << (int)serialized[i] << "  ";
	}
	cout << endl;
	for(vector<unsigned int>::iterator iter = packet.relayIPs.begin(); iter != packet.relayIPs.end(); iter++) {
		IpAddress ipAddr(*iter);
		cout << ipAddr.toString() << endl;
	}

	cout << "----------------------------------------" << endl;

	BroadcastPacket deserial (serialized);
	cout << "msg: " <<  deserial.getMessage() << endl;
	for(vector<unsigned int>::iterator iter = deserial.relayIPs.begin(); iter != deserial.relayIPs.end(); iter++) {
		IpAddress ipAddr(*iter);
		cout << ipAddr.toString() << endl;
	}

	cout << "----------------------------------------" << endl;

	char* ser = deserial.serialize();
	int l = strlen(ser);
	cout << "l: " << l << endl;
	for(int i = 0; i < l; i++) {
		cout << hex << (int)ser[i] << "  ";
	}
	cout << endl;
	for(vector<unsigned int>::iterator iter = deserial.relayIPs.begin(); iter != deserial.relayIPs.end(); iter++) {
		IpAddress ipAddr(*iter);
		cout << ipAddr.toString() << endl;
	}


    return 0;
}
