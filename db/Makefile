CC=g++
CFLAGS=-std=c++11 -g -c -Wall -fgnu-tm
INCLUDES=-I../ -I/home/jieliu/nvm-middleware -I/home/jieliu/nvm-middleware/cmake/build -I/home/jieliu/.local/include
SOURCES=$(wildcard *.cc)
OBJECTS=$(SOURCES:.cc=.o)

all: $(SOURCES) $(OBJECTS)

.cc.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	$(RM) $(OBJECTS)

