#!/bin/sh

if [ "$1" = "-kmeans" ]
then
        ./kmeans $2 $3
elif [ "$1" = "-dbscan" ]
then
        ./DbScan $2 $3 $4
elif [ "$1" = "-optics" ]
then
        python myOptics-fast.py -optics $2 $3 $4
fi
