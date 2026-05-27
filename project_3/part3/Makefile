CC      = gcc
CFLAGS  = -g -Wall -Wextra -pthread
LDFLAGS = -lpthread
TARGET  = park
SRCS    = park.c helpers.c
OBJS    = $(SRCS:.c=.o)
 
all: $(TARGET)
 
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
 
%.o: %.c park.h
	$(CC) $(CFLAGS) -c -o $@ $<
 
clean:
	rm -f $(OBJS) $(TARGET)
 
.PHONY: all clean