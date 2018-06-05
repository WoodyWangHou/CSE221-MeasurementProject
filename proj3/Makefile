CC=g++
CFLAGS=-ggdb -std=c++11 -Wall -pedantic

H_FILES=nwFsTest.hpp timer.hpp
FILES=mm-main.cpp nwFsTest.cpp timer.cpp

all: nw_fs_test

nw_fs_test: $(FILES) $(H_FILES)
	$(CC) $(CFLAGS) -o nw_fs_test $(FILES)

clean: rm -rf nw_fs_test
