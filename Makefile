G++ = g++ -std=c++11

bin/main: build/huffmanCompression.o build/main.o \
	build/imageIOHelper.o build/imageIOException.o \
	build/encodeFile.o build/image.o build/hfmCodeBitSet.o
	$(G++) -o bin/main build/* -g

build/main.o: src/main.cpp
	$(G++) -c -o build/main.o src/main.cpp -I ./include

build/imageIOHelper.o: src/imageIOHelper.cpp include/imageIOHelper.h
	$(G++) -c -o build/imageIOHelper.o src/imageIOHelper.cpp -g -I ./include

build/imageIOException.o: src/imageIOException.cpp include/imageIOException.h
	$(G++) -c -o build/imageIOException.o src/imageIOException.cpp -g -I ./include

build/encodeFile.o: src/encodeFile.cpp include/encodeFile.h
	$(G++) -c -o build/encodeFile.o src/encodeFile.cpp -g -I ./include

build/image.o: src/image.cpp include/image.h
	$(G++) -c -o build/image.o src/image.cpp -g -I ./include

build/huffmanCompression.o: src/huffmanCompression.cpp include/huffmanCompression.h
	$(G++) -c -o build/huffmanCompression.o src/huffmanCompression.cpp -g -I ./include

build/hfmCodeBitSet.o: src/hfmCodeBitSet.cpp include/hfmCodeBitSet.h
	$(G++) -c -o build/hfmCodeBitSet.o src/hfmCodeBitSet.cpp -g -I ./include

clean:
	rm build/* bin/*
