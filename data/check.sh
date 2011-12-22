#!/bin/sh -e

files="ru.txt"

if [ -n "$*" ]; then
	files="$@"
fi

for f in $files; do
	echo "===> $f"
	grep -E '.#.*(XXX|FIXME)' $f && echo "\--------- XXX/FIXME items found"
	sed -e 's| *#.*$||' < $f | grep -Eiv '[XIV]+.*съезда' | grep '[acekmopuxyABCEHKMOPTXY]' | grep '.' && echo "\--------- Non-cyrillic letters found"
	sed -e 's| *#.*$||' < $f | uniq -d | grep '.' && echo "\--------- Duplicates found"
	for x in улица переулок проезд проспект аллея площадь; do
		grep -E "^[^#]*$x[^#]*$x" $f | grep -v 'Автопроезд' | grep '.' && echo "\--------- likely, newline missing"
	done
done
