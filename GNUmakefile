build: main.o hash_table.o every_define.o
	gcc -Wall main.o hash_table.o every_define.o -o so-cpp

main.o: main.c
	gcc -Wall -c main.c -o main.o

hash_table.o: hash_table.c
	gcc -Wall -c hash_table.c -o hash_table.o

every_define.o: every_define.c
	gcc -Wall -c every_define.c -o every_define.o

clean: so-cpp every_define.o hash_table.o main.o
	rm -rf $^
