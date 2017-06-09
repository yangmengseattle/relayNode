//-----------------------------UdpRelay.h-------------------------------------------
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

#ifndef UDPRELAY_H_
#define UDPRELAY_H_

#include "IpPortPair.h"
#include "Socket.h"
#include "BroadcastPacket.h"
#include "UdpMulticast.h"

#include <string>
#include <vector>
using namespace std;

//* since my student ID is ending with 73978, and it exceeds the max port number 65535, so I used another port instead.
static int TCP_PORT = 23978;

class UdpRelay{
public:
	UdpRelay (char* arg);
	virtual ~UdpRelay();

private:
	void commandRunnable ();
	void acceptRunnable ();
	void relayInRunnable ();
	void relayOutRunnable (char*);

	void addRelayNode(string, int);
	void deleteConnection (string);
	void handleTcpRequest(int);

	string localhostIP;
	string groupIP;
	int    groupUdpPort;
	vector<IpPortPair> relayNodes;

	UdpMulticast* ptrUdpMulticast;
	Socket serverSocket;
};

#endif /* UDPRELAY_H_ */
