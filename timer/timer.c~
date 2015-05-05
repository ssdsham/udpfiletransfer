#include "timer.h"

typedef struct packet_node *packetNd;
unsigned int TIMER_in_socket;
int timeElapsed;
struct itimerval value, setvalue, xval;
struct timeval;
struct recv_struct from_tcpd;
char wrap_up_mode=FALSE;
int addr_len;
packetNd head = NULL;


// Data Structure declaration
struct recv_struct
{
	unsigned int seq_no;
	struct timeval AERT;
	int mode;
};

struct packet_node
{
	unsigned int seq_no;
	unsigned long int delta_time;
	packetNd next;
	packetNd prev;
};

// Insert function for inserting new time from driver

void starttimer(int timevalue,int seq_no)
{
	packetNd node;
	packetNd traverse;
	node = (packetNd)malloc(sizeof(struct packet_node));

	if(head==NULL)
	{
		timeElapsed=0;
	}

	node->delta_time = timevalue-timeElapsed;
	node->seq_no = seq_no;
	node->next = NULL;
	node->prev = NULL;

	if(head==NULL)
	{
		head = node;
		time_update();
		print_list();
		return;
	}
	traverse = head;
	while(1)
	{
		if(node->delta_time<traverse->delta_time)
		{
			if(traverse->prev!=NULL)
				(traverse->prev)->next = node;
			node->prev = traverse->prev;
			node->next = traverse;
			traverse->prev = node;
			traverse->delta_time = traverse->delta_time - node->delta_time;
			if (traverse==head)
			{
				head = node;
				time_update();
			}
			break;
		}
		else if(traverse->next==NULL)
		{
			node->delta_time = node->delta_time - traverse->delta_time;
			traverse->next = node;
			node->prev = traverse;
			break;
		}
		else
		{
			node->delta_time = node->delta_time - traverse->delta_time;
			traverse = traverse->next;
		}
	}
print_list();
return;
}


// Function to cancel timer based on sequence number
void canceltimer(int seq_no)
{
printf("Canceled timer with sequence number: %d \n", seq_no);
	packetNd traverse;
	traverse = head;
	while(traverse!=NULL)
	{
		if(traverse->seq_no==seq_no)
		{
			if(traverse->next!=NULL)
			{
				(traverse->next)->prev = traverse->prev;
				(traverse->next)->delta_time = (traverse->next)->delta_time + traverse->delta_time;
			}
			if(traverse->prev!=NULL)
				(traverse->prev)->next = traverse->next;
			if(traverse == head)
			{
				if(traverse->next!=NULL)
					head = traverse->next;
				else
					head = NULL;
				time_update();
			}
			free(traverse);
			break;
		}
		else traverse = traverse->next;
	}
print_list();
}

// Function to traverse and print the timers

void print_list()
{
	printf("Updated Delta Timer: \n");
	packetNd traverse;
	traverse = head;
	int s=0;
	while(traverse!=NULL)
	{	s=s+1;
		printf("Sl No: %d and Original Sequence Number: %u and delta time:%u \n", s, (unsigned)traverse->seq_no,(unsigned)traverse->delta_time);
		traverse = traverse->next;
	}
}


// Function to calculate time elapsed
void time_elapsed()
{
	packetNd traverse;
	if (head!=NULL)
	{

		printf("Timer elapsed for %d\n",head->seq_no);
		timeElapsed = timeElapsed + head->delta_time/1000;
		traverse = head;
		if(head->next==NULL)
			head=NULL;
		else
		{
			(head->next)->prev = NULL;
			head = head->next;
		}
		free(traverse);
		print_list();
	}
	while(head!=NULL&&head->delta_time==0)
	{
		printf("Timer elapsed for %d\n",head->seq_no);
		timeElapsed = timeElapsed + head->delta_time/1000;
		traverse = head;
		if(head->next==NULL)
			head=NULL;
		else
		{
			(head->next)->prev = NULL;
			head = head->next;
		}
		free(traverse);
		print_list();
	}
	if(head!=NULL){
		time_update();
	}
}


// Function to update time

void time_update()
{
	struct itimerval setvalue, xval;
	setvalue.it_interval.tv_sec=0;
	setvalue.it_interval.tv_usec=0;
	if(head!=NULL)
	{
		setvalue.it_value.tv_sec=head->delta_time;
		setvalue.it_value.tv_usec=(head->delta_time%1000)*1000;
	}
	else
	{
		setvalue.it_value.tv_sec=0;
		setvalue.it_value.tv_usec=0;
	}
	printf("Timer at head is for %u Seconds\n", (unsigned)setvalue.it_value.tv_sec);
	signal(SIGALRM,time_elapsed);
	setitimer(ITIMER_REAL, &setvalue, &xval);
}



