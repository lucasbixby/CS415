CC = gcc
CFLAGS = -Wall -g

lab3.exe: main.o string_parser.o command.o
	$(CC) $(CFLAGS) main.o string_parser.o command.o -o lab3.exe

main.o: main.c string_parser.h command.h
	$(CC) $(CFLAGS) -c main.c -o main.o 

string_parser.o: string_parser.c string_parser.h
	$(CC) $(CFLAGS) -c string_parser.c -o string_parser.o

command.o: command.c command.h
	$(CC) $(CFLAGS) -c command.c -o command.o

clean:
	rm -f *.o lab3.exe
