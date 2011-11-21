main: objTester.cpp list.o objLoader.o obj_parser.o string_extra.o
	g++ objTester.cpp -o main list.o objLoader.o obj_parser.o string_extra.o -Wall

string_extra.o: string_extra.cpp string_extra.h
	g++ -c string_extra.cpp -o string_extra.o -Wall

obj_parser.o: obj_parser.cpp obj_parser.h
	g++ -c obj_parser.cpp -o obj_parser.o -Wall

objLoader.o: objLoader.cpp objLoader.h
	g++ -c objLoader.cpp -o objLoader.o -Wall

list.o: list.cpp list.h
	g++ -c list.cpp -o list.o -Wall

clean:
	rm *.o main

.PHONY:clean
