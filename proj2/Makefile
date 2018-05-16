CC=g++
CFLAGS=-ggdb -std=c++11 -Wall -pedantic

H_FILES=mmTest.hpp timer.hpp
FILES=mm-main.cpp mmTest.cpp timer.cpp mm-latency.cpp

all: mm_test

mm_test: $(FILES) $(H_FILES)
	$(CC) $(CFLAGS) -o mm_test $(FILES)

clean: rm -rf mm_test
