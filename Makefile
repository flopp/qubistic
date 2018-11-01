all: build
	cd build ; qmake ../qubistic.pro
	cd build ; make -j4

clean:
	rm -rf build

build:
	mkdir -p build
