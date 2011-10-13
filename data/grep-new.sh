#!/bin/sh

( cat ru.txt | sed -e 's|#.*||' | awk '{print;print}'; cat "$1") | sort | uniq -u
