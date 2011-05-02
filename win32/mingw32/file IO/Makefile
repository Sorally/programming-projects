# Name: server.c
# Creation Date: March, 2011
# Author: r0B
 
CC=gcc
FLAGS=
LIBS=
EXE_SUFFIX=.exe
NAME=main
PROGRAM=$(NAME)$(EXE_SUFFIX)

all: $(PROGRAM)

#.c.o:
#	$(CC) $(CFLAGS) -c $<

$(PROGRAM): $(NAME).o
	$(CC) -o $(PROGRAM) $(NAME).o $(LIBS)

clean:
	rm -f *.o $(PROGRAM)
	
