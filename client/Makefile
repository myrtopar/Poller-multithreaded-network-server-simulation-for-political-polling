CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Wno-unused-parameter
LDFLAGS = -pthread
GDBFLAGS = -g
OBJ1 = pollSwayer.o
OBJ2 = swayerMethods.o

all: pollSwayer

pollSwayer.o: pollSwayer.c header.h
	$(CC) $(CFLAGS) $(GDBFLAGS) -c pollSwayer.c

swayerMethods.o: swayerMethods.c header.h
	$(CC) $(CFLAGS) $(GDBFLAGS) -c swayerMethods.c

pollSwayer: $(OBJ1) $(OBJ2)
	$(CC) $(CFLAGS) $(GDBFLAGS) $(LDFLAGS) -o pollSwayer $(OBJ1) $(OBJ2)

debug: pollSwayer
	gdb ./pollSwayer

clean:
	rm -f pollSwayer *.o
