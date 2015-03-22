#!/bin/bash

maxnums="512"
startnum="512"
maxthreads="4"
thread="4"
iters="50"

cat /dev/null > test.txt

# Perform tests up to maxthreads and maxnums
while [ $thread -le $maxthreads ]
do
	num="1"
	while [ $num -le $maxnums ]
	do
		./psum_openmp $num $thread $iters >> test.txt
		wait
		num=$[$num*2]
	done
	thread=$[$thread+1]
	printf "\n" >> test.txt
done
