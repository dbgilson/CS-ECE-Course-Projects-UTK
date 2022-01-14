CC = g++
CFLAGS = -Wall

all: Mazemake Mazesolve

Mazemake: Mazemake.cpp dset.h dset.o
	$(CC) $(CFLAGS) -o $@ Mazemake.cpp dset.cpp

Mazesolve: Mazesolve.cpp dset.h dset.o
	$(CC) $(CFLAGS) -o $@ Mazesolve.cpp dset.cpp

.cpp.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f Mazemake Mazesolve


# CPP_FILES = Mazemake.cpp Mazesolve.cpp
# OBJ_FILES = Mazemake.o Mazesolve.o


# all: Mazemake Mazesolve

# main: Mazemake.cpp Mazesolve.cpp


# Mazemake.o: Mazemake.cpp dset.h


# Mazesolve.o: Mazesolve.cpp

