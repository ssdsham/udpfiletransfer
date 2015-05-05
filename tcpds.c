#include "SocketFunc.h"

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
        case tcpdsSeFtpSock:
            bindAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            bindAddr.sin_port = htons(atoi(tcpdsPort));
            break;
        case tcpdsRvTrlSock:
            bindAddr.sin_addr.s_addr = INADDR_ANY;
            bindAddr.sin_port = htons(atoi(tcpdsTrolPort));
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


// Function for data received from Troll

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
	printf("***********************\n");
	printf("garbled packet! Received checksum: %hu \n", checksum((unsigned short *)pk.buf,bytes));
	printf("***********************\n"); }
	else
		{
	printf("\t %s has received %d bytes and checksum value is:%hu \n", recvName, hasReceived,pk.cs);
	}
	       
    }
if((strcmp(recvName,"FTPS")!=0)&&(strcmp(recvName,"TCPDC")!=0))
{ // printf("\t %s has received %d bytes and checksum=%hu \n", recvName, hasReceived,pk.cs);
	}  


    return hasReceived;
}


// Function for Data Sent to Server

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
        case tcpdsToFtpsAddr:
            destAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            destAddr.sin_port = htons(atoi(ftpsPort));
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


        bcopy((void *)hp->h_addr, (void *)&tcpdsAddr.sin_addr, hp->h_length);
        tcpdsAddr.sin_family = AF_INET;
        tcpdsAddr.sin_port = htons(atoi(tcpdsTrolPort));
        pk.messageHeader = tcpdsAddr;
        bzero(pk.buf,bufferSize);
        bcopy(str, pk.buf, bytes);
        pk.numberOfBytes = bytes;
	pk.cs=checksum((unsigned short *)pk.buf,bytes);
		
	printf("checksum : %hu \n",pk.cs);
        if((hasSent = sendto(sockid, (char *)&pk, sizeof(pk),0,(struct sockaddr *)&destAddr,sizeof(destAddr))) < 0)
        {
            perror("Error sending message\n");
            exit(1);
            
        }
    }
    printf("\t %s has sent %d bytes \n",sendName, bytes);
    return bytes;
}


int main(void)
{

    int tcpdsSockID;     
    int tcpdsRvTrlSockID; 
    int fileLen;         
    int length;
    char fileName[20];   
    char buffer[bufferSize]; 
    int iterCount = 0;
    int hasReceived;
    
   printf("\t TCPD-Server Side..\n");

    //Datagram socket
    tcpdsSockID = createSock(AF_INET, SOCK_DGRAM, 0);
    tcpdsRvTrlSockID = createSock(AF_INET, SOCK_DGRAM, 0);
    
    //Bind socket
    BIND(tcpdsSockID, tcpdsSeFtpSock);
    BIND(tcpdsRvTrlSockID, tcpdsRvTrlSock);
       
    // File length

    receiveData(tcpdsRvTrlSockID, (char *)&fileLen, 4, 1, "TCPDS");
    length = fileLen;
    fileLen = htonl(fileLen);
    printf("file length = %d\n",fileLen);
    sendData(tcpdsSockID, tcpdsToFtpsAddr, (char *)&fileLen, 4, NULL, "TCPDS");
    
    // File name
    receiveData(tcpdsRvTrlSockID, fileName, 20, 1, "TCPDS");
    printf("file name = %s\n",fileName);
    sendData(tcpdsSockID, tcpdsToFtpsAddr, fileName, 20, NULL, "TCPDS");
    
    hasReceived = bufferSize;
    while (hasReceived == bufferSize) 
    {
        iterCount++;
        bzero(buffer, bufferSize);
        hasReceived = receiveData(tcpdsRvTrlSockID, buffer, bufferSize, 1, "TCPDS");
        //printf("\t tcpds has received %d bytes: \n", hasReceived);
        sendData(tcpdsSockID, tcpdsToFtpsAddr, buffer, hasReceived, NULL, "TCPDS" );
    }
    close(tcpdsSockID);
    close(tcpdsRvTrlSockID);
    
    return 0;
}


