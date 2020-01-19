CC:=g++ -std=c++11
OBJS = sockop
TARGETS=server client

all: $(TARGETS)	

$(OBJS).o: $(OBJS).cpp
	$(CC) -c -o $@ $<

cards.o:cards.cpp
	$(CC) -c -o cards.o cards.cpp

server: server.cpp $(OBJS).o cards.o
	$(CC) -pthread -g -o server $^

client: client.cpp $(OBJS).o
	$(CC) -o client $^

clean:
	rm *.o
	rm $(TARGETS)
