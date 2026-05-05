CC = gcc
CFLAGS = -Wall -g

all: MCP iobound cpubound #writer

MCP: part1.o
	$(CC) $(CFLAGS) part1.o -o MCP

part1.o: part1.c MCP.h
	$(CC) $(CFLAGS) -c part1.c -o part1.o

iobound: iobound.c
	$(CC) $(CFLAGS) iobound.c -o iobound

cpubound: cpubound.c
	$(CC) $(CFLAGS) cpubound.c -o cpubound

#writer: writer.c
#	$(CC) $(CFLAGS) writer.c -o writer

clean:
	rm -f *.o MCP iobound cpubound