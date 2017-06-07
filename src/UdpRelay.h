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
#include <string>
#include <vector>
using namespace std;

static int TCP_PORT = 73978;

class UdpRelay{
public:
	UdpRelay (char* arg);

private:
	void commandRunnable ();
	void acceptRunnable ();
	void relayInRunnable ();
	void relayOutRunnable (BroadcastPacket packet);

	void addRelayNode(string, int);
	void deleteConnection (string);
	void handleTcpRequest(int);

	string networkSegment;
	string udpPort;
	vector<IpPortPair> relayNodes;

	Socket serverSocket;
};

#endif /* UDPRELAY_H_ */
