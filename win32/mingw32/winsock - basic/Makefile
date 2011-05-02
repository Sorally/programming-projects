# Name: main.c
# Creation Date: April, 2011
# Author: r0B
 
CC=gcc
FLAGS=
LIBS=-lws2_32
EXE_SUFFIX=.exe
NAME=main
PROGRAM=$(NAME)$(EXE_SUFFIX)
PROGRAM=main.exe

all: $(PROGRAM)

$(PROGRAM): $(NAME).o
	$(CC) -o $(PROGRAM) $(NAME).o $(LIBS)

clean:
	rm -f *.o

cleanall:
	rm -f *.o
	rm -f $(PROGRAM)
