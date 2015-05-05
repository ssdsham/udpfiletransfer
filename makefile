#Makefile
CC = gcc
OBJSRV = ftps.c
OBJTCPDS = tcpds.c
OBJCLI = ftpc.c
OBJTCPDC = tcpdc.c


all:	ftps tcpds ftpc tcpdc
ftps:	$(OBJSRV)
	$(CC) $(CFLAGS) $(OBJSRV) $(OBJFUN) -o $@
tcpds:	$(OBJTCPDS)
	$(CC) $(CFLAGS) $(OBJTCPDS) $(OBJFUN) -o $@
ftpc:	$(OBJCLI)
	$(CC) $(CFLAGS) $(OBJCLI) $(OBJFUN) -o $@
tcpdc:	$(OBJTCPDC)
	$(CC) $(CFLAGS) $(OBJTCPDC) $(OBJFUN) -o $@

clean:
	rm ftps tcpds ftpc tcpdc
