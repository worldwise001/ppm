all: lib src py

clean:
	make -C lib clean
	make -C src clean
	make -C py clean

.PHONY: lib src py

lib:
	make -C lib

src:
	make -C src

py:
	make -C py
