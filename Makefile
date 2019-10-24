tp2: src/helper.o src/hashing.o src/bplus.o src/main.o
	mkdir -p bin
	g++ -o bin/db src/main.o src/helper.o src/hashing.o src/bplus.o -pthread

src/helper.o: src/helper.cpp
	g++ -o src/helper.o -c src/helper.cpp

src/hashing.o: src/hashing.cpp
	g++ -o src/hashing.o -c src/hashing.cpp

src/bplus.o: src/bplus.cpp
	g++ -o src/bplus.o -c src/bplus.cpp

src/main.o: src/main.cpp
	g++ -o src/main.o -c src/main.cpp

clean:
	rm -rf src/*.o
	rm Makefile