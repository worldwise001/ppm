all: ppm.so

clean:
	rm -r build ppm.so

ppm.so:
	python setup.py build
	cp build/lib*/ppm.so ./
