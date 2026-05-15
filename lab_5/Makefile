all: main signal iobound

main: main.c
	gcc -o $@ $^

signal: signal.c
	gcc -o $@ $^

iobound: iobound.c
	gcc -o $@ $^

clean:
	rm -rf main signal iobound
