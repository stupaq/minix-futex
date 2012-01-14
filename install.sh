#!/usr/bin/env bash

FILES="`find modified/ -type f`"
for f in $FILES
do
	DEST="`echo $f | cut -b 9-`"
	mkdir -p "`dirname $DEST`"
	echo $f "->" $DEST
	cp -f "$f" "$DEST"
done
