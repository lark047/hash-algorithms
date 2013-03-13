CC = gcc
CFLAGS = -W -Wall -O2 -std=c99 -pedantic
LIB-DIR = /usr/local/lib
LIB-MATH = m
LIB-CUNIT = cunit
DEBUG = -DDEBUG

all: main

util.o: util.c util.h
	$(CC) $(CFLAGS) $< -c -o $@

util-debug.o: util.c util.h
	$(CC) $(CFLAGS) $< -c -o $@ $(DEBUG)

main: main.c md5.o md5-test.o sha1.o sha1-test.o sha224.o sha224-test.o sha256.o sha256-test.o sha384.o sha384-test.o util.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o hash -l$(LIB-CUNIT)

#### MD5 ####
md5: md5-main.c md5.o md5-test.o util.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT)

md5-debug: md5-main.c md5-debug.o md5-test.o util-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) $(DEBUG)

md5.o: md5.c md5.h
	$(CC) $(CFLAGS) -c $< -o $@ -DMD5

md5-debug.o: md5.c md5.h
	$(CC) $(CFLAGS) -c $< -o $@ -DMD5 $(DEBUG)

md5-test.o: md5-test.c md5.o
	$(CC) $(CFLAGS) -c $< -o $@

#### SHA1 ####
sha1: sha1-main.c sha1.o sha1-test.o util.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT)

sha1-debug: sha1-main.c sha1-debug.o sha1-test.o util-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) $(DEBUG)

sha1.o: sha1.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA1

sha1-debug.o: sha1.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA1 $(DEBUG)

sha1-test.o: sha1-test.c sha1.o
	$(CC) $(CFLAGS) -c $< -o $@

#### SHA256 ####
sha256: sha256-main.c sha256.o sha256-test.o util.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT)

sha256-debug: sha256-main.c sha256-debug.o sha256-test.o util-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) $(DEBUG)

sha256.o: sha256.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA256

sha256-debug.o: sha256.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA256 $(DEBUG)

sha256-test.o: sha256-test.c sha256.o
	$(CC) $(CFLAGS) -c $< -o $@

#### SHA224 ####
sha224: sha224-main.c sha224.o sha224-test.o util.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT)

sha224-debug: sha224-main.c sha224-debug.o sha224-test.o util-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) $(DEBUG)

sha224.o: sha224.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA224

sha224-debug.o: sha224.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA224 $(DEBUG)

sha224-test.o: sha224-test.c sha1.o
	$(CC) $(CFLAGS) -c $< -o $@

#### SHA384 ####
sha384: sha384-main.c sha384.o sha384-test.o util.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT)

sha384-debug: sha384-main.c sha384-debug.o sha384-test.o util-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) $(DEBUG)

sha384.o: sha384.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA384

sha384-debug.o: sha384.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA384 $(DEBUG)

sha384-test.o: sha384-test.c sha384.o
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: clean

clean:
	rm -f *.exe *.o *.stackdump
