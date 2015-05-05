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
        case ftpsSendSock:
            bindAddr.sin_addr.s_addr = INADDR_ANY;
            bindAddr.sin_port = htons(atoi(ftpsPort));
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

// Function for receiving data from TCPD

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
	printf("***********************");
	printf("garbled packet");
	printf("***********************"); }
	else
	{
	//	printf("correct packet");
	}
	       
    }
if((strcmp(recvName,"FTPS")!=0)&&(strcmp(recvName,"TCPDC")!=0))
    printf("\t %s has received %d bytes and checksum=%hu \n", recvName, hasReceived,pk.cs);



    return hasReceived;
}




int main(void)
{

    int ftpsSockID;      
    int fileLen;         
    char fileName[20];   
    char buffer[bufferSize]; 
    char filePath[40] = "./transfer/";
    int iterCount = 0;
    int hasReceived = bufferSize;
    int totalReceived = 0;
    int fd;
    
    printf("\t ftps : Ready to receive file...\n");
    // Open socket
    ftpsSockID = createSock(AF_INET, SOCK_DGRAM,0);
    
    // Bind socket
    BIND(ftpsSockID, ftpsSendSock);
    
    // File length
    receiveData(ftpsSockID, (char *)&fileLen, 4, 0, "FTPS");
    
    printf("\tFile length = %d\n",fileLen);
    
    // File name
    receiveData(ftpsSockID, fileName, 20, 0, "FTPS");
    printf("\tFile name = %s\n",fileName);
    
    strcat(filePath,fileName);
    if ((fd = creat(filePath, 0666)) < 0) 
    {
        perror("\tCannot create file\n");
        exit(1);
    }

    while (hasReceived == bufferSize)
    {
        hasReceived = receiveData(ftpsSockID, buffer,bufferSize, 0, "FTPS");
        iterCount++;
       
        totalReceived = totalReceived + hasReceived;

        if (write(fd, buffer, hasReceived) < 0) 
        {
            perror("\tCannot write to file\n");
            exit(1);
        }
        bzero(buffer, bufferSize);
    }
    
    printf("\tFTPS has received %s  \n Total Bytes Received = %d\n",fileName, totalReceived);
    
    close(fd);
    close(ftpsSockID);
    
    return 0;
}






