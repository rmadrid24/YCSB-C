CC=g++
CFLAGS=-std=c++11 -g -Wall -pthread -I./ -I/home/jiel/HdrHistogram_c/src -I/usr/include -L/home/jiel/nvm-middleware/cmake/build -L/home/jiel/HdrHistogram_c/cmake/build/src
LDFLAGS= -lpthread -ltbb -lhiredis -lmiddleware -lpmemkv -lhdr_histogram -lnuma -lprofiler
SUBDIRS=core db redis
SUBSRCS=$(wildcard core/*.cc) $(wildcard db/*.cc)
OBJECTS=$(SUBSRCS:.cc=.o)
EXEC=ycsbc

all: $(SUBDIRS) $(EXEC)

$(SUBDIRS):
	$(MAKE) -C $@

$(EXEC): $(wildcard *.cc) $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $@; \
	done
	$(RM) $(EXEC)

.PHONY: $(SUBDIRS) $(EXEC)

