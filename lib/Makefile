CC=gcc
AR=ar
CFLAGS=-c -Wall -Werror -std=c99 -ggdb
LDFLAGS=-lm
ARFLAGS=-cvq
SOURCES=data.c node.c nodelist.c trie.c
OBJECTS=$(SOURCES:.c=.o)

all: libppm.a

libppm.a: $(OBJECTS)
	$(AR) $(ARFLAGS) libppm.a *.o

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o libppm.a
