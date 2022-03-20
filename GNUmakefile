build: main.o hash_table.o functions.o
	gcc -Wall main.o hash_table.o functions.o -o so-cpp

main.o: main.c
	gcc -Wall -c main.c -o main.o

hash_table.o: hash_table.c
	gcc -Wall -c hash_table.c -o hash_table.o

functions.o: functions.c
	gcc -Wall -c functions.c -o functions.o

clean: so-cpp functions.o hash_table.o main.o
	rm -rf $^
