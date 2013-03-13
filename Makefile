CC = gcc
CFLAGS = -W -Wall -O2 -std=c99 -pedantic
LIB-DIR = /usr/local/lib
LIB-MATH = m
LIB-CUNIT = cunit
DEBUG = -DDEBUG

all: main

util.o: util.c util.h
	$(CC) $(CFLAGS) $< -c -o util.o

main: main.c md5.o md5-test.o sha1.o sha1-test.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $< -o hash md5.o md5-test.o sha1.o sha1-test.o util.o -l$(LIB-CUNIT)

#### MD5 ####
md5: md5-main.c md5.o md5-test.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $< md5.o md5-test.o util.o -o md5 -l$(LIB-MATH) -l$(LIB-CUNIT)

md5-debug: md5-main.c md5-debug.o md5-test.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $< md5-debug.o md5-test.o util.o -o md5 $(DEBUG) -l$(LIB-MATH) -l$(LIB-CUNIT)

md5.o: md5.c md5.h util.o
	$(CC) $(CFLAGS) -c $< -o md5.o -DMD5

md5-debug.o: md5.c md5.h util.o
	$(CC) $(CFLAGS) -c $< -o md5-debug.o -DMD5 $(DEBUG)

md5-test.o: md5-test.c md5.o
	$(CC) $(CFLAGS) -c $< -o md5-test.o

#### SHA1 ####
sha1: sha1-main.c sha1.o sha1-test.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $< sha1.o sha1-test.o util.o -o sha1 -l$(LIB-MATH) -l$(LIB-CUNIT)

sha1-debug: sha1-main.c sha1-debug.o sha1-test.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $< sha1-debug.o sha1-test.o util.o -o sha1 $(DEBUG) -l$(LIB-MATH) -l$(LIB-CUNIT)

sha1.o: sha1.c sha.h util.o
	$(CC) $(CFLAGS) -c $< -o sha1.o -DSHA1

sha1-debug.o: sha1.c sha.h util.o
	$(CC) $(CFLAGS) -c $< -o sha1-debug.o -DSHA1 $(DEBUG)

sha1-test.o: sha1-test.c sha1.o
	$(CC) $(CFLAGS) -c $< -o sha1-test.o

#### SHA256 ####
sha256: main-sha256.c sha256.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $< sha256.o util.o -o sha256 -l$(LIB-MATH) -l$(LIB-CUNIT)

sha256-debug: main-sha256.c sha256-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $< sha256-debug.o util.o -o sha256 $(DEBUG) -l$(LIB-MATH) -l$(LIB-CUNIT)

sha256.o: sha256.c sha.h util.o
	$(CC) $(CFLAGS) -c $< -o sha256.o -DSHA256

sha256-debug.o: sha256.c sha.h util.o
	$(CC) $(CFLAGS) -c $< -o sha256-debug.o -DSHA256 $(DEBUG)

#### SHA224 ####
sha224: main-sha224.c sha224.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $< sha224.o util.o -o sha224 -l$(LIB-MATH) -l$(LIB-CUNIT)

sha224-debug: main-sha224.c sha224-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $< sha224-debug.o util.o -o sha224 $(DEBUG) -l$(LIB-MATH) -l$(LIB-CUNIT)

sha224.o: sha224.c sha.h util.o
	$(CC) $(CFLAGS) -c $< -o sha224.o -DSHA224

sha224-debug.o: sha224.c sha.h util.o
	$(CC) $(CFLAGS) -c $< -o sha224-debug.o -DSHA224 $(DEBUG)



.PHONY: clean

clean:
	rm -f *.exe *.o *.stackdump
