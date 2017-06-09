//-----------------------------UdpRelay.cpp----------------------------------------
// Programmer's Name: Meng Yang
// Course Section Number: CSS503
// Creation Date: May 20th, 2017
// Date of Last Modification: June 5th, 2017
// --------------------------------------------------------------------------------
// Purpose: This assignment intends to familiarize us with the implementation of
// UDP/TCP, together with threads.
// --------------------------------------------------------------------------------
// Notes:
//---------------------------------------------------------------------------------
#include "UdpRelay.h"
#include "Socket.h"
#include "BroadcastPacket.h"
#include "utils.h"
#include "UdpMulticast.h"
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

using namespace std;

#define RCVBUFSIZE 2048    // Size of receive buffer
#define MAXPENDING 15    // Maximum outstanding connection requests


void handleTcpRequest(int);


UdpRelay::UdpRelay (char* arg)
	   : serverSocket(TCP_PORT), ptrUdpMulticast(NULL) {
	string strArg(arg);
	vector<string> ipAndPort = split(strArg, ":");
	groupIP = ipAndPort.at(0);
	groupUdpPort = stoi(ipAndPort.at(1));
	localhostIP = getLocalIpAddress();
	cout << "UdpRelay: booted up at " << groupIP << " , port : " << groupUdpPort <<endl;
	ptrUdpMulticast = new UdpMulticast(const_cast<char*>(groupIP.c_str()), groupUdpPort);

	thread commandThread(&UdpRelay::commandRunnable, this);
	thread acceptThread(&UdpRelay::acceptRunnable, this);
	thread relayInThread(&UdpRelay::relayInRunnable, this);
	commandThread.join();
}

UdpRelay::~UdpRelay() {
	if(ptrUdpMulticast != NULL) {
		delete ptrUdpMulticast;
	}
}

/**
 * the method to run in commandThread.
 */
void UdpRelay::commandRunnable () {
	string commandLine = "";
	string addCommand = "add";
	string deleteCommand = "delete";
	while (true) {
		cin >> commandLine;
		cout << "command line: " << commandLine << endl;
		if (commandLine.compare("quit") == 0) {
			break;
		} else if (commandLine.compare("help") == 0) {
			cout << "UdpRelay.commandThread: accepts..." << endl
				 << "  add remoteIp:remoteTcpPort" << endl
				 << "  delete remoteIp" <<endl
				 << "  show" << endl
				 << "  help" << endl
				 << "  quit" << endl;

		} else if (commandLine.compare("show") == 0)  {
			cout << "Current connections: " << endl;
			for (vector<IpPortPair>::iterator iter = relayNodes.begin(); iter != relayNodes.end(); iter++) {
			    cout << "  " << iter->ipAddr << " : " << iter->port << endl;
			}
		} else if (commandLine.compare("delete") == 0)  {
			string ip = "";
			cin >> ip;
			deleteConnection(ip);
		} else if (commandLine.compare("add") == 0) {
			string ipPort = "";
			cin >> ipPort;
			vector<string> l2_split = split(ipPort, ":");
			addRelayNode(l2_split.at(0), stoi(l2_split.at(1), nullptr, 0));
		} else {
			cout << "unsupported command \"" << commandLine << "\"" << endl;
		}
	}
}

/**
 * the method to run in acceptThread.
 */
void UdpRelay::acceptRunnable () {
	int serverSocketFd = serverSocket.getServerSocket();
	while (true) {
		// Read to accept new requests
		int clientConnection = NULL_FD;
		sockaddr_in newSockAddr;
		socklen_t newSockAddrSize = sizeof( newSockAddr );

		if ((clientConnection = accept(serverSocketFd, (sockaddr*)&newSockAddr, &newSockAddrSize)) < 0) {
			cout << "Cannot accept from another host." <<endl;
			exit(1);
		}

		handleTcpRequest(clientConnection);
	}
}

/**
 * receive messages from TCP connections, if localhost IP address is not listed in UDP header,
 *  then spawn a relayOutThread to multicast to the group.
 */
void UdpRelay::handleTcpRequest(int clntSocket)
{
    char echoBuffer[RCVBUFSIZE];        //* Buffer for echo string
    int recvMsgSize;                    //* Size of received message

    while ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) > 0) {
    	cout << "in handleTcpRequest, received " << dec << recvMsgSize << "bytes, they are:" << endl;
    	echoBuffer[recvMsgSize] = '\0';
    	BroadcastPacket receivedPacket(echoBuffer);

    	if (!receivedPacket.containsIp(localhostIP)) {
    		thread relayOutThread(&UdpRelay::relayOutRunnable, this, echoBuffer);
    		relayOutThread.join();
    	} else {
    	    // the localhost IP is contained in the UDP header, then just ignore it.
    	}
    }
}

/**
 * add a new host that relay UDP message to.
 */
void UdpRelay::addRelayNode (string ipAddr, int port) {
	cout << "addRelayNode (" << ipAddr << ", " << port << ")" << endl;
	deleteConnection(ipAddr);

	IpPortPair ipPort(ipAddr, port);
	char* cstr = new char[ipAddr.length() + 1];
	strcpy(cstr, ipAddr.c_str());
	ipPort.connectionFd = serverSocket.getClientSocket(cstr);
	delete[] cstr;
	relayNodes.push_back(ipPort);
}

/**
 * delete the connections with specified IP
 */
void UdpRelay::deleteConnection (string ip) {
	for (vector<IpPortPair>::iterator iter = relayNodes.begin(); iter != relayNodes.end(); ) {
	    if (iter->ipAddr.compare(ip) == 0) {
	    	if (iter->connectionFd != NULL_FD) {
	    		close(iter->connectionFd);
	    		iter->connectionFd = NULL_FD;
	    	}
	        iter = relayNodes.erase(iter);
	    } else {
	        ++iter;
	    }
	}
}

/**
 * the method to run in relayInThread.
 */
void UdpRelay::relayInRunnable () {
	int serverFd = ptrUdpMulticast->getServerSocket();
	int clientFd = ptrUdpMulticast->getClientSocket();
    char echoBuffer[RCVBUFSIZE];
    while (true) {
        int lengthReceived = ptrUdpMulticast->recv(echoBuffer, RCVBUFSIZE);

        echoBuffer[lengthReceived] = '\0';
        cout << "in relayInRunnable, received " << lengthReceived << "bytes, they are:" << endl;
        printHex(echoBuffer);

        // deserialize to get a BroadcastPacket
    	BroadcastPacket receivedPacket(echoBuffer);

    	if (!receivedPacket.containsIp(localhostIP)) {
        	// UDP header doesn't contain this IP address, then add this IP to it and relay to other nodes.
    		receivedPacket.addIp(localhostIP);
    		char* serialized = receivedPacket.serialize();
    		int newLen = receivedPacket.getLengthInBytes();
    		memcpy (echoBuffer, serialized, newLen);
    		delete serialized;

        	for (vector<IpPortPair>::iterator iter = relayNodes.begin(); iter != relayNodes.end(); iter++) {
        		if (iter->connectionFd != NULL_FD && send(iter->connectionFd, echoBuffer, newLen, 0) != newLen) {
        			cout << "send() failed to " << iter->ipAddr << ":" << iter->port << endl;
        		    exit(1);
        		}
        	}
    	} else {
    		// the localhost IP is contained in the UDP header, then just ignore it.
    	}
    }
}

/**
 * the method to run in relayOutThread.
 */
void UdpRelay::relayOutRunnable (char* msg) {
	ptrUdpMulticast->multicast(msg);
}

