# Name: main.c
# Creation Date: April, 2011
# Author: r0B
 
CC=gcc
FLAGS=-Wall
LIBS=-lws2_32
EXE_SUFFIX=.exe
NAME=main
PROGRAM=$(NAME)$(EXE_SUFFIX)
PROGRAM=main.exe

all: $(PROGRAM)

$(PROGRAM): $(NAME).o
	$(CC) $(FLAGS) -o $(PROGRAM) $(NAME).o $(LIBS)

clean:
	rm -f *.o $(PROGRAM)
