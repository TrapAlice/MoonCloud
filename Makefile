CC=g++
CFLAGS=-c -Wall -ISDL -g -std=c++11
LDFLAGS= -lSDL -lSDL_net
BROKER_SOURCES=$(wildcard src/broker/*.cpp)
BROKER_OBJECTS=$(BROKER_SOURCES:.cpp=.o)
BROKER_EXECUTABLE=bin/moonbroker
NODE_SOURCES=$(wildcard src/node/*.cpp)
NODE_OBJECTS=$(NODE_SOURCES:.cpp=.o)
NODE_EXECUTABLE=bin/moonnode
CLIENT_SOURCES=$(wildcard src/client/*.cpp)
CLIENT_OBJECTS=$(CLIENT_SOURCES:.cpp=.o)
CLIENT_EXECUTABLE=bin/moonclient

all: utility broker node client

utility: 
	$(CC) $(CFLAGS) src/util.cpp -o src/util.o

broker: $(BROKER_SOURCES) $(BROKER_EXECUTABLE)

node: $(NODE_SOURCES) $(NODE_EXECUTABLE)

client: $(CLIENT_SOURCES) $(CLIENT_EXECUTABLE)

$(BROKER_EXECUTABLE): $(BROKER_OBJECTS) 
	$(CC) $(BROKER_OBJECTS) src/util.o $(LDFLAGS) -o $@

$(NODE_EXECUTABLE): $(NODE_OBJECTS) 
	$(CC) $(NODE_OBJECTS) src/util.o $(LDFLAGS) -o $@

$(CLIENT_EXECUTABLE): $(CLIENT_OBJECTS) 
	$(CC) $(CLIENT_OBJECTS) src/util.o $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean: clean-broker clean-node clean-client
	-rm src/util.o

clean-broker: clean-broker-objects clean-broker-executable

clean-node: clean-node-objects clean-node-executable

clean-client: clean-client-objects clean-client-executable

clean-broker-objects:
	-rm src/broker/*.o

clean-broker-executable:
	-rm $(BROKER_EXECUTABLE)

clean-node-objects:
	-rm src/node/*.o

clean-node-executable:
	-rm $(NODE_EXECUTABLE)

clean-client-objects:
	-rm src/client/*.o

clean-client-executable:
	-rm $(CLIENT_EXECUTABLE)

rebuild: clean all
