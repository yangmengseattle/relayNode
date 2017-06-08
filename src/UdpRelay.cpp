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
	cout << "network segment: " << groupIP << " , port : " << groupUdpPort <<endl;
	ptrUdpMulticast = new UdpMulticast(const_cast<char*>(groupIP.c_str()), groupUdpPort);

	thread commandThread([this] { commandRunnable(); });
	thread acceptThread([this] { acceptRunnable(); });
	thread relayInThread( [this] { relayInRunnable(); });
	commandThread.join();
}

UdpRelay::~UdpRelay() {
	if(ptrUdpMulticast != NULL) {
		delete ptrUdpMulticast;
	}
}

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


void UdpRelay::acceptRunnable () {

	// create  a socket object with a given TCP port (73978) and keeps accepting a TCP connection request from a remote UdpRelay
	// check if another TCP connection has been already established to that remote node
	//   if so, delete the former connection
	//          start relayOutThread that keeps reading a UDP multicast message relayed thru this TCP connection from the remote
	//          node and multicasting it to the local group.

	int serverSocketFd = serverSocket.getServerSocket();
	while (true) {
		cout << "before accept:" << endl;
		// Read to accept new requests
		int clientConnection = NULL_FD;
		sockaddr_in newSockAddr;
		socklen_t newSockAddrSize = sizeof( newSockAddr );

		if ((clientConnection = accept(serverSocketFd, (sockaddr*)&newSockAddr, &newSockAddrSize)) < 0) {
			cout << "Cannot accept from another host." <<endl;
			//exit(1);
		}

		handleTcpRequest(clientConnection);
		cout << "after handleTcpRequest" << endl;
	}
}




void UdpRelay::handleTcpRequest(int clntSocket)
{
    char echoBuffer[RCVBUFSIZE];        //* Buffer for echo string
    int recvMsgSize;                    //* Size of received message

    while ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) > 0) {
    	// thread relayOutThread( [this] { relayOutRunnable(echoBuffer); }, echoBuffer);
    	echoBuffer[recvMsgSize] = '\0';

        cout << "in handleTcpRequest, received " << recvMsgSize << "bytes, they are:" << endl;
        printHex(echoBuffer);

        BroadcastPacket receivedPacket(echoBuffer);
    	if (!receivedPacket.containsIp(localhostIP)) {
    		//receivedPacket.addIp(localhostIP);
    		char* serialized = receivedPacket.serialize();
    		memcpy (echoBuffer, serialized, receivedPacket.getLengthInBytes());
    		delete serialized;

    		thread relayOutThread(&UdpRelay::relayOutRunnable, this, echoBuffer);
    		relayOutThread.join();
    	} else {
    	    // the localhost IP is contained in the UDP header, then just ignore it.
    	    cout << "HOORAY!! handleTcpRequest !! I GOT A MESSAGE WITH MY IP IN THE HEADER !!" << endl;
    	}
    }
}

/**
 *
 * @return the connection FD to the IP & port.
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
	cout<< "relayNodes.size: " << relayNodes.size() << endl;
}

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
    	// if the packet contain this IP address, then discard the packet.
    	// otherwise, spawn a thread to send to relay nodes.
    	BroadcastPacket receivedPacket(echoBuffer);
    	if (!receivedPacket.containsIp(localhostIP)) {
    		receivedPacket.addIp(localhostIP);
        	for (vector<IpPortPair>::iterator iter = relayNodes.begin(); iter != relayNodes.end(); iter++) {
        		if (iter->connectionFd != NULL_FD && send(iter->connectionFd, echoBuffer, lengthReceived, 0) != lengthReceived) {
        			cout << "send() failed to " << iter->ipAddr << ":" << iter->port << endl;
        		    //exit(1);
        		}
        	}
    	} else {
    		// the localhost IP is contained in the UDP header, then just ignore it.
    		cout << "HOORAY!! I GOT A MESSAGE WITH MY IP IN THE HEADER !!" << endl;
    	}
    }
}

void UdpRelay::relayOutRunnable (char* msg) {
	ptrUdpMulticast->multicast(msg);
}

