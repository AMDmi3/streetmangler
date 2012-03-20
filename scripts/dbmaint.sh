#!/bin/sh -u

. "`dirname $0`/common.sh"

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
	local tmparea="_dbmaint_tmp_area"
	local tmpnonarea="_dbmaint_tmp_nonarea"
	local tmpmultistatus="_dbmaint_tmp_multistatus"
	local tmpnomultistatus="_dbmaint_tmp_nomultistatus"

	cat "$@" | sort | grep -v ^.include > $tmpall
	cat $tmpall | grep -E '.#.*(XXX|FIXME)' > $tmpfixme
	cat $tmpall | sed -e 's| *#.*$||' | grep -Eiv '[XIV]+.*съезда' | grep '[acekmopuxyABCEHKMOPTXY]' > $tmpnoncyr
	cat $tmpall | sed -e 's| *#.*$||' | uniq -d | grep -v '^$' > $tmpdup

	cat `echo $@ | xargs -n1 echo | grep area` | grep -E "^($nonarea_statuses) " > $tmparea
	cat `echo $@ | xargs -n1 echo | grep area` | grep -E " ($nonarea_statuses)$" >> $tmparea
	cat `echo $@ | xargs -n1 echo | grep -v area` | grep -E "^($area_statuses) " > $tmpnonarea
	cat `echo $@ | xargs -n1 echo | grep -v area` | grep -E " ($area_statuses)$" >> $tmpnonarea

	cat `echo $@ | xargs -n1 echo | grep multistatus` | grep -v '^#' | sed -e 's| *#.*$||' | grep -viE "\b($nonarea_statuses)\b.*\b($nonarea_statuses)\b" > $tmpnomultistatus
	cat `echo $@ | xargs -n1 echo | grep -v multistatus` | grep -v '^#' | sed -e 's| *#.*$||' | grep -viE '(линия.*шоссе|Мичуринский Проспект)' | grep -iE "\b($nonarea_statuses)\b.*\b($nonarea_statuses)\b" > $tmpmultistatus

	rm -f $tmpdupstat
	for x in `echo "$nonarea_statuses" | tr '|' ' '`; do
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
	if [ -s "$tmparea" ]; then
		echo "---[ Nonarea status in areas file ]---"
		cat "$tmparea"
		echo "---[ End of nonarea status in areas file ]---"
	fi
	if [ -s "$tmpnonarea" ]; then
		echo "---[ Area status in nonareas file ]---"
		cat "$tmpnonarea"
		echo "---[ End of area status in nonareas file ]---"
	fi
	if [ -s "$tmpmultistatus" ]; then
		echo "---[ Multiple statuses in non-multistatuss file ]---"
		cat "$tmpmultistatus"
		echo "---[ End of multiple statuses in non-multistatuss file ]---"
	fi
	if [ -s "$tmpnomultistatus" ]; then
		echo "---[ Non-multiple status in multistatuss file ]---"
		cat "$tmpnomultistatus"
		echo "---[ End of non-multiple status in multistatuss file ]---"
	fi

	rm -f $tmpall $tmpfixme $tmpnoncyr $tmpdup $tmpdupstat $tmparea $tmpnonarea $tmpmultistatus $tmpnomultistatus
}

for f in $automaint_files; do
	sort_file "$f"
done

check_files $all_files
