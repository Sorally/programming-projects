CC=g++
CFLAGS=-mwindows -Wall -O2
COMPILE = $(CC) $(CFLAGS) -c

main:
	$(CC) $(CFLAGS) main.c polarssl/md5.c polarssl/sha1.c polarssl/xtea.c -o main.exe
	
clean:
	rm -f *.o main.exe