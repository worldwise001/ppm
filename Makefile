CFLAGS=-c -Wall -Werror
LDFLAGS=
CORE_SOURCES=data.c node.c nodelist.c trie.c
SOURCES=$(CORE_SOURCES) main_pan13.c main_bobicev.c main_ppm.c test.c
CORE_OBJECTS=$(CORE_SOURCES:.c=.o)
OBJECTS=$(SOURCES:.c=.o)

all: pan13 bobicev ppm

pan13: core
	gcc $(LDFLAGS) $(CORE_OBJECTS) main_pan13.o -o $@

bobicev: core
	gcc $(LDFLAGS) $(CORE_OBJECTS) main_bobicev.o -o $@

ppm: core
	gcc $(LDFLAGS) $(CORE_OBJECTS) main_ppm.o -o $@

test: core
	gcc $(LDFLAGS) $(CORE_OBJECTS) test.o -o $@
    
core: $(OBJECTS) 

.c.o:
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm *.o
