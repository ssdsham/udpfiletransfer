#ifndef _timer_h
#define _timer_h


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


#include <sys/time.h>
#include <signal.h>
#include <sys/socket.h>			
#include <arpa/inet.h>			

#define TIMER_PORT_IN 5656
#define TIMEVAL2UNSIGNED(SSD) ((SSD.tv_sec*1000) + ((SSD.tv_usec + 500)/1000))
#define TRUE 1
#define FALSE 0


extern void timer_calibrate();
extern void print_list();
extern void starttimer(int timevalue,int seq_no);
extern void canceltimer(int seq_no);
extern void time_update();

#endif
