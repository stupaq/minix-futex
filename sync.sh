#!/bin/bash

rsync -av --delete --delete-excluded --exclude='.git*' --exclude='sync.sh' ./ minix:~/futex/

