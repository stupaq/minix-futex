#!/bin/bash

# sync
ssh minix 'rm -rf ~/zad3/*'
scp -q -r ./ minix:~/zad3/
ssh minix 'rm -rf ~/zad3/.git*'

# make
#ssh minix 'cd ~/zad3/; make; make compile;'

# reboot
#ssh minix 'reboot'

#rsync -a -e 'ssh -p 2222 -l root' --exclude=".git*" --exclude="${0}" ./ 127.0.0.1:~/

