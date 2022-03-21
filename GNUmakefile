build: main.o hash_map.o functions.o
	gcc -Wall main.o hash_map.o functions.o -o so-cpp

main.o: main.c
	gcc -Wall -c main.c -o main.o

hash_map.o: hash_map.c
	gcc -Wall -c hash_map.c -o hash_map.o

functions.o: functions.c
	gcc -Wall -c functions.c -o functions.o

clean: so-cpp functions.o hash_map.o main.o
	rm -rf $^
