stamp: main.o stamp.o
	g++ -Wall -g main.o stamp.o -o stamp -lcrypto

main.o: main.cpp
	g++ -Wall -g -c main.cpp

stamp.o: stamp.cpp stamp.h
	g++ -Wall -g -c stamp.cpp

clean:
	rm -rf *.o stamp