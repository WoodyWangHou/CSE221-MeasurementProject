#!/bin/bash
make clean
make
rm project1_result.txt

./cpu_test 2 >> ./project1_result.txt
