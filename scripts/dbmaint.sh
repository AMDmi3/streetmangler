#!/bin/sh -u

datadir="`dirname $0`/../data"

all_files="
	$datadir/ru/areas.txt
	$datadir/ru/exceptions.txt
	$datadir/ru/main.txt
	$datadir/ru_BY.txt
	$datadir/ru_RU.txt
	"

sort_file() {
	local file="$1"
	local tmpcomments="_dbmaint_tmp_comments"
	local tmpdata="_dbmaint_tmp_data"

	grep ^# $file > $tmpcomments
	grep -v ^# $file | grep -v '^$' | sort -u > $tmpdata
	cat $tmpcomments $tmpdata > $file
	rm -f $tmpcomments $tmpdata
}

check_files() {
	local tmpall="_dbmaint_tmp_all"
	local tmpfixme="_dbmaint_tmp_fixme"
	local tmpnoncyr="_dbmaint_tmp_noncyr"
	local tmpdup="_dbmaint_tmp_dup"
	local tmpdupstat="_dbmaint_tmp_dupstat"

	cat "$@" | sort | grep -v ^.include > $tmpall
	cat $tmpall | grep -E '.#.*(XXX|FIXME)' > $tmpfixme
	cat $tmpall | sed -e 's| *#.*$||' | grep -Eiv '[XIV]+.*съезда' | grep '[acekmopuxyABCEHKMOPTXY]' > $tmpnoncyr
	cat $tmpall | sed -e 's| *#.*$||' | uniq -d | grep -v '^$' > $tmpdup
	rm -f $tmpdupstat

	for x in улица переулок проезд проспект аллея площадь; do
		cat $tmpall | grep -E "^[^#]*$x[^#]*$x" | grep -v 'Автопроезд' >> $tmpdupstat
	done

	if [ -s "$tmpfixme" ]; then
		echo "---[ XXX/FIXME items ]---"
		cat "$tmpfixme"
		echo "---[ End of XXX/FIXME items ]---"
	fi
	if [ -s "$tmpnoncyr" ]; then
		echo "---[ Non-cyrillic letters ]---"
		cat "$tmpnoncyr"
		echo "---[ End of non-cyrillic letters ]---"
	fi
	if [ -s "$tmpdup" ]; then
		echo "---[ Duplicates ]---"
		cat "$tmpdup"
		echo "---[ End of duplicates ]---"
	fi
	if [ -s "$tmpdupstat" ]; then
		echo "---[ Duplicate status ]---"
		cat "$tmpdupstat"
		echo "---[ End of duplicate status ]---"
	fi

	rm -f $tmpall $tmpfixme $tmpnoncyr $tmpdup $tmpdupstat
}

for f in $all_files; do
	sort_file "$f"
done

check_files $all_files
