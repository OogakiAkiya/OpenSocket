Server:main.o BaseSocket.o BaseRoutine.o BaseServer.o BaseClient.o
	g++ -o Server main.o BaseSocket.o BaseRoutine.o BaseServer.o BaseClient.o
	
Client:main.o BaseSocket.o BaseRoutine.o BaseServer.o BaseClient.o
	g++ -o Client main.o BaseSocket.o BaseRoutine.o BaseServer.o BaseClient.o

BaseSocket.o:./Library/OpenSockets/BaseSocket.cpp include.h
	g++ -c ./Library/OpenSockets/BaseSocket.cpp

BaseRoutine.o:./Library/OpenSockets/BaseRoutine.cpp include.h
	g++ -c ./Library/OpenSockets/BaseRoutine.cpp

BaseServer.o:./Library/OpenSockets/BaseServer.cpp include.h
	g++ -c ./Library/OpenSockets/BaseServer.cpp

BaseClient.o:./Library/OpenSockets/BaseClient.cpp include.h
	g++ -c ./Library/OpenSockets/BaseClient.cpp

main.o:main.cpp include.h
	g++ -c main.cpp

clean:
	rm -f Server Client main.o BaseSocket.o BaseRoutine.o BaseServer.o BaseClient.o
