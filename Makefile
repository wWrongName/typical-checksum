CC=g++
EXE=codec
CFLAGS=-c -Wall -std=c++17
SOURCES=functions.cpp codec.cpp main.cpp
OBJECTS=$(SOURCES:.cpp=.o)

all: $(OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) $<

$(EXE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

clear:
	rm -rf *.o $(EXE) data.txt