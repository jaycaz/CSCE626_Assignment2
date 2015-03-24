#!/bin/bash

maxnums="512"
startnum="1"
maxthreads="4"
startthread="1"
iters="50"

writefile=$1

cat /dev/null > test.txt

# Perform tests up to maxthreads and maxnums
thread=$startthread
while [ $thread -le $maxthreads ]
do
	num=$startnum
	while [ $num -le $maxnums ]
    do
        printf "%d threads, %d nums\n" $thread $num
		time=`./psum_openmp $thread $num $iters`
        printf "%f\t" $time >> $writefile
		wait
		num=$[$num*2]
	done
	thread=$[$thread+1]
	printf "\n" >> $writefile
done
