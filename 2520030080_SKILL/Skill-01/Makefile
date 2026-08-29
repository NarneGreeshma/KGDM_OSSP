CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = skill1

all: $(TARGET)

$(TARGET): skill1.c
	$(CC) $(CFLAGS) skill1.c -o $(TARGET)

clean:
	rm -f $(TARGET) trace.txt testtrace.txt

.PHONY: all clean
