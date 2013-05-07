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

hmac.o: hmac.c hmac.h
	$(CC) $(CFLAGS) $< -c -o $@

hmac-debug.o: hmac.c hmac.h
	$(CC) $(CFLAGS) $< -c -o $@ $(DEBUG)

main: main.c \
		md2.o md2-test.o \
		md4.o md4-test.o \
		md5.o md5-test.o \
		sha1.o sha1-test.o \
		sha224.o sha224-test.o \
		sha256.o sha256-test.o \
		sha384.o sha384-test.o \
		sha512.o sha512-test.o \
		sha512-224.o \
		sha512-256.o \
		util.o \
		hmac.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o hash -l$(LIB-CUNIT)

debug: main.c \
		md2-debug.o md2-test-debug.o \
		md4-debug.o md4-test-debug.o \
		md5-debug.o md5-test-debug.o \
		sha1-debug.o sha1-test-debug.o \
		sha224-debug.o sha224-test-debug.o \
		sha256-debug.o sha256-test-debug.o \
		sha384-debug.o sha384-test-debug.o \
		sha512-debug.o sha512-test-debug.o \
		sha512-224-debug.o \
		sha512-256-debug.o \
		util-debug.o \
		hmac-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o hash -l$(LIB-CUNIT) $(DEBUG)
	
#### MD2 ####
md2: md2-main.c md2.o md2-test.o util.o hmac.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) -DMD2

md2-debug: md2-main.c md2-debug.o md2-test-debug.o util-debug.o hmac-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) -DMD2 $(DEBUG)

md2.o: md2.c md.h
	$(CC) $(CFLAGS) -c $< -o $@ -DMD2

md2-debug.o: md2.c md.h
	$(CC) $(CFLAGS) -c $< -o $@ -DMD2 $(DEBUG)

md2-test.o: md2-test.c md2.o hmac.o
	$(CC) $(CFLAGS) -c $< -o $@ -DMD2

md2-test-debug.o: md2-test.c md2-debug.o hmac-debug.o
	$(CC) $(CFLAGS) -c $< -o $@ -DMD2 $(DEBUG)

#### MD4 ####
md4: md4-main.c md4.o md4-test.o util.o hmac.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) -DMD4

md4-debug: md4-main.c md4-debug.o md4-test-debug.o util-debug.o hmac-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) -DMD4 $(DEBUG)

md4.o: md4.c md.h
	$(CC) $(CFLAGS) -c $< -o $@ -DMD4

md4-debug.o: md4.c md.h
	$(CC) $(CFLAGS) -c $< -o $@ -DMD4 $(DEBUG)

md4-test.o: md4-test.c md4.o hmac.o
	$(CC) $(CFLAGS) -c $< -o $@ -DMD4

md4-test-debug.o: md4-test.c md4-debug.o hmac-debug.o
	$(CC) $(CFLAGS) -c $< -o $@ -DMD4 $(DEBUG)

#### MD5 ####
md5: md5-main.c md5.o md5-test.o util.o hmac.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) -DMD5

md5-debug: md5-main.c md5-debug.o md5-test-debug.o util-debug.o hmac-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) -DMD5 $(DEBUG)

md5.o: md5.c md.h
	$(CC) $(CFLAGS) -c $< -o $@ -DMD5

md5-debug.o: md5.c md.h
	$(CC) $(CFLAGS) -c $< -o $@ -DMD5 $(DEBUG)

md5-test.o: md5-test.c md5.o hmac.o
	$(CC) $(CFLAGS) -c $< -o $@ -DMD5

md5-test-debug.o: md5-test.c md5-debug.o hmac-debug.o
	$(CC) $(CFLAGS) -c $< -o $@ -DMD5 $(DEBUG)

#### SHA1 ####
sha1: sha1-main.c sha1.o sha1-test.o util.o hmac.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) -DSHA1

sha1-debug: sha1-main.c sha1-debug.o sha1-test-debug.o util-debug.o hmac-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) -DSHA1 $(DEBUG)

sha1.o: sha1.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA1

sha1-debug.o: sha1.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA1 $(DEBUG)

sha1-test.o: sha1-test.c sha1.o hmac.o
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA1

sha1-test-debug.o: sha1-test.c sha1-debug.o hmac-debug.o
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA1 $(DEBUG)

