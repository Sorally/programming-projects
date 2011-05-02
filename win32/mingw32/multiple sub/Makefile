# Name: server.c
# Creation Date: March, 2011
# Author: r0B
 
CC=gcc
FLAGS=
LIBS=
EXE_SUFFIX=.exe
NAME=test
PROGRAM=$(NAME)$(EXE_SUFFIX)

all: $(PROGRAM)

#.c.o:
#	$(CC) $(CFLAGS) -c $<

$(PROGRAM): $(NAME).o mod1.o mod2.o
	$(CC) -o $(PROGRAM) $(NAME).o mod1.o mod2.o $(LIBS)

mod1.o:
	$(CC) $(CFLAGS) -c sub/mod1.c

mod2.o:
	$(CC) $(CFLAGS) -c sub/mod2.c

strip: $(PROGRAM)
	strip $(PROGRAM)

clean:
	rm -f *.o $(PROGRAM)
	
