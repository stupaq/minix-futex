#!/bin/bash

# sync
ssh minix 'rm -rf ~/futex/*'
scp -q -r ./ minix:~/futex/
ssh minix 'rm -rf ~/futex/.git*'

# reboot
#ssh minix 'reboot'

#rsync -a -e 'ssh -p 2222 -l root' --exclude=".git*" --exclude="${0}" ./ 127.0.0.1:~/

