#!/bin/bash

maxnums="1400000"
startnum="500000"
numinc="100000"
maxthreads="4"
startthread="1"
iters="5"

writefile="testmpi.txt"

cat /dev/null > $writefile

# Perform tests up to maxthreads and maxnums
# Increase $nums linearly by $numinc
# Increase $threads by powers of 2
thread=$startthread
while [ $thread -le $maxthreads ]
do
	num=$startnum
	while [ $num -le $maxnums ]
    do
        printf "%d threads, %d nums\n" $thread $num
		time=`mpirun -np $thread psum_mpi $num $iters 2>/dev/null`
        printf "%f\t" $time >> $writefile
		wait
		num=$[$num+$numinc]
	done
	thread=$[$thread*2]
	printf "\n" >> $writefile
done
