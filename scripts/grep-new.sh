#!/bin/sh -u

. "`dirname $0`/common.sh"

( cat $all_files | sed p; cat "$1" ) | sed -e 's| *#.*||' | sort | uniq -u
