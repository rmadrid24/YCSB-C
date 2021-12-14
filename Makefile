CC=g++
CFLAGS=-std=c++11 -Wall -pthread -I./ -I/home/jieliu/.local/ -I/home/jieliu/HdrHistogram_c/src -I/usr/include -L/home/jieliu/nvm-middleware/cmake/build -L/home/jieliu/HdrHistogram_c/cmake/build/src -L/home/jieliu/.local/lib
LDFLAGS= -lpthread -ltbb -lmiddleware -lpmemkv -lhdr_histogram -lnuma -lmemcached -lmemcachedutil -lmwclient
SUBDIRS=core db
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

