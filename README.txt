Sham Dorairaj
-------------------------------------------------------------------------
HOW TO RUN THE CODE:

STEP 1:

run the make file by typing 'make' in the terminal

STEP 2:

open two workstations. One pointing for the client and another for server

STEP 3:

in the machine for the server open 2 terminals
in the machine for the server enter the command ./tcpds   
in a second terminal enter ./ftps

STEP 4:

in the machine for the client open 3 terminals
in the machine for the client open the troll using the command ./troll 19869  (this port number is hardcoded)
in a second terminal enter ./tcpdc
in a third terminal enter ./ftpc <serverIPaddress> <fileToSend>
-------------------------------------------------------------------------
CHECKSUM IMPLEMENTATION:

The checksum function is implemented in the tcpd file. The client sends a checksum along with the data. When the server receives this, it recomputes the checksum using the function and displays the value. If there is a garbled packet (garbled by Troll) then the checksum on the recived end does not match and an error message is displayed for that packet.

-------------------------------------------------------------------------
FILES SUBMITTED:

The project consists of:

ftps.c    and    ftpc.c   and    tcpds.c    and    tcpdc.c    and   SocketFunc.h   and    makefile

-------------------------------------------------------------------------
WHAT THE PROGRAM DOES: 

The ftps.c implements a program to start a server application listening to a particular port and it accepts incoming connections at that port.

The ftpc.c implements a program to start a client application that connects to a specified server that is listening on a port. 

The tcpds.c is a daemon that accepts data from the troll and passes it to the server
The tcpdc.c is a daemon that accepts data from the client and passes it to the troll
The once the connection is established, the client sends a file to the server. 
The server accepts the file and stores it locally.
To verify the file has been transferred, open the "transfer" folder and check if file transfer is done. 
Additionally, the unix command "diff" can be used to verify the file transfer.

-------------------------------------------------------------------------
