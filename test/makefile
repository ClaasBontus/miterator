CppOpts= -g -O2 -Wall -Wshadow -Winline -std=c++1y -fdiagnostics-color=auto
CC= g++
LOpts=
HEADERS=../miterator.hpp ../stream_wrapper.hpp

.cpp.o:
	$(CC) -c $(CppOpts) $< -o $@

.o:
	$(CC) $^ $(LOpts) -o $@

all: miterator_test01

clean:
	rm -f *.o miterator_test01

miterator_test01: miterator_test01.o

miterator_test01.o: miterator_test01.cpp $(HEADERS)
