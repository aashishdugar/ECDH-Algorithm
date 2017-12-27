CC ?= cc
RM ?= rm -f

.PHONY: all clean

all: ecdh-openssl ecdh

ecdh: ecdh.c ecdh.h
	$(CC) $(CFLAGS) -Wall -o ecdh ecdh.c -lgmp

ecdh-openssl: ecdh-openssl.c
	$(CC) $(CFLAGS) -Wall -o ecdh-openssl -lssl -lcrypto ecdh-openssl.c

clean:
	$(RM) ecdh-openssl ecdh
