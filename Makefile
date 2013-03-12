CC = gcc
CFLAGS = -W -Wall -O2 -std=c99 -pedantic
LIB-MATH = m
DEBUG = -DDEBUG

all: sha1-debug

util.o: util.c util.h
	$(CC) $(CFLAGS) $< -c -o util.o -DMD5

#### MD5 ####
md5: main-md5.c md5.o
	$(CC) $(CFLAGS) -l$(LIB-MATH) $< md5.o util.o -o md5

md5-debug: main-md5.c md5-debug.o
	$(CC) $(CFLAGS) -l$(LIB-MATH) $< md5-debug.o util.o -o md5 $(DEBUG)

md5.o: md5.c md5.h util.o
	$(CC) $(CFLAGS) -c $< -o md5.o -DMD5

md5-debug.o: md5.c md5.h util.o
	$(CC) $(CFLAGS) -c $< -o md5-debug.o -DMD5 $(DEBUG)

#### SHA1 ####
sha1: main-sha1.c sha1.o
	$(CC) $(CFLAGS) -l$(LIB-MATH) $< sha1.o util.o -o sha1

sha1-debug: main-sha1.c sha1-debug.o
	$(CC) $(CFLAGS) -l$(LIB-MATH) $< sha1-debug.o util.o -o sha1 $(DEBUG)

sha1.o: sha1.c sha.h util.o
	$(CC) $(CFLAGS) -c $< -o sha1.o -DSHA1

sha1-debug.o: sha1.c sha.h util.o
	$(CC) $(CFLAGS) -c $< -o sha1-debug.o -DSHA1 $(DEBUG)

#### SHA256 ####
sha256: main-sha256.c sha256.o
	$(CC) $(CFLAGS) -l$(LIB-MATH) $< sha256.o util.o -o sha256

sha256-debug: main-sha256.c sha256-debug.o
	$(CC) $(CFLAGS) -l$(LIB-MATH) $< sha256-debug.o util.o -o sha256 $(DEBUG)

sha256.o: sha256.c sha.h util.o
	$(CC) $(CFLAGS) -c $< -o sha256.o -DSHA256

sha256-debug.o: sha256.c sha.h util.o
	$(CC) $(CFLAGS) -c $< -o sha256-debug.o -DSHA256 $(DEBUG)



.PHONY: clean

clean:
	rm -f *.exe *.o *.stackdump
