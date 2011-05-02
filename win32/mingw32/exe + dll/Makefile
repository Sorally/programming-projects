# Name: server.c
# Creation Date: March, 2011
# Author: r0B
 
CC=gcc
FLAGS=
LIBS=


dll.dll: dll.o
#	$(CC) -o $(PROGRAM) $(NAME).o mod1.o mod2.o $(LIBS)
	$(CC) -shared -o dll.dll dll.o -Wl,--out-implib,libdll.a

dll.o:
	$(CC) -c dll.c

test: 
	$(CC) -c test.c
	$(CC) -o test.exe test.o -L. -ldll

clean:
	rm -f *.o

cleanall:
	rm -f *.o *.dll *.a *.exe
	
