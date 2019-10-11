tp2: src/helper.o src/main.o
	g++ -o tp2 src/main.o src/helper.o -pthread

src/helper.o: src/helper.cpp
	g++ -o src/helper.o -c src/helper.cpp

src/main.o: src/main.cpp
	g++ -o src/main.o -c src/main.cpp

clean:
	rm -rf src/*.o
