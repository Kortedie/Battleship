programa: all
	./server 8080

all: cliente.o server.o
	g++ -o cliente cliente.o
	g++ -o server server.o

cliente.o: clienteprueba.cpp
	g++ -c clienteprueba.cpp

server.o: server.cpp
	g++ -c server.cpp

clean:
	rm -f server cliente server.o cliente.o