#!/bin/sh

files="*.txt"

if [ -n "$*" ]; then
	files="$@"
fi

for f in $files; do
	grep ^# $f > _temp.comments
	grep -v ^# $f | grep -v '^$' | sort -u > _temp.data
	cat _temp.comments _temp.data > $f
	rm _temp.comments _temp.data
done
