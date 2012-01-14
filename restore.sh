#!/usr/bin/env bash

FILES="`find modified/ -type f`"
for f in $FILES
do
	DEST="`echo $f | cut -b 9-`"
	rm -f "$DEST"
	echo $DEST
	if [ -f "backup$DEST" ]
	then
		cp -f "backup$DEST" "$DEST"
	fi
done
