CC=g++
CFLAGS=-c -Wall -ISDL -g -std=c++11
LDFLAGS= -lSDL -lSDL_net
BROKER_SOURCES=$(wildcard src/broker/*.cpp)
BROKER_OBJECTS=$(BROKER_SOURCES:.cpp=.o)
BROKER_EXECUTABLE=bin/moonbroker
NODE_SOURCES=$(wildcard src/node/*.cpp)
NODE_OBJECTS=$(NODE_SOURCES:.cpp=.o)
NODE_EXECUTABLE=bin/moonnode

all: broker node

broker: $(BROKER_SOURCES) $(BROKER_EXECUTABLE)

node: $(NODE_SOURCES) $(NODE_EXECUTABLE)

$(BROKER_EXECUTABLE): $(BROKER_OBJECTS) 
	$(CC) $(BROKER_OBJECTS) $(LDFLAGS) -o $@

$(NODE_EXECUTABLE): $(NODE_OBJECTS) 
	$(CC) $(NODE_OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean: clean-objects clean-executable

clean-objects:
	-rm src/broker/*.o

clean-executable:
	-rm $(BROKER_EXECUTABLE)

rebuild: clean all
