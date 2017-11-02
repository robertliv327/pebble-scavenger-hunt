#Makefile for commontesting directory

# Eleph, May 2017
#

C = ..
L = ../../libcs50
PROG = parsertest
OBJS = parsertest.o
PROG1 = messagetest
OBJS1 = messagetest.o
PROG2 = kragstest
OBJS2 = kragstest.o 
LIBS =
LLIBS = $L/libcs50.a $C/common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50
CC = gcc

MAKE = make 

all: $(PROG) $(PROG1) $(PROG2)

$(PROG): $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LLIBS) $(LIBS) -o $@

$(PROG1): $(OBJS1) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LLIBS) $(LIBS) -o $@

$(PROG2): $(OBJS2) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LLIBS) $(LIBS) -o $@

.PHONY: test clean

clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG)
	rm -f $(PROG1)