#### SHA256 ####
sha256: sha256-main.c sha256.o sha256-test.o util.o hmac.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) -DSHA256

sha256-debug: sha256-main.c sha256-debug.o sha256-test-debug.o util-debug.o hmac-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) -DSHA256 $(DEBUG)

sha256.o: sha256.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA256

sha256-debug.o: sha256.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA256 $(DEBUG)

sha256-test.o: sha256-test.c sha256.o hmac.o
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA256

sha256-test-debug.o: sha256-test.c sha256-debug.o hmac-debug.o
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA256 $(DEBUG)

#### SHA224 ####
sha224: sha224-main.c sha224.o sha224-test.o util.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT)

sha224-debug: sha224-main.c sha224-debug.o sha224-test-debug.o util-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) $(DEBUG)

sha224.o: sha224.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA224

sha224-debug.o: sha224.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA224 $(DEBUG)

sha224-test.o: sha224-test.c sha1.o
	$(CC) $(CFLAGS) -c $< -o $@

sha224-test-debug.o: sha224-test.c sha224-debug.o
	$(CC) $(CFLAGS) -c $< -o $@ $(DEBUG)

#### SHA384 ####
sha384: sha384-main.c sha384.o sha384-test.o util.o hmac.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT)

sha384-debug: sha384-main.c sha384-debug.o sha384-test-debug.o util-debug.o hmac-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) $(DEBUG)

sha384.o: sha384.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA384

sha384-debug.o: sha384.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA384 $(DEBUG)

sha384-test.o: sha384-test.c sha384.o hmac.o
	$(CC) $(CFLAGS) -c $< -o $@

sha384-test-debug.o: sha384-test.c sha384-debug.o hmac-debug.o
	$(CC) $(CFLAGS) -c $< -o $@ $(DEBUG)

#### SHA512 ####
sha512: sha512-main.c sha512.o sha512-test.o util.o hmac.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT)

sha512-debug: sha512-main.c sha512-debug.o sha512-test-debug.o util-debug.o hmac-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) $(DEBUG)

sha512.o: sha512.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA512

sha512-debug.o: sha512.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA512 $(DEBUG)

sha512-test.o: sha512-test.c sha512.o hmac.o
	$(CC) $(CFLAGS) -c $< -o $@

sha512-test-debug.o: sha512-test.c sha512-debug.o hmac-debug.o
	$(CC) $(CFLAGS) -c $< -o $@ $(DEBUG)

#### SHA512/224 ####
sha512-224: sha512-224-main.c sha512-224.o sha512-224-test.o sha512.o util.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT)

sha512-224-debug: sha512-224-main.c sha512-224-debug.o sha512-224-test-debug.o sha512-debug.o util-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) $(DEBUG)

sha512-224.o: sha512-224.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA512224

sha512-224-debug.o: sha512-224.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA512224 $(DEBUG)

sha512-224-test.o: sha512-224-test.c sha512-224.o
	$(CC) $(CFLAGS) -c $< -o $@

sha512-224-test-debug.o: sha512-224-test.c sha512-224-debug.o
	$(CC) $(CFLAGS) -c $< -o $@ $(DEBUG)

#### SHA512/256 ####
sha512-256: sha512-256-main.c sha512-256.o sha512-256-test.o sha512.o util.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT)

sha512-256-debug: sha512-256-main.c sha512-256-debug.o sha512-256-test-debug.o sha512-debug.o util-debug.o
	$(CC) $(CFLAGS) -L$(LIB-DIR) $^ -o $@ -l$(LIB-MATH) -l$(LIB-CUNIT) $(DEBUG)

sha512-256.o: sha512-256.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA512256

sha512-256-debug.o: sha512-256.c sha.h
	$(CC) $(CFLAGS) -c $< -o $@ -DSHA512256 $(DEBUG)

sha512-256-test.o: sha512-256-test.c sha512-256.o
	$(CC) $(CFLAGS) -c $< -o $@

sha512-256-test-debug.o: sha512-256-test.c sha512-256-debug.o
	$(CC) $(CFLAGS) -c $< -o $@ $(DEBUG)


.PHONY: clean nuke

clean:
	cp -p hash.exe hash.out
	rm -f *.exe *.o *.stackdump
	cp -p hash.out hash.exe
	rm -rf hash.out

nuke:
	rm -f *.exe *.o *.stackdump
	