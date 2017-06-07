#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <iostream>      // cerr

using namespace std;

extern "C"
{
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <string.h>       // bzero
#include <netinet/tcp.h>  // TCP_NODELAY
}

#define NULL_FD -1

class Socket {
 public:
  Socket( int );
  ~Socket( );
  int getClientSocket( char[] );
  //int getClientSocket( char[], int sndbufsize, bool nodelay );
  int getServerSocket( );
  //int getServerSocket( int rcvbufsize, bool nodelay );
  int port;
 private:
  int clientFd;
  int serverFd;
};  

#endif


  
