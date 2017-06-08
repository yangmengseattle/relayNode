/*
 * utils.cpp
 *
 *  Created on: Jun 6, 2017
 *      Author: qiuping
 */

#include "utils.h"

#include <thread>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



string getLocalIpAddress() {
	struct ifaddrs * ifAddrStruct=NULL;
	struct ifaddrs * ifa=NULL;
	void * tmpAddrPtr=NULL;

	getifaddrs(&ifAddrStruct);

	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr) {
			continue;
		}
		if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
			// is a valid IP4 Address
			char mask[INET_ADDRSTRLEN];
			void* mask_ptr = &((struct sockaddr_in*) ifa->ifa_netmask)->sin_addr;
			inet_ntop(AF_INET, mask_ptr, mask, INET_ADDRSTRLEN);
			if (strcmp(mask, "255.255.255.0") == 0) {
				//printf("mask:%s\n", mask);
				// Is a valid IPv4 Address
				tmpAddrPtr = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
				char addressBuffer[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
				//printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
				return addressBuffer;
			}
		}
	}
	return NULL;
}

vector<string> split(const string& text, const string& delims)
		{
	vector<string> tokens;
	size_t start = text.find_first_not_of(delims), end = 0;

	while((end = text.find_first_of(delims, start)) != string::npos)
	{
		tokens.push_back(text.substr(start, end - start));
		start = text.find_first_not_of(delims, end);
	}
	if(start != string::npos) {
		tokens.push_back(text.substr(start));
	}

	return tokens;
}


void printHex(char* arr) {
	int len = strlen(arr);
	for(int i = 0; i < len; i++) {
		cout << hex << (int)arr[i] << "  ";
	}
	cout << endl;
}
