CC=g++
CFLAGS=-ggdb -std=c++11 -Wall -pedantic

H_FILES=osservice.hpp
FILES=osservice.cpp cpu-main.cpp

all: cpu_test

cpu_test: $(FILES) $(H_FILES)
	$(CC) $(CFLAGS) -o cpu_test $(FILES)

clean: rm -rf cpu_test
