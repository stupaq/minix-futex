#!/usr/bin/env bash

FILES="`find modified/ -type f`"
for f in $FILES
do
	DEST="`echo $f | cut -b 9-`"
	mkdir -p "backup`dirname $DEST`"
	echo $DEST "->" backup$DEST
	if [ -f "$DEST" ]
	then
		cp -f "$DEST" backup$DEST
	fi
done
