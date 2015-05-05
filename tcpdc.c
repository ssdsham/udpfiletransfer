#include "SocketFunc.h"

#include <unistd.h>

 

unsigned short checksum(unsigned short * buffer, int bytes)
{
    unsigned long sum = 0;
    unsigned short answer = 0;
    int i = bytes;
    while(i>0)
    {
            sum+=*buffer;
            buffer+=1;
            i-=2;
    }
    sum = (sum >> 16) + (sum & htonl(0x0000ffff));
    sum += (sum >> 16);
	
    return ~sum;
}

int BIND (int sockID, int bindTarget)
{
    struct sockaddr_in bindAddr;
    bindAddr.sin_family = AF_INET;
    switch (bindTarget) {
        case tcpdcSeFtpSock:
            bindAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            bindAddr.sin_port = htons(atoi(tpcdcFtpcPort));
            break;
        case tcpdcSeTrlSock:
            bindAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            bindAddr.sin_port = htons(atoi(tpcdcTrolPort));
            break;
        default:
            break;
    }
    if(bind(sockID, (struct sockaddr *)&bindAddr, sizeof(bindAddr)) < 0)
    {
        perror("Error binding socket\n");
        exit(1);
    }
    
    return 0;
}

// Function for creating socket

int createSock (int domain, int type, int protocol)
{
    int sockID;
    sockID = socket(domain, type, protocol);
    if(sockID < 0)
    {
        perror("Error opening socket\n");
        exit(1);
    }
    return sockID;
}


// Function for Data Received from Client

int receiveData (int sockid, char *buffer, int bytes, int isTcpds, char *recvName)
{
    struct sockaddr_in receiveAddr;
    packet pk;
    int receiveLen;
    int hasReceived;
    bzero(buffer, bytes);
    if(isTcpds == 0)
    {
        if((hasReceived = recvfrom(sockid, buffer, bytes, 0, (struct sockaddr *)&receiveAddr,&receiveLen )) < 0)
        {
            perror("Error receiving message\n");
            exit(1);
        }
    }
    else
    {
        if((hasReceived = recvfrom(sockid, (char *)&pk, sizeof(pk), 0, (struct sockaddr *)&receiveAddr, &receiveLen)) < 0)
        {
            perror("Error receiving message\n");
            exit(1);            
        }
        hasReceived = pk.numberOfBytes;
        bcopy(pk.buf, buffer, bytes); 
	if(pk.cs!=checksum((unsigned short *)pk.buf,bytes))
	{
	//printf("***********************");
	//printf("garbled packet Received checksum: %hu",pk.cs);
	//printf("***********************");
	 }
	else
		{
	//	printf("correct packet");
	}
	       
    }
if((strcmp(recvName,"FTPS")!=0)&&(strcmp(recvName,"TCPDC")!=0))
   { //printf("\n %s has received %d bytes and checksum=%hu \n", recvName, hasReceived,pk.cs);
	}


    return hasReceived;
}


// Send to Troll all that is received from Client

int sendData (int sockid, int dest, char *str, int bytes, char *server, char *sendName)
{
    struct sockaddr_in destAddr;        
    struct sockaddr_in tcpdsAddr;      
    struct hostent *hp;                
    int hasSent;                       
    int flag = 0;                      
    packet pk;                         
    
    destAddr.sin_family = AF_INET;
    switch (dest) {
        case tcpdcToTrolAddr:
            destAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            destAddr.sin_port = htons(atoi(trollPort));
            flag = 1;
            break; 
        default:
            break;
    }
    if (flag == 0) 
    {
        if((hasSent = sendto(sockid, str, bytes, 0, (struct sockaddr *)&destAddr, sizeof(destAddr))) < 0)
        {
            perror("Error sending message\n");
            exit(1);
        }
    } 
    else 
    {
        hp = gethostbyname(server);
        if (hp == 0) 
        {
            fprintf(stderr,"%s: unkonwn host\n",server);
            exit(1);
        }

        // Message header
        bcopy((void *)hp->h_addr, (void *)&tcpdsAddr.sin_addr, hp->h_length);
        tcpdsAddr.sin_family = AF_INET;
        tcpdsAddr.sin_port = htons(atoi(tcpdsTrolPort));
        pk.messageHeader = tcpdsAddr;
        // Message buffer
        bzero(pk.buf,bufferSize);
        bcopy(str, pk.buf, bytes);
        // Message length
        pk.numberOfBytes = bytes;

	pk.cs=checksum((unsigned short *)pk.buf,bytes);
		
	printf("and checksum value is: %hu \n",pk.cs);
        if((hasSent = sendto(sockid, (char *)&pk, sizeof(pk),0,(struct sockaddr *)&destAddr,sizeof(destAddr))) < 0)
        {
            perror("Error sending message\n");
            exit(1);
            
        }
    }
    printf("\t%s has sent %d bytes \n",sendName, bytes);
    return bytes;
}



int main(void)
{
    int tcpdcFtpcSockID;
    int tcpdcTrolSockID;
    char remoteAddr[40];
    char fileName[20];
    int fileLen;
    char buffer[bufferSize];
    int iterCount = 0;
    int hasReceived;
    
    
    printf("TCPD-Client Side..\n Waiting for data..");
    
    // Open datagram socket
    tcpdcFtpcSockID = createSock(AF_INET, SOCK_DGRAM, 0);
    tcpdcTrolSockID = createSock(AF_INET, SOCK_DGRAM, 0);
    
    // Bind to socket
    BIND(tcpdcFtpcSockID, tcpdcSeFtpSock);
    BIND(tcpdcTrolSockID, tcpdcSeTrlSock);
    
    // Remote server addr
    receiveData(tcpdcFtpcSockID, remoteAddr, 40, 0, "TCPDC");
    
    // File length
    receiveData(tcpdcFtpcSockID, (char *)&fileLen, 4, 0, "TCPDC");
    fileLen = htonl(fileLen);
    printf("File Length = %d\n",fileLen);
    sendData(tcpdcTrolSockID, tcpdcToTrolAddr,(char *)&fileLen, 4, remoteAddr, "TCPDC");
    
    // File name
    receiveData(tcpdcFtpcSockID, fileName,20, 0, "TCPDC");  
    printf("File name  = %s\n", fileName);
    sendData(tcpdcTrolSockID, tcpdcToTrolAddr,fileName, 20, remoteAddr, "TCPDC");
    
    hasReceived = bufferSize;
    while (hasReceived == bufferSize) 
    {
        iterCount++;
        bzero(buffer,bufferSize);
        hasReceived = receiveData(tcpdcFtpcSockID, buffer, bufferSize, 0, "TCPDC");
        printf("\tTCPDC has received %d bytes ", hasReceived);
        sendData(tcpdcTrolSockID, tcpdcToTrolAddr,buffer, hasReceived, remoteAddr, "TCPDC");
    }
    
    close(tcpdcFtpcSockID);
    close(tcpdcTrolSockID);
    
    return 0;
}







