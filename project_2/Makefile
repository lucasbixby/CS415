CC = gcc
CFLAGS = -Wall -g

all: part1 part2 iobound cpubound

# -----------------------
# Part 1
# -----------------------

part1: part1.o helpers.o
	$(CC) $(CFLAGS) part1.o helpers.o -o part1

part1.o: part1.c MCP.h
	$(CC) $(CFLAGS) -c part1.c -o part1.o

# -----------------------
# Part 2
# -----------------------

part2: part2.o helpers.o
	$(CC) $(CFLAGS) part2.o helpers.o -o part2

part2.o: part2.c MCP.h
	$(CC) $(CFLAGS) -c part2.c -o part2.o

# -----------------------
# Workload programs
# -----------------------

helpers.o: helpers.c MCP.h
	$(CC) $(CFLAGS) -c helpers.c -o helpers.o

iobound: iobound.c
	$(CC) $(CFLAGS) iobound.c -o iobound

cpubound: cpubound.c
	$(CC) $(CFLAGS) cpubound.c -o cpubound

# -----------------------
# Clean
# -----------------------

clean:
	rm -f *.o part1 part2 iobound cpubound