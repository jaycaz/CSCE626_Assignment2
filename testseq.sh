#!/bin/bash

maxnums="1400000"
startnum="500000"
numinc="100000"
iters="50"

writefile="testseq.txt"

cat /dev/null > $writefile

# Perform tests up to maxthreads and maxnums
# Increase $nums linearly by $numinc
num=$startnum
while [ $num -le $maxnums ]
do
    printf "1 thread (seq), %d nums\n" $num
	time=`./psum_seq $num $iters`
    printf "%f\t" $time >> $writefile
	wait
	num=$[$num+$numinc]
done
printf "\n" >> $writefile
