sonnet: main.o sonnet.o
	g++ -Wall -g main.o sonnet.o -o sonnet

main.o: main.cpp sonnet.h
	g++ -Wall -c main.cpp

sonnet.o: sonnet.cpp sonnet.h
	g++ -Wall -c sonnet.cpp