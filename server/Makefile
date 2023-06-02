CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Wno-unused-parameter
LDFLAGS = -pthread
GDBFLAGS = -g
OBJ1 = pollerMethods.o
OBJ2 = poller.o
OBJ3 = workerMethods.o

all: poller

pollerMethods.o: pollerMethods.c header.h
	$(CC) $(CFLAGS) $(GDBFLAGS) -c pollerMethods.c

workerMethods.o: workerMethods.c header.h
	$(CC) $(CFLAGS) $(GDBFLAGS) -c workerMethods.c

poller.o: poller.c header.h
	$(CC) $(CFLAGS) $(GDBFLAGS) -c poller.c

poller: $(OBJ1) $(OBJ2) $(OBJ3)
	$(CC) $(CFLAGS) $(GDBFLAGS) $(LDFLAGS) -o poller $(OBJ1) $(OBJ2) $(OBJ3)

debug: poller
	gdb ./poller 

clean:
	rm -f poller *.o poll*.txt
