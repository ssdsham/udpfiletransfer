to compile timer: make all
to run timer: ./driver
to clean: make clean

The delta list is maintained in timer.c

There is a doubly linked list where each node has delta-time, port number and information.

The 3 timers are started expiring at 10, 20, and 30 seconds from current time. The head of the list will have time field 10. The second node will have time field of 20-10=10 and last node will have time field of 10 as well. 

Some of the linked list code is derived from online tutorials
http://www.cprogramming.com/tutorial/c/lesson15.html
