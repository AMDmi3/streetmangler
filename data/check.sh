#!/bin/sh -e

files="ru.txt"

if [ -n "$*" ]; then
	files="$@"
fi

for f in $files; do
	echo "===> $f"
	sed -e 's| *#.*$||' < $f | grep '[acekmopuxyABCEHKMOPTXY]' && echo ">-- Non-cyrillic letters found"
	sed -e 's| *#.*$||' < $f | uniq -d | grep '.' && echo ">-- Duplicates found"
	grep -E '.#.*(XXX|FIXME)' $f && echo ">-- XXX/FIXME items found"
done
