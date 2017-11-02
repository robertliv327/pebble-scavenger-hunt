# Makefile for Game-server
#
# Eleph, May 2017

C = ../common
L = ../libcs50

CC = gcc
PROG1 = gameserver
#PROG2 = opCode_handler_tester
OBJS1 = message_handler.o opCode_handler.o gamestats.o gameserver.o
#OBJS2 = opCode_handler_tester.o opCode_handler.o 
LLIBS = $L/libcs50.a $C/common.a 
# add in to LLIBS later: $C/common.a 
CFLAGS = -Wall -pedantic -std=gnu11 -ggdb -I$L -I$C -DNOPEBBLE 
# add in to CFLAGS later: -I$C

.PHONY: all clean

all: $(PROG1) $(PROG2)

$(PROG1): $(OBJS1) $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

#$(PROG2): $(OBJS2) $(LLIBS)
#	$(CC) $(CFLAGS) $^ -o $@


# object files depend on include files

opCode_handler.o: opCode_handler.h gamestats.h $C/message.h $C/krags.h $L/hashtable.h $L/memory.h $L/bag.h message_handler.h $C/krags.h
gameserver.o: gamestats.h opCode_handler.h $C/message.h $L/file.h $L/bag.h $L/hashtable.h message_handler.h $C/krags.h
gamestats.o: gamestats.h $L/hashtable.h $L/bag.h $L/set.h $L/counters.h
message_handler.o: message_handler.h $C/message.h $L/hashtable.h


#opCode_handler_tester.o: opCode_handler.h

# clean up after our compilation
clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG1)
#	rm -f $(PROG2)
	rm -f $(PROG3)
