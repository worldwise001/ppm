CFLAGS=-c -Wall -Werror -std=c99 -ggdb
LDFLAGS=-lm
CORE_SOURCES=data.c node.c nodelist.c trie.c
SOURCES=$(CORE_SOURCES) main_pan13.c main_bobicev.c main_ppm.c test.c
CORE_OBJECTS=$(CORE_SOURCES:.c=.o)
OBJECTS=$(SOURCES:.c=.o)

all: pan13 bobicev ppm

pan13: core
	gcc $(CORE_OBJECTS) main_pan13.o -o $@ $(LDFLAGS)

bobicev: core
	gcc $(CORE_OBJECTS) main_bobicev.o -o $@ $(LDFLAGS)

ppm: core
	gcc $(CORE_OBJECTS) main_ppm.o -o $@ $(LDFLAGS)

test: core
	gcc $(CORE_OBJECTS) test.o -o $@ $(LDFLAGS)
    
core: $(OBJECTS) 

.c.o:
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm *.o
