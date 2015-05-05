#ifndef _SocketFunc_h
#define _SocketFunc_h

#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<string.h>

#define ftpsPort      "96891"
#define tcpdsPort     "86891"
#define ftpcPort      "07891"
#define tpcdcFtpcPort "99899"
#define tpcdcTrolPort "19868"
#define trollPort     "19869"
#define tcpdsTrolPort "19870" 


#define bufferSize 1000

#define ftpsSendSock   0
#define tcpdsSeFtpSock 1
#define ftpcSendSock   2
#define tcpdcSeFtpSock 3
#define tcpdcSeTrlSock 4
#define tcpdsRvTrlSock 5

#define ftpsToTcpdsAddr 6
#define tcpdsToFtpsAddr 7
#define ftpcToTcpdcAddr 8
#define tcpdcToFtpcAddr 9
#define tcpdcToTrolAddr 10


typedef struct packet {
    struct sockaddr_in messageHeader;
	int seq_no;
    char buf[bufferSize];
    int numberOfBytes;
unsigned short int cs;
} packet;


#endif
