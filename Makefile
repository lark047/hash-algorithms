CC = gcc
CFLAGS = -W -Wall -O2 -std=c99 -pedantic
LIB-DIR = /usr/local/lib
LIB-MATH = m
LIB-CUNIT = cunit
DEBUG = -DDEBUG

all: sha1

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
	$(CC) $(CFLAGS) -L$(LIB-DIR) $< sha1.o util.o -o sha1 -l$(LIB-MATH) -l$(LIB-CUNIT)

sha1-debug: main-sha1.c sha1-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $< sha1-debug.o util.o -o sha1 $(DEBUG) -l$(LIB-MATH) -l$(LIB-CUNIT)

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

#### SHA224 ####
sha224: main-sha224.c sha224.o
	$(CC) $(CFLAGS) -l$(LIB-MATH) $< sha224.o util.o -o sha224

sha224-debug: main-sha224.c sha224-debug.o
	$(CC) $(CFLAGS) -l$(LIB-MATH) $< sha224-debug.o util.o -o sha224 $(DEBUG)

sha224.o: sha224.c sha.h util.o
	$(CC) $(CFLAGS) -c $< -o sha224.o -DSHA224

sha224-debug.o: sha224.c sha.h util.o
	$(CC) $(CFLAGS) -c $< -o sha224-debug.o -DSHA224 $(DEBUG)



.PHONY: clean

clean:
	rm -f *.exe *.o *.stackdump
