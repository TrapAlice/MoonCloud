CC=g++
CFLAGS=-c -Wall -ISDL -g -std=c++11
LDFLAGS= -lSDL -lSDL_net
BROKER_SOURCES=$(wildcard src/broker/*.cpp)
BROKER_OBJECTS=$(BROKER_SOURCES:.cpp=.o)
EXECUTABLE=bin/moonbroker

all: $(BROKER_SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(BROKER_OBJECTS) 
	$(CC) $(BROKER_OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean: clean-objects clean-executable

clean-objects:
	-rm src/broker/*.o

clean-executable:
	-rm $(EXECUTABLE)

rebuild: clean all
