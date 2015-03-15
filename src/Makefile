CFLAGS=-c -Wall -Werror -std=c99 -ggdb -I../lib
LDFLAGS=-lm
SOURCES=main_pan13.c main_bobicev.c main_ppm.c test.c
OBJECTS=$(SOURCES:.c=.o)

all: pan13 bobicev ppm test

pan13: $(OBJECTS)
	make -C ../lib
	$(CC) $(LDFLAGS) main_pan13.o ../lib/libppm.a -o $@ $(LDFLAGS)

bobicev: $(OBJECTS)
	make -C ../lib
	$(CC) $(LDFLAGS) main_bobicev.o ../lib/libppm.a -o $@ $(LDFLAGS)

ppm: $(OBJECTS)
	make -C ../lib
	$(CC) $(LDFLAGS) main_ppm.o ../lib/libppm.a -o $@ $(LDFLAGS)

test: $(OBJECTS)
	make -C ../lib
	$(CC) $(LDFLAGS) test.o ../lib/libppm.a -o $@ $(LDFLAGS)
    
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o pan13 bobicev ppm test
