PROGRAM=MyEth
OBJS=main.o utils.o ether.o arp.o ip.o icmp.o
SRCS=$(OBJS:%.o=%.c)
CFLAGS=-Wall -g
LDFLAGS=-pthread
$(PROGRAM):$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(PROGRAM) $(OBJS) $(LDLIBS)
clean:
	rm $(OBJS) $(PROGRAM)
