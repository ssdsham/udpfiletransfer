#Makefile
CC = gcc
OBJSRV = driver.c
OBJFUN = timer.c

all:	driver
driver:	$(OBJSRV)
	$(CC) $(CFLAGS) $(OBJSRV) $(OBJFUN) -o $@


clean:
	rm driver
