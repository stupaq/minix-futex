#!/usr/bin/env bash

cd ./tests

make all

echo "--- tests ---"

for t in ./*
do
	if [ -x $t ]
	then
		./$t;
		if [ $? -eq 0 ]
		then
			echo "$t passed";
		else
			echo "$t failed";
		fi
	fi
done

