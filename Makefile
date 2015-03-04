all: lib src

clean:
	make -C lib clean
	make -C src clean

.PHONY: lib src

lib:
	make -C lib

src:
	make -C src
