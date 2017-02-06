#	Makefile for backdoor program
#	Authors: Tyrone Lagore (10151950) and James MacIsaac (10063078)
#	Contact: tyrone.lagore@ucalgary.ca  james.macisaac@ucalgary.ca
#	Tutorial: Tyrone-T01 James-T03

CC=g++
PROXY_TARGET=proxy
PROXY_SOURCES=pmain.cpp proxy_master.h proxy_master.cpp proxy_worker.h proxy_worker.cpp
SDIR=src/
BDIR=bin/

all: clean proxy

proxy:
	@if test -d $(BDIR); then :; else mkdir $(BDIR); fi;
	$(CC) $(OPT) $(addprefix $(SDIR),$(PROXY_SOURCES)) -o $(BDIR)$(PROXY_TARGET)

clean:
	rm -f *.o $(BDIR)*
	rm -f $(SDIR)*~
	rm -f *~
