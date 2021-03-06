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
        case ftpcSendSock:
            bindAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            bindAddr.sin_port = htons(atoi(ftpcPort));
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


// Function for sending data to TCPD

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
        case ftpcToTcpdcAddr: 
            destAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            destAddr.sin_port = htons(atoi(tpcdcFtpcPort));
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
        
        // Message Header
        bcopy((void *)hp->h_addr, (void *)&tcpdsAddr.sin_addr, hp->h_length);
        tcpdsAddr.sin_family = AF_INET;
        tcpdsAddr.sin_port = htons(atoi(tcpdsTrolPort));
        pk.messageHeader = tcpdsAddr;
        // Message buffer
        bzero(pk.buf,bufferSize);
        bcopy(str, pk.buf, bytes);
        // Message Length
        pk.numberOfBytes = bytes;
	pk.cs=checksum((unsigned short *)pk.buf,bytes);
		
	printf("checksum : %hu",pk.cs);
        if((hasSent = sendto(sockid, (char *)&pk, sizeof(pk),0,(struct sockaddr *)&destAddr,sizeof(destAddr))) < 0)
        {
            perror("Error sending message\n");
            exit(1);
            
        }
    }
    printf("\t %s has sent %d bytes \n",sendName, bytes);
    return bytes;
}


int main(int argc, char *argv[])
{
    char remoteAddr[40];
    char fileName[20];
    char buffer[bufferSize];
    int ftpcSockID;
    int fileLen;
    int length;
    int fd;
    int iterCount = 0;
    int totalBytes =0;
    struct stat statbuff;
    
    printf("\t Sending %s file ...\n",argv[2]);
    
    if(argc!=3)
    {
        printf("Usage: ftpc <remote-server> <file-to-transfer>\n");
        exit(1);
    }
    else
    {
        strcpy(remoteAddr,argv[1]);
        strcpy(fileName,argv[2]);
    } 

    // Open socket
    ftpcSockID = createSock(AF_INET, SOCK_DGRAM, 0);
    
    // Bind Socket
    BIND(ftpcSockID,ftpcSendSock);
    
    // file to be transferred
    fd = open(fileName, O_RDONLY);
    if(fd < 0)
    {
        printf("Error opening the file\n");
        exit(1);
    }
    if(stat(fileName, &statbuff) < 0)
    {
        printf("STAT cannot open file\n");
        exit(1);
    }
    fileLen = statbuff.st_size;
    fileLen = htonl(fileLen);
    
    // Remote server addr
    sendData(ftpcSockID, ftpcToTcpdcAddr, remoteAddr,40, NULL, "FTPC");
   
    // File length
    sendData(ftpcSockID, ftpcToTcpdcAddr, (char *)&fileLen, 4, NULL, "FTPC");
    
    // File name
    sendData(ftpcSockID, ftpcToTcpdcAddr, fileName, 20, NULL, "FTPC");
    
    
    while (1) 
    {
        bzero(buffer, bufferSize);
        length = read(fd, buffer, sizeof(buffer));
        totalBytes = totalBytes + length;
        if(length == 0)
            break;
        iterCount++;
        sendData(ftpcSockID, ftpcToTcpdcAddr, buffer, length, NULL, "FTPC");
        usleep(15000);
       
    }
    printf("\t ftpc has sent %s  \n  Bytes Sent = %d\n", fileName, totalBytes);
    
    close(fd);
    close(ftpcSockID);

    return 0;
}





