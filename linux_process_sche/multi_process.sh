#!/bin/bash
# do done之间为for的循环体，每次循环执行一次
for((i=0;i<5;i++)) do 
{
    sleep 5; 
    echo 1>>aa && echo "done!";
} &
done
wait
cat aa | wc -l
rm aa