# Makefile for common library
# Builds and compiles all these utility functions into a library.
# Eleph May 2017

L = ../libcs50

# object files, and the target library

OBJS = message.o krags.o 
LIB = common.a

# add -DNOSLEEP to disable the automatic sleep after web-page fetches
#               (students, please do not use -DNOSLEEP!)
# add -DMEMTEST for memory tracking report in indexer
# (and run `make clean; make` whenever you change this)
FLAGS = # -DNOSLEEP # -DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS) -DNOPEBBLE
CC = gcc
MAKE = make

# Build the library by archiving object files

$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

#ar cr $(LIB) $(OBJS) -o $a

# Dependencies: object files depend on header files
message.o: message.h
krags.o: krags.h $L/hashtable.h $L/bag.h $L/set.h $L/counters.h $L/file.h

#parser: parser.o
.PHONY: clean sourcelist

# list all the sources and docs in this directory.
sourcelist: Makefile *.md *.c *.h
	@echo $^

# clean up after our compilation
clean:
	rm -f $(LIB) *~ *.o

