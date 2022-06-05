
CC=g++
FLAGS=-Wall -pthread

all: ex6 Reactor

ex6:  main1.o guard Singleton client
	$(CC) $(FLAGS)  main1.o  -o server

main1.o: main1.cpp
	$(CC) $(FLAGS) -c main1.cpp

guard: guard.cpp
	$(CC) $(FLAGS) -o guard guard.cpp

Singleton: Singleton.cpp
	$(CC) $(FLAGS) -o Singleton Singleton.cpp

client: client.cpp
	$(CC) $(FLAGS) -o client client.cpp 

Reactor: Reactorserver.o Reactor.o Reactorc
	$(CC) $(FLAGS)  Reactorserver.o Reactor.o -o serverreactor

Reactorserver.o : Reactorserver.cpp
	$(CC) $(FLAGS) -c Reactorserver.cpp

Reactor.o : Reactor.cpp
	$(CC) $(FLAGS) -c Reactor.cpp

Reactorc : Reactorclient.cpp
	$(CC) $(FLAGS) -o clientreactor Reactorclient.cpp

clean:
	rm -f *.o .a server client Singleton guard serverreactor clientreactor