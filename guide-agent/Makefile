# Makefile for Guide Agent
# Group 9 Eleph, April 2017

C = ../common
L = ../libcs50

CC = gcc
PROG1 = guideagent

OBJS1 = opCode_handler.o GAstats.o guideagent.o
#OBJS2 = opCode_handler_tester.o opCode_handler.o                                                                                                        
LLIBS = $L/libcs50.a $C/common.a -lncurses
# add in to LLIBS later: $C/common.a                                                                                                                     
CFLAGS = -Wall -pedantic -std=gnu11 -ggdb -I$L -I$C -DNOPEBBLE
# add in to CFLAGS later: -I$C                                                                                                                           

.PHONY: all clean

all: $(PROG1) 

$(PROG1): $(OBJS1) $(LLIBS)
#	$(CC) $(CFLAGS) $^ -o $@

#$(PROG2): $(OBJS2) $(LLIBS)                                                                                                                             
#       $(CC) $(CFLAGS) $^ -o $@                                                                                                                         


# object files depend on include files                                                                                                                   

opCode_handler.o: opCode_handler.h GAstats.h $C/message.h $C/krags.h $L/hashtable.h $L/memory.h $L/bag.h 
#gameserver.o: gamestats.h opCode_handler.h $C/message.h $L/file.h $L/bag.h $L/hashtable.h message_handler.h $C/krags.h
GAstats.o: GAstats.h $L/hashtable.h $L/bag.h $L/set.h $L/counters.h

# clean up after our compilation                                                                                                                         
clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG1)

