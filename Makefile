Server:main.o libsock.a
	g++ -o Server main.o libsock.a
	
Client:main.o libsock.a
	g++ -o Client main.o libsock.a
	
libsock.a:BaseSocket.o BaseRoutine.o BaseServer.o BaseClient.o
	ar r libsock.a BaseSocket.o BaseRoutine.o BaseServer.o BaseClient.o

BaseSocket.o: ./src/Library/OpenSocket/BaseSocket.cpp ./src/Library/OpenSocket/OpenSocket.h ./src/Library/OpenSocket/OpenSocket_STD.h ./src/Library/OpenSocket/OpenSocket_Def.h
	g++ -c -fno-builtin-memcpy ./src/Library/OpenSocket/BaseSocket.cpp

BaseRoutine.o: ./src/Library/OpenSocket/BaseRoutine.cpp ./src/Library/OpenSocket/OpenSocket.h ./src/Library/OpenSocket/OpenSocket_STD.h ./src/Library/OpenSocket/OpenSocket_Def.h
	g++ -c -fno-builtin-memcpy ./src/Library/OpenSocket/BaseRoutine.cpp

BaseServer.o: ./src/Library/OpenSocket/BaseServer.cpp ./src/Library/OpenSocket/OpenSocket.h ./src/Library/OpenSocket/OpenSocket_STD.h ./src/Library/OpenSocket/OpenSocket_Def.h
	g++ -c -fno-builtin-memcpy ./src/Library/OpenSocket/BaseServer.cpp

BaseClient.o: ./src/Library/OpenSocket/BaseClient.cpp ./src/Library/OpenSocket/OpenSocket.h ./src/Library/OpenSocket/OpenSocket_STD.h ./src/Library/OpenSocket/OpenSocket_Def.h
	g++ -c -fno-builtin-memcpy ./src/Library/OpenSocket/BaseClient.cpp

main.o: ./src/Library/OpenSocket/OpenSocket.h ./src/Library/OpenSocket/OpenSocket_STD.h ./src/Library/OpenSocket/OpenSocket_Def.h ./src/Library/OpenSocket/OpenSocket.h
	g++ -c -fno-builtin-memcpy ./src/main.cpp

clean:
	rm -f Server Client main.o BaseSocket.o BaseRoutine.o BaseServer.o BaseClient.o libsock.a
temp:
	g++ main.cpp ./src/Library/OpenSocket/BaseSocket.cpp ./src/Library/OpenSocket/BaseRoutine.cpp ./src/Library/OpenSocket/BaseServer.cpp ./src/Library/OpenSocket/BaseClient.cpp ./src/Library/OpenSocket/OpenSocket_STD.h ./src/Library/OpenSocket/OpenSocket_Def.h ./src/Library/OpenSocket/OpenSocket.h