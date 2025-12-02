#!/bin/bash
g++ -std=c++14 -I ${mkEigenInc} test.cpp -o test && ./test

