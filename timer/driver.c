
#include "timer.h"

int main()
{

starttimer(20.0,1);
starttimer(10.0,2);
starttimer(30.0,3);
sleep(5);
canceltimer(2);
starttimer(20.0,4);
sleep(5);
starttimer(18.0,5);

sleep(20);
sleep(1);
starttimer(1.0,6);
return 0;

}

