#!/bin/sh

#g++ -std=c++14 -O3 -o DbScan DbScan.cc -IANN/include -LANN/lib -lANN
g++ -std=c++14 -O3 -o DbScan DbScan_MTree.cc
gcc -o  kmeans source.c -lm

