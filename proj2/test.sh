#!/bin/bash
make clean
make
rm pageFaultTestFile

./mm_test
